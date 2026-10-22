/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

size_t strnpcpy_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, size_t len, bool *ret_truncated);
ਸਥਿਰ ਸਤਰਅੰਦਰ size_t strnpcpy(ਅੱਖਰ **dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, size_t len) {
        ਵਾਪਸ strnpcpy_ਭਰਿਆ(dest, ਮਾਪ, src, len, ਨਿਰਾ);
}
size_t strpcpy_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, bool *ret_truncated);
ਸਥਿਰ ਸਤਰਅੰਦਰ size_t strpcpy(ਅੱਖਰ **dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src) {
        ਵਾਪਸ strpcpy_ਭਰਿਆ(dest, ਮਾਪ, src, ਨਿਰਾ);
}
size_t strpcpyf_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, bool *ret_truncated, ਅਚਲ ਅੱਖਰ *src, ...) _printf_(4, 5);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strpcpyf(dest, ਮਾਪ, src, ...) \
        strpcpyf_ਭਰਿਆ((dest), (ਮਾਪ), ਨਿਰਾ, (src), ##__VA_ARGS__)
size_t strpcpyl_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, bool *ret_truncated, ਅਚਲ ਅੱਖਰ *src, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strpcpyl(dest, ਮਾਪ, src, ...) \
        strpcpyl_ਭਰਿਆ((dest), (ਮਾਪ), ਨਿਰਾ, (src), ##__VA_ARGS__)
size_t strnscpy_ਭਰਿਆ(ਅੱਖਰ *dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, size_t len, bool *ret_truncated);
ਸਥਿਰ ਸਤਰਅੰਦਰ size_t strnscpy(ਅੱਖਰ *dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, size_t len) {
        ਵਾਪਸ strnscpy_ਭਰਿਆ(dest, ਮਾਪ, src, len, ਨਿਰਾ);
}
size_t strscpy_ਭਰਿਆ(ਅੱਖਰ *dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, bool *ret_truncated);
ਸਥਿਰ ਸਤਰਅੰਦਰ size_t strscpy(ਅੱਖਰ *dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src) {
        ਵਾਪਸ strscpy_ਭਰਿਆ(dest, ਮਾਪ, src, ਨਿਰਾ);
}
size_t strscpyl_ਭਰਿਆ(ਅੱਖਰ *dest, size_t ਮਾਪ, bool *ret_truncated, ਅਚਲ ਅੱਖਰ *src, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strscpyl(dest, ਮਾਪ, src, ...) \
        strscpyl_ਭਰਿਆ(dest, ਮਾਪ, ਨਿਰਾ, src, ##__VA_ARGS__)
