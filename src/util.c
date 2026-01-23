#include "util.h"
#include <string.h>
#include <ctype.h>

void wf_trim_inplace(char* s) {
    if (!s) return;

    size_t n = strlen(s);
    while (n > 0 && (unsigned char)s[n - 1] <= 32) {
        s[n - 1] = 0;
        n--;
    }

    size_t i = 0;
    while (s[i] && (unsigned char)s[i] <= 32) i++;

    if (i > 0) memmove(s, s + i, strlen(s + i) + 1);
}
