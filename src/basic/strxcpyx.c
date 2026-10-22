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

#شامل <stdio.h>
#شامل <string.h>

#شامل "strxcpyx.h"

size_t strnpcpy_بھرا(حرف **dest, size_t سائز, ثابت حرف *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        أكد(dest);
        أكد(src);

        اگر (سائز == 0) {
                اگر (ret_truncated)
                        *ret_truncated = len > 0;
                واپس 0;
        }

        اگر (len >= سائز) {
                اگر (سائز > 1)
                        *dest = mempcpy(*dest, src, سائز-1);
                سائز = 0;
                truncated = true;
        } ورنہ اگر (len > 0) {
                *dest = mempcpy(*dest, src, len);
                سائز -= len;
        }

        اگر (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        واپس سائز;
}

size_t strpcpy_بھرا(حرف **dest, size_t سائز, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        واپس strnpcpy_بھرا(dest, سائز, src, لڑی_لمبائی(src), ret_truncated);
}

size_t strpcpyf_بھرا(حرف **dest, size_t سائز, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        صحیح i;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        i = vsnprintf(*dest, سائز, src, va);
        va_end(va);

        أكد(i >= 0);

        اگر (i < (صحیح) سائز) {
                *dest += i;
                سائز -= i;
        } ورنہ {
                سائز = 0;
                truncated = i > 0;
        }

        اگر (ret_truncated)
                *ret_truncated = truncated;

        واپس سائز;
}

size_t strpcpyl_بھرا(حرف **dest, size_t سائز, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        کرو {
                bool t;

                سائز = strpcpy_بھرا(dest, سائز, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } جب (src);
        va_end(va);

        اگر (ret_truncated)
                *ret_truncated = truncated;
        واپس سائز;
}

size_t strnscpy_بھرا(حرف *dest, size_t سائز, ثابت حرف *src, size_t len, bool *ret_truncated) {
        حرف *s;

        أكد(dest);
        أكد(src);

        s = dest;
        واپس strnpcpy_بھرا(&s, سائز, src, len, ret_truncated);
}

size_t strscpy_بھرا(حرف *dest, size_t سائز, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        واپس strnscpy_بھرا(dest, سائز, src, لڑی_لمبائی(src), ret_truncated);
}

size_t strscpyl_بھرا(حرف *dest, size_t سائز, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        حرف *s;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        s = dest;
        کرو {
                bool t;

                سائز = strpcpy_بھرا(&s, سائز, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } جب (src);
        va_end(va);

        اگر (ret_truncated)
                *ret_truncated = truncated;

        واپس سائز;
}
