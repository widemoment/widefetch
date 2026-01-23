#include "modules/uptime.h"
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

static uint64_t filetime_to_u64(FILETIME ft) {
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return (uint64_t)u.QuadPart;
}

static FILETIME u64_to_filetime(uint64_t x) {
    ULARGE_INTEGER u;
    u.QuadPart = (ULONGLONG)x;
    FILETIME ft;
    ft.dwLowDateTime = u.LowPart;
    ft.dwHighDateTime = u.HighPart;
    return ft;
}

static int parse_cim_datetime_utc(const wchar_t* s, FILETIME* outUtcFt) {
    if (!s || wcslen(s) < 22) return 0;

    int y=0, mo=0, d=0, h=0, mi=0, se=0;
    if (swscanf(s, L"%4d%2d%2d%2d%2d%2d", &y, &mo, &d, &h, &mi, &se) != 6) return 0;

    const wchar_t* tz = s + 21;
    wchar_t sign = tz[0];
    int offMin = 0;
    if ((sign == L'+' || sign == L'-') && swscanf(tz + 1, L"%3d", &offMin) == 1) {
    } else {
        sign = L'+';
        offMin = 0;
    }

    SYSTEMTIME st;
    ZeroMemory(&st, sizeof(st));
    st.wYear = (WORD)y;
    st.wMonth = (WORD)mo;
    st.wDay = (WORD)d;
    st.wHour = (WORD)h;
    st.wMinute = (WORD)mi;
    st.wSecond = (WORD)se;

    int delta = offMin;
    if (sign == L'-') delta = -offMin;

    FILETIME ftLocalAsIfUtc;
    if (!SystemTimeToFileTime(&st, &ftLocalAsIfUtc)) return 0;

    uint64_t t = filetime_to_u64(ftLocalAsIfUtc);
    int64_t adj100ns = (int64_t)delta * 60LL * 10000000LL;
    int64_t utc = (int64_t)t - adj100ns;

    if (utc < 0) return 0;
    *outUtcFt = u64_to_filetime((uint64_t)utc);
    return 1;
}

static int wmi_last_boot_utc(FILETIME* outBootUtc) {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    int needUninit = SUCCEEDED(hr);

    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL
    );
    if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
        if (needUninit) CoUninitialize();
        return 0;
    }

    IWbemLocator* loc = NULL;
    hr = CoCreateInstance(&CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          &IID_IWbemLocator, (LPVOID*)&loc);
    if (FAILED(hr) || !loc) {
        if (needUninit) CoUninitialize();
        return 0;
    }

    IWbemServices* svc = NULL;
    hr = loc->lpVtbl->ConnectServer(
        loc,
        L"ROOT\\CIMV2",
        NULL, NULL, 0, 0, 0, 0,
        &svc
    );
    loc->lpVtbl->Release(loc);
    if (FAILED(hr) || !svc) {
        if (needUninit) CoUninitialize();
        return 0;
    }

    hr = CoSetProxyBlanket(
        (IUnknown*)svc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE
    );
    if (FAILED(hr)) {
        svc->lpVtbl->Release(svc);
        if (needUninit) CoUninitialize();
        return 0;
    }

    IEnumWbemClassObject* en = NULL;
    hr = svc->lpVtbl->ExecQuery(
        svc,
        L"WQL",
        L"SELECT LastBootUpTime FROM Win32_OperatingSystem",
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &en
    );
    if (FAILED(hr) || !en) {
        svc->lpVtbl->Release(svc);
        if (needUninit) CoUninitialize();
        return 0;
    }

    IWbemClassObject* obj = NULL;
    ULONG ret = 0;
    hr = en->lpVtbl->Next(en, 4000, 1, &obj, &ret);

    int ok = 0;
    if (SUCCEEDED(hr) && ret == 1 && obj) {
        VARIANT v;
        VariantInit(&v);

        hr = obj->lpVtbl->Get(obj, L"LastBootUpTime", 0, &v, NULL, NULL);
        if (SUCCEEDED(hr) && v.vt == VT_BSTR && v.bstrVal) {
            FILETIME bootUtc;
            if (parse_cim_datetime_utc(v.bstrVal, &bootUtc)) {
                *outBootUtc = bootUtc;
                ok = 1;
            }
        }

        VariantClear(&v);
        obj->lpVtbl->Release(obj);
    }

    en->lpVtbl->Release(en);
    svc->lpVtbl->Release(svc);
    if (needUninit) CoUninitialize();

    return ok;
}

int wf_get_uptime(char* out, size_t cap) {
    FILETIME bootUtc;
    if (!wmi_last_boot_utc(&bootUtc)) {
        uint64_t s = (uint64_t)(GetTickCount64() / 1000ULL);
        uint64_t d = s / 86400ULL; s %= 86400ULL;
        uint64_t h = s / 3600ULL;  s %= 3600ULL;
        uint64_t m = s / 60ULL;

        if (d) snprintf(out, cap, "%llu days, %llu hours, %llu mins",
                        (unsigned long long)d, (unsigned long long)h, (unsigned long long)m);
        else   snprintf(out, cap, "%llu hours, %llu mins",
                        (unsigned long long)h, (unsigned long long)m);
        return 1;
    }

    FILETIME nowUtcFt;
    GetSystemTimeAsFileTime(&nowUtcFt);

    uint64_t now = filetime_to_u64(nowUtcFt);
    uint64_t boot = filetime_to_u64(bootUtc);
    if (now < boot) {
        snprintf(out, cap, "Unknown");
        return 0;
    }

    uint64_t diff100ns = now - boot;
    uint64_t s = diff100ns / 10000000ULL;

    uint64_t d = s / 86400ULL; s %= 86400ULL;
    uint64_t h = s / 3600ULL;  s %= 3600ULL;
    uint64_t m = s / 60ULL;

    if (d) snprintf(out, cap, "%llu days, %llu hours, %llu mins",
                    (unsigned long long)d, (unsigned long long)h, (unsigned long long)m);
    else   snprintf(out, cap, "%llu hours, %llu mins",
                    (unsigned long long)h, (unsigned long long)m);

    return 1;
}
