/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

size_t strnpcpy_ڀريل(حرف **dest, size_t سائيز, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnpcpy(حرف **dest, size_t سائيز, ثابت حرف *src, size_t len) {
        أرجع strnpcpy_ڀريل(dest, سائيز, src, len, عدم);
}
size_t strpcpy_ڀريل(حرف **dest, size_t سائيز, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strpcpy(حرف **dest, size_t سائيز, ثابت حرف *src) {
        أرجع strpcpy_ڀريل(dest, سائيز, src, عدم);
}
size_t strpcpyf_ڀريل(حرف **dest, size_t سائيز, bool *ret_truncated, ثابت حرف *src, ...) _printf_(4, 5);
#وصف_ڪريو strpcpyf(dest, سائيز, src, ...) \
        strpcpyf_ڀريل((dest), (سائيز), عدم, (src), ##__VA_ARGS__)
size_t strpcpyl_ڀريل(حرف **dest, size_t سائيز, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#وصف_ڪريو strpcpyl(dest, سائيز, src, ...) \
        strpcpyl_ڀريل((dest), (سائيز), عدم, (src), ##__VA_ARGS__)
size_t strnscpy_ڀريل(حرف *dest, size_t سائيز, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnscpy(حرف *dest, size_t سائيز, ثابت حرف *src, size_t len) {
        أرجع strnscpy_ڀريل(dest, سائيز, src, len, عدم);
}
size_t strscpy_ڀريل(حرف *dest, size_t سائيز, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strscpy(حرف *dest, size_t سائيز, ثابت حرف *src) {
        أرجع strscpy_ڀريل(dest, سائيز, src, عدم);
}
size_t strscpyl_ڀريل(حرف *dest, size_t سائيز, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#وصف_ڪريو strscpyl(dest, سائيز, src, ...) \
        strscpyl_ڀريل(dest, سائيز, عدم, src, ##__VA_ARGS__)
