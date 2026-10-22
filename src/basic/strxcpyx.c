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

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "strxcpyx.h"

size_t strnpcpy_بھریا(حرف **dest, size_t ناپ, ثابت حرف *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        أكد(dest);
        أكد(src);

        إذا (ناپ == 0) {
                إذا (ret_truncated)
                        *ret_truncated = len > 0;
                أرجع 0;
        }

        إذا (len >= ناپ) {
                إذا (ناپ > 1)
                        *dest = mempcpy(*dest, src, ناپ-1);
                ناپ = 0;
                truncated = true;
        } وإلا إذا (len > 0) {
                *dest = mempcpy(*dest, src, len);
                ناپ -= len;
        }

        إذا (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        أرجع ناپ;
}

size_t strpcpy_بھریا(حرف **dest, size_t ناپ, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        أرجع strnpcpy_بھریا(dest, ناپ, src, لڑی_لمبائی(src), ret_truncated);
}

size_t strpcpyf_بھریا(حرف **dest, size_t ناپ, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        صحيح i;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        i = vsnprintf(*dest, ناپ, src, va);
        va_end(va);

        أكد(i >= 0);

        إذا (i < (صحيح) ناپ) {
                *dest += i;
                ناپ -= i;
        } وإلا {
                ناپ = 0;
                truncated = i > 0;
        }

        إذا (ret_truncated)
                *ret_truncated = truncated;

        أرجع ناپ;
}

size_t strpcpyl_بھریا(حرف **dest, size_t ناپ, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        افعل {
                bool t;

                ناپ = strpcpy_بھریا(dest, ناپ, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } طالما (src);
        va_end(va);

        إذا (ret_truncated)
                *ret_truncated = truncated;
        أرجع ناپ;
}

size_t strnscpy_بھریا(حرف *dest, size_t ناپ, ثابت حرف *src, size_t len, bool *ret_truncated) {
        حرف *s;

        أكد(dest);
        أكد(src);

        s = dest;
        أرجع strnpcpy_بھریا(&s, ناپ, src, len, ret_truncated);
}

size_t strscpy_بھریا(حرف *dest, size_t ناپ, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        أرجع strnscpy_بھریا(dest, ناپ, src, لڑی_لمبائی(src), ret_truncated);
}

size_t strscpyl_بھریا(حرف *dest, size_t ناپ, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        حرف *s;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        s = dest;
        افعل {
                bool t;

                ناپ = strpcpy_بھریا(&s, ناپ, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } طالما (src);
        va_end(va);

        إذا (ret_truncated)
                *ret_truncated = truncated;

        أرجع ناپ;
}
