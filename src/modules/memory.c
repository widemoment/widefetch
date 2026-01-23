#include "modules/memory.h"
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

static void bytes_to_mib(uint64_t b, char* out, size_t cap) {
    double mib = (double)b / (1024.0 * 1024.0);
    snprintf(out, cap, "%.0fMiB", mib);
}

int wf_get_memory(char* out, size_t cap) {
    MEMORYSTATUSEX ms;
    ZeroMemory(&ms, sizeof(ms));
    ms.dwLength = sizeof(ms);

    if (!GlobalMemoryStatusEx(&ms)) {
        snprintf(out, cap, "Unknown");
        return 0;
    }

    uint64_t total = (uint64_t)ms.ullTotalPhys;
    uint64_t used  = total - (uint64_t)ms.ullAvailPhys;

    char usedS[64], totalS[64];
    bytes_to_mib(used, usedS, sizeof(usedS));
    bytes_to_mib(total, totalS, sizeof(totalS));

    snprintf(out, cap, "%s / %s", usedS, totalS);
    return 1;
}
