/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

size_t strnpcpy_بھریا(حرف **dest, size_t ناپ, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnpcpy(حرف **dest, size_t ناپ, ثابت حرف *src, size_t len) {
        أرجع strnpcpy_بھریا(dest, ناپ, src, len, عدم);
}
size_t strpcpy_بھریا(حرف **dest, size_t ناپ, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strpcpy(حرف **dest, size_t ناپ, ثابت حرف *src) {
        أرجع strpcpy_بھریا(dest, ناپ, src, عدم);
}
size_t strpcpyf_بھریا(حرف **dest, size_t ناپ, bool *ret_truncated, ثابت حرف *src, ...) _printf_(4, 5);
#تعریف_کرو strpcpyf(dest, ناپ, src, ...) \
        strpcpyf_بھریا((dest), (ناپ), عدم, (src), ##__VA_ARGS__)
size_t strpcpyl_بھریا(حرف **dest, size_t ناپ, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کرو strpcpyl(dest, ناپ, src, ...) \
        strpcpyl_بھریا((dest), (ناپ), عدم, (src), ##__VA_ARGS__)
size_t strnscpy_بھریا(حرف *dest, size_t ناپ, ثابت حرف *src, size_t len, bool *ret_truncated);
ساكن مضمن size_t strnscpy(حرف *dest, size_t ناپ, ثابت حرف *src, size_t len) {
        أرجع strnscpy_بھریا(dest, ناپ, src, len, عدم);
}
size_t strscpy_بھریا(حرف *dest, size_t ناپ, ثابت حرف *src, bool *ret_truncated);
ساكن مضمن size_t strscpy(حرف *dest, size_t ناپ, ثابت حرف *src) {
        أرجع strscpy_بھریا(dest, ناپ, src, عدم);
}
size_t strscpyl_بھریا(حرف *dest, size_t ناپ, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کرو strscpyl(dest, ناپ, src, ...) \
        strscpyl_بھریا(dest, ناپ, عدم, src, ##__VA_ARGS__)
