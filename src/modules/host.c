#include "modules/host.h"
#include <windows.h>
#include <stdio.h>

int wf_get_host(char* out, size_t cap) {
    DWORD n = (DWORD)cap;
    if (!GetComputerNameA(out, &n)) {
        snprintf(out, cap, "Unknown");
        return 0;
    }
    return 1;
}

int wf_get_user(char* out, size_t cap) {
    DWORD n = (DWORD)cap;
    if (!GetUserNameA(out, &n)) {
        snprintf(out, cap, "Unknown");
        return 0;
    }
    return 1;
}
