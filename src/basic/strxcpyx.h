/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

size_t strnpcpy_full(حرف **dest, size_t size, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnpcpy(حرف **dest, size_t size, ثابت حرف *src, size_t len) {
        ܦܢܐ strnpcpy_full(dest, size, src, len, عدم);
}
size_t strpcpy_full(حرف **dest, size_t size, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strpcpy(حرف **dest, size_t size, ثابت حرف *src) {
        ܦܢܐ strpcpy_full(dest, size, src, عدم);
}
size_t strpcpyf_full(حرف **dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), عدم, (src), ##__VA_ARGS__)
size_t strpcpyl_full(حرف **dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), عدم, (src), ##__VA_ARGS__)
size_t strnscpy_full(حرف *dest, size_t size, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnscpy(حرف *dest, size_t size, ثابت حرف *src, size_t len) {
        ܦܢܐ strnscpy_full(dest, size, src, len, عدم);
}
size_t strscpy_full(حرف *dest, size_t size, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strscpy(حرف *dest, size_t size, ثابت حرف *src) {
        ܦܢܐ strscpy_full(dest, size, src, عدم);
}
size_t strscpyl_full(حرف *dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, عدم, src, ##__VA_ARGS__)
