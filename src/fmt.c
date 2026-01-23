#include "fmt.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

void wf_enable_ansi(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;

    DWORD mode = 0;
    if (!GetConsoleMode(h, &mode)) return;

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(h, mode);
}

const char* wf_c_cyan(void)  { return "\x1b[36m"; }
const char* wf_c_dim(void)   { return "\x1b[2m"; }
const char* wf_c_reset(void) { return "\x1b[0m"; }

static int visible_len_ansi(const char* s) {
    if (!s) return 0;
    int n = 0;

    for (const unsigned char* p = (const unsigned char*)s; *p; ) {
        if (*p == 0x1B && p[1] == '[') {
            p += 2;
            while (*p && *p != 'm') p++;
            if (*p == 'm') p++;
            continue;
        }
        n++;
        p++;
    }

    return n;
}

static void print_padded_left(const char* s, int width) {
    if (!s) s = "";
    fputs(s, stdout);

    int vlen = visible_len_ansi(s);
    for (int i = 0; i < width - vlen; i++) putchar(' ');
}

void wf_print_side_by_side(const char* const* left, size_t left_n,
                           const char* const* right, size_t right_n,
                           int left_width)
{
    size_t rows = left_n > right_n ? left_n : right_n;

    for (size_t i = 0; i < rows; i++) {
        const char* l = (i < left_n) ? left[i] : "";
        const char* r = (i < right_n) ? right[i] : "";

        print_padded_left(l, left_width);
        fputs(r ? r : "", stdout);
        putchar('\n');
    }
}