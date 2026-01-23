
#include "modules/gpu.h"
#include <windows.h>
#include <stdio.h>

int wf_get_gpu(char* out, size_t cap) {
    DISPLAY_DEVICEA dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);

    if (EnumDisplayDevicesA(NULL, 0, &dd, 0) && dd.DeviceString[0]) {
        snprintf(out, cap, "%s", dd.DeviceString);
        return 1;
    }

    snprintf(out, cap, "Unknown");
    return 0;
}
