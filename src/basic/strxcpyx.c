/* SPDX-License-Identifier: LGPL-2.1-or-later */

/*
 * Concatenates/copies strings. In any case, terminates in all cases
 * with '\0' and moves the @dest pointer forward to the added '\0'.
 * Returns the remaining size, and 0 if the string was truncated.
 *
 * Due to the intended usage, these helpers silently noop invocations
 * having zero size.  This is technically an exception to the above
 * statement "terminates in all cases".  It's unexpected for such calls to
 * occur outside of a loop where this is the preferred behavior.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "strxcpyx.h"

size_t strnpcpy_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        ਪ੍ਰਤਿਜ੍ਞਾ(dest);
        ਪ੍ਰਤਿਜ੍ਞਾ(src);

        ਜੇ (ਮਾਪ == 0) {
                ਜੇ (ret_truncated)
                        *ret_truncated = len > 0;
                ਵਾਪਸ 0;
        }

        ਜੇ (len >= ਮਾਪ) {
                ਜੇ (ਮਾਪ > 1)
                        *dest = mempcpy(*dest, src, ਮਾਪ-1);
                ਮਾਪ = 0;
                truncated = true;
        } ਨਹੀਂਤਾਂ ਜੇ (len > 0) {
                *dest = mempcpy(*dest, src, len);
                ਮਾਪ -= len;
        }

        ਜੇ (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        ਵਾਪਸ ਮਾਪ;
}

size_t strpcpy_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, bool *ret_truncated) {
        ਪ੍ਰਤਿਜ੍ਞਾ(dest);
        ਪ੍ਰਤਿਜ੍ਞਾ(src);

        ਵਾਪਸ strnpcpy_ਭਰਿਆ(dest, ਮਾਪ, src, ਲੜੀ_ਲੰਬਾਈ(src), ret_truncated);
}

size_t strpcpyf_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, bool *ret_truncated, ਅਚਲ ਅੱਖਰ *src, ...) {
        bool truncated = false;
        va_list va;
        ਪੂਰਨ i;

        ਪ੍ਰਤਿਜ੍ਞਾ(dest);
        ਪ੍ਰਤਿਜ੍ਞਾ(src);

        va_start(va, src);
        i = vsnprintf(*dest, ਮਾਪ, src, va);
        va_end(va);

        ਪ੍ਰਤਿਜ੍ਞਾ(i >= 0);

        ਜੇ (i < (ਪੂਰਨ) ਮਾਪ) {
                *dest += i;
                ਮਾਪ -= i;
        } ਨਹੀਂਤਾਂ {
                ਮਾਪ = 0;
                truncated = i > 0;
        }

        ਜੇ (ret_truncated)
                *ret_truncated = truncated;

        ਵਾਪਸ ਮਾਪ;
}

size_t strpcpyl_ਭਰਿਆ(ਅੱਖਰ **dest, size_t ਮਾਪ, bool *ret_truncated, ਅਚਲ ਅੱਖਰ *src, ...) {
        bool truncated = false;
        va_list va;

        ਪ੍ਰਤਿਜ੍ਞਾ(dest);
        ਪ੍ਰਤਿਜ੍ਞਾ(src);

        va_start(va, src);
        ਕਰੋ {
                bool t;

                ਮਾਪ = strpcpy_ਭਰਿਆ(dest, ਮਾਪ, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ਅੱਖਰ *);
        } ਜਦੋਂ (src);
        va_end(va);

        ਜੇ (ret_truncated)
                *ret_truncated = truncated;
        ਵਾਪਸ ਮਾਪ;
}

size_t strnscpy_ਭਰਿਆ(ਅੱਖਰ *dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, size_t len, bool *ret_truncated) {
        ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(dest);
        ਪ੍ਰਤਿਜ੍ਞਾ(src);

        s = dest;
        ਵਾਪਸ strnpcpy_ਭਰਿਆ(&s, ਮਾਪ, src, len, ret_truncated);
}

size_t strscpy_ਭਰਿਆ(ਅੱਖਰ *dest, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *src, bool *ret_truncated) {
        ਪ੍ਰਤਿਜ੍ਞਾ(dest);
        ਪ੍ਰਤਿਜ੍ਞਾ(src);

        ਵਾਪਸ strnscpy_ਭਰਿਆ(dest, ਮਾਪ, src, ਲੜੀ_ਲੰਬਾਈ(src), ret_truncated);
}

size_t strscpyl_ਭਰਿਆ(ਅੱਖਰ *dest, size_t ਮਾਪ, bool *ret_truncated, ਅਚਲ ਅੱਖਰ *src, ...) {
        bool truncated = false;
        va_list va;
        ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(dest);
        ਪ੍ਰਤਿਜ੍ਞਾ(src);

        va_start(va, src);
        s = dest;
        ਕਰੋ {
                bool t;

                ਮਾਪ = strpcpy_ਭਰਿਆ(&s, ਮਾਪ, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ਅੱਖਰ *);
        } ਜਦੋਂ (src);
        va_end(va);

        ਜੇ (ret_truncated)
                *ret_truncated = truncated;

        ਵਾਪਸ ਮਾਪ;
}
