#include "modules/cpu.h"
#include <windows.h>
#include <stdio.h>

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

int wf_get_cpu(char* out, size_t cap) {
    if (reg_read_sz(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        "ProcessorNameString",
        out, cap))
    {
        return 1;
    }

    snprintf(out, cap, "Unknown");
    return 0;
}
