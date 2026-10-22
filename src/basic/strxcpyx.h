/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

size_t strnpcpy_بھرا(حرف **dest, size_t سائز, ثابت حرف *src, size_t len, bool *ret_truncated);
ساکن اندرونی size_t strnpcpy(حرف **dest, size_t سائز, ثابت حرف *src, size_t len) {
        واپس strnpcpy_بھرا(dest, سائز, src, len, تہی);
}
size_t strpcpy_بھرا(حرف **dest, size_t سائز, ثابت حرف *src, bool *ret_truncated);
ساکن اندرونی size_t strpcpy(حرف **dest, size_t سائز, ثابت حرف *src) {
        واپس strpcpy_بھرا(dest, سائز, src, تہی);
}
size_t strpcpyf_بھرا(حرف **dest, size_t سائز, bool *ret_truncated, ثابت حرف *src, ...) _printf_(4, 5);
#تعریف_کرو strpcpyf(dest, سائز, src, ...) \
        strpcpyf_بھرا((dest), (سائز), تہی, (src), ##__VA_ARGS__)
size_t strpcpyl_بھرا(حرف **dest, size_t سائز, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کرو strpcpyl(dest, سائز, src, ...) \
        strpcpyl_بھرا((dest), (سائز), تہی, (src), ##__VA_ARGS__)
size_t strnscpy_بھرا(حرف *dest, size_t سائز, ثابت حرف *src, size_t len, bool *ret_truncated);
ساکن اندرونی size_t strnscpy(حرف *dest, size_t سائز, ثابت حرف *src, size_t len) {
        واپس strnscpy_بھرا(dest, سائز, src, len, تہی);
}
size_t strscpy_بھرا(حرف *dest, size_t سائز, ثابت حرف *src, bool *ret_truncated);
ساکن اندرونی size_t strscpy(حرف *dest, size_t سائز, ثابت حرف *src) {
        واپس strscpy_بھرا(dest, سائز, src, تہی);
}
size_t strscpyl_بھرا(حرف *dest, size_t سائز, bool *ret_truncated, ثابت حرف *src, ...) _sentinel_;
#تعریف_کرو strscpyl(dest, سائز, src, ...) \
        strscpyl_بھرا(dest, سائز, تہی, src, ##__VA_ARGS__)
