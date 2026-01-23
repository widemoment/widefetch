#include "fmt.h"
#include "logo.h"
#include "modules/os.h"
#include "modules/host.h"
#include "modules/cpu.h"
#include "modules/gpu.h"
#include "modules/memory.h"
#include "modules/uptime.h"

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>

static void get_resolution(char* out, size_t cap) {
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    snprintf(out, cap, "%dx%d", w, h);
}

static void get_terminal(char* out, size_t cap) {
    DWORD n = GetEnvironmentVariableA("WT_SESSION", NULL, 0);
    if (n > 0) {
        snprintf(out, cap, "Windows Terminal");
        return;
    }
    snprintf(out, cap, "Console Host");
}

static int parent_process_name(char* out, size_t cap) {
    DWORD pid = GetCurrentProcessId();

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    ZeroMemory(&pe, sizeof(pe));
    pe.dwSize = sizeof(pe);

    DWORD ppid = 0;
    if (Process32First(snap, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(snap, &pe));
    }

    if (!ppid) {
        CloseHandle(snap);
        return 0;
    }

    pe.dwSize = sizeof(pe);
    if (Process32First(snap, &pe)) {
        do {
            if (pe.th32ProcessID == ppid) {
                snprintf(out, cap, "%s", pe.szExeFile);
                CloseHandle(snap);
                return 1;
            }
        } while (Process32Next(snap, &pe));
    }

    CloseHandle(snap);
    return 0;
}

static void get_shell(char* out, size_t cap) {
    if (parent_process_name(out, cap)) return;
    snprintf(out, cap, "Unknown");
}

static void make_palette(char* out, size_t cap) {
    const int colors[] = { 40,41,42,43,44,45,46,47, 100,101,102,103,104,105,106,107 };
    size_t pos = 0;

    for (int i = 0; i < 16; i++) {
        int n = snprintf(out + pos, (pos < cap ? cap - pos : 0), "\x1b[%dm  \x1b[0m", colors[i]);
        if (n < 0) break;
        pos += (size_t)n;
        if (pos >= cap) break;
    }

    if (cap) out[cap - 1] = 0;
}

int main(void) {
    wf_enable_ansi();

    char host[128], user[128];
    wf_get_host(host, sizeof(host));
    wf_get_user(user, sizeof(user));

    char os[256], kernel[64], uptime[128], cpu[512], gpu[256], mem[128];
    wf_get_os(os, sizeof(os));
    wf_get_kernel(kernel, sizeof(kernel));
    wf_get_uptime(uptime, sizeof(uptime));
    wf_get_cpu(cpu, sizeof(cpu));
    wf_get_gpu(gpu, sizeof(gpu));
    wf_get_memory(mem, sizeof(mem));

    char res[64], term[128], shell[128];
    get_resolution(res, sizeof(res));
    get_terminal(term, sizeof(term));
    get_shell(shell, sizeof(shell));

    char line_userhost[256];
    snprintf(line_userhost, sizeof(line_userhost), "%s%s@%s%s",
        wf_c_cyan(), user, host, wf_c_reset()
    );

    const char* sep = "----------------------";

    char l_os[512], l_host[512], l_kernel[256], l_uptime[256];
    char l_shell[256], l_res[256], l_term[256], l_cpu[768], l_gpu[512], l_mem[256];

    snprintf(l_os, sizeof(l_os),           "%sOS:%s %s",         wf_c_cyan(), wf_c_reset(), os);
    snprintf(l_host, sizeof(l_host),       "%sHost:%s %s",       wf_c_cyan(), wf_c_reset(), host);
    snprintf(l_kernel, sizeof(l_kernel),   "%sKernel:%s %s",     wf_c_cyan(), wf_c_reset(), kernel);
    snprintf(l_uptime, sizeof(l_uptime),   "%sUptime:%s %s",     wf_c_cyan(), wf_c_reset(), uptime);
    snprintf(l_shell, sizeof(l_shell),     "%sShell:%s %s",      wf_c_cyan(), wf_c_reset(), shell);
    snprintf(l_res, sizeof(l_res),         "%sResolution:%s %s", wf_c_cyan(), wf_c_reset(), res);
    snprintf(l_term, sizeof(l_term),       "%sTerminal:%s %s",   wf_c_cyan(), wf_c_reset(), term);
    snprintf(l_cpu, sizeof(l_cpu),         "%sCPU:%s %s",        wf_c_cyan(), wf_c_reset(), cpu);
    snprintf(l_gpu, sizeof(l_gpu),         "%sGPU:%s %s",        wf_c_cyan(), wf_c_reset(), gpu);
    snprintf(l_mem, sizeof(l_mem),         "%sMemory:%s %s",     wf_c_cyan(), wf_c_reset(), mem);

    char pal[512];
    make_palette(pal, sizeof(pal));

    const char* right[] = {
        line_userhost,
        sep,
        l_os,
        l_host,
        l_kernel,
        l_uptime,
        l_shell,
        l_res,
        l_term,
        l_cpu,
        l_gpu,
        l_mem,
        pal,
        ""
    };

    size_t logo_n = 0;
    const char* const* logo = wf_logo_lines(&logo_n);

    static char logo_buf[64][256];
    static const char* logo_lines[64];

    size_t use_n = (logo_n < 64) ? logo_n : 64;
    for (size_t i = 0; i < use_n; i++) {
        snprintf(logo_buf[i], sizeof(logo_buf[i]), "%s%s%s",
            wf_c_cyan(), logo[i], wf_c_reset()
        );
        logo_lines[i] = logo_buf[i];
    }

    wf_print_side_by_side(logo_lines, use_n, right, sizeof(right) / sizeof(right[0]), 44);
    return 0;
}
