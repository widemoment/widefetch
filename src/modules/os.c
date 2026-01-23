#include "modules/os.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef LONG (WINAPI *RtlGetVersionFn)(PRTL_OSVERSIONINFOW);

static int rtl_version(DWORD* maj, DWORD* min, DWORD* build) {
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll) return 0;

    RtlGetVersionFn fn = (RtlGetVersionFn)GetProcAddress(ntdll, "RtlGetVersion");
    if (!fn) return 0;

    RTL_OSVERSIONINFOW vi;
    ZeroMemory(&vi, sizeof(vi));
    vi.dwOSVersionInfoSize = sizeof(vi);

    if (fn(&vi) != 0) return 0;

    if (maj) *maj = vi.dwMajorVersion;
    if (min) *min = vi.dwMinorVersion;
    if (build) *build = vi.dwBuildNumber;
    return 1;
}

static int reg_read_sz(HKEY root, const char* subkey, const char* name, char* out, size_t cap) {
    if (!out || cap == 0) return 0;
    out[0] = 0;

    HKEY h = NULL;
    if (RegOpenKeyExA(root, subkey, 0, KEY_READ, &h) != ERROR_SUCCESS) return 0;

    DWORD type = 0;
    DWORD size = (DWORD)cap;
    LONG rc = RegQueryValueExA(h, name, NULL, &type, (LPBYTE)out, &size);
    RegCloseKey(h);

    if (rc != ERROR_SUCCESS) return 0;
    if (type != REG_SZ && type != REG_EXPAND_SZ) return 0;

    out[cap - 1] = 0;
    return out[0] != 0;
}

static void replace_windows10_with_11(char* s) {
    if (!s) return;
    const char* needle = "Windows 10";
    char* pos = strstr(s, needle);
    if (!pos) return;

    char tmp[256];
    size_t before = (size_t)(pos - s);
    snprintf(tmp, sizeof(tmp), "%.*sWindows 11%s", (int)before, s, pos + (int)strlen(needle));
    strncpy(s, tmp, 255);
    s[255] = 0;
}

int wf_get_os(char* out, size_t cap) {
    char product[256];
    if (!reg_read_sz(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
        "ProductName",
        product, sizeof(product)))
    {
        snprintf(out, cap, "Windows");
        return 1;
    }

    DWORD build = 0;
    rtl_version(NULL, NULL, &build);
    if (build >= 22000) replace_windows10_with_11(product);

    const char* arch = "x86_64";
#if defined(_M_ARM64) || defined(__aarch64__)
    arch = "arm64";
#elif defined(_M_IX86) || defined(__i386__)
    arch = "x86";
#endif

    snprintf(out, cap, "%s %s", product, arch);
    return 1;
}

int wf_get_kernel(char* out, size_t cap) {
    DWORD maj=0, min=0, build=0;
    if (!rtl_version(&maj, &min, &build)) {
        snprintf(out, cap, "Unknown");
        return 0;
    }
    snprintf(out, cap, "%lu.%lu.%lu", (unsigned long)maj, (unsigned long)min, (unsigned long)build);
    return 1;
}
