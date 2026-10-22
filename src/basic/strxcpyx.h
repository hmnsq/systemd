/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

size_t strnpcpy_ډک(حرف **dest, size_t اندازه, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnpcpy(حرف **dest, size_t اندازه, ثابت حرف *src, size_t len) {
        أرجع strnpcpy_ډک(dest, اندازه, src, len, عدم);
}
size_t strpcpy_ډک(حرف **dest, size_t اندازه, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strpcpy(حرف **dest, size_t اندازه, ثابت حرف *src) {
        أرجع strpcpy_ډک(dest, اندازه, src, عدم);
}
size_t strpcpyf_ډک(حرف **dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) _printf_(4, 5);
#تعریف_کړه strpcpyf(dest, اندازه, src, ...) \
        strpcpyf_ډک((dest), (اندازه), عدم, (src), ##__VA_ARGS__)
size_t strpcpyl_ډک(حرف **dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کړه strpcpyl(dest, اندازه, src, ...) \
        strpcpyl_ډک((dest), (اندازه), عدم, (src), ##__VA_ARGS__)
size_t strnscpy_ډک(حرف *dest, size_t اندازه, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnscpy(حرف *dest, size_t اندازه, ثابت حرف *src, size_t len) {
        أرجع strnscpy_ډک(dest, اندازه, src, len, عدم);
}
size_t strscpy_ډک(حرف *dest, size_t اندازه, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strscpy(حرف *dest, size_t اندازه, ثابت حرف *src) {
        أرجع strscpy_ډک(dest, اندازه, src, عدم);
}
size_t strscpyl_ډک(حرف *dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کړه strscpyl(dest, اندازه, src, ...) \
        strscpyl_ډک(dest, اندازه, عدم, src, ##__VA_ARGS__)
