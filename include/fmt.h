#pragma once
#include <stddef.h>

void wf_enable_ansi(void);
const char* wf_c_cyan(void);
const char* wf_c_dim(void);
const char* wf_c_reset(void);

void wf_print_side_by_side(const char* const* left, size_t left_n,
                           const char* const* right, size_t right_n,
                           int left_width);