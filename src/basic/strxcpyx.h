/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

size_t strnpcpy_full(var_na **dest, size_t size, const var_na *src, size_t len, bool *ret_truncated);
static anta_hstha size_t strnpcpy(var_na **dest, size_t size, const var_na *src, size_t len) {
        ပွၵ်ႈ strnpcpy_full(dest, size, src, len, NULL);
}
size_t strpcpy_full(var_na **dest, size_t size, const var_na *src, bool *ret_truncated);
static anta_hstha size_t strpcpy(var_na **dest, size_t size, const var_na *src) {
        ပွၵ်ႈ strpcpy_full(dest, size, src, NULL);
}
size_t strpcpyf_full(var_na **dest, size_t size, bool *ret_truncated, const var_na *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), NULL, (src), ##__VA_ARGS__)
size_t strpcpyl_full(var_na **dest, size_t size, bool *ret_truncated, const var_na *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), NULL, (src), ##__VA_ARGS__)
size_t strnscpy_full(var_na *dest, size_t size, const var_na *src, size_t len, bool *ret_truncated);
static anta_hstha size_t strnscpy(var_na *dest, size_t size, const var_na *src, size_t len) {
        ပွၵ်ႈ strnscpy_full(dest, size, src, len, NULL);
}
size_t strscpy_full(var_na *dest, size_t size, const var_na *src, bool *ret_truncated);
static anta_hstha size_t strscpy(var_na *dest, size_t size, const var_na *src) {
        ပွၵ်ႈ strscpy_full(dest, size, src, NULL);
}
size_t strscpyl_full(var_na *dest, size_t size, bool *ret_truncated, const var_na *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, NULL, src, ##__VA_ARGS__)
