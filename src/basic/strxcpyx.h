/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

size_t strnpcpy_پر(حرف **dest, size_t اندازه, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnpcpy(حرف **dest, size_t اندازه, ثابت حرف *src, size_t len) {
        أرجع strnpcpy_پر(dest, اندازه, src, len, عدم);
}
size_t strpcpy_پر(حرف **dest, size_t اندازه, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strpcpy(حرف **dest, size_t اندازه, ثابت حرف *src) {
        أرجع strpcpy_پر(dest, اندازه, src, عدم);
}
size_t strpcpyf_پر(حرف **dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) _printf_(4, 5);
#تعریف_کن strpcpyf(dest, اندازه, src, ...) \
        strpcpyf_پر((dest), (اندازه), عدم, (src), ##__VA_ARGS__)
size_t strpcpyl_پر(حرف **dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کن strpcpyl(dest, اندازه, src, ...) \
        strpcpyl_پر((dest), (اندازه), عدم, (src), ##__VA_ARGS__)
size_t strnscpy_پر(حرف *dest, size_t اندازه, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnscpy(حرف *dest, size_t اندازه, ثابت حرف *src, size_t len) {
        أرجع strnscpy_پر(dest, اندازه, src, len, عدم);
}
size_t strscpy_پر(حرف *dest, size_t اندازه, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strscpy(حرف *dest, size_t اندازه, ثابت حرف *src) {
        أرجع strscpy_پر(dest, اندازه, src, عدم);
}
size_t strscpyl_پر(حرف *dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کن strscpyl(dest, اندازه, src, ...) \
        strscpyl_پر(dest, اندازه, عدم, src, ##__VA_ARGS__)
