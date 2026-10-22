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

size_t strnpcpy_full(نویسه **dest, size_t size, ثابت نویسه *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        أكد(dest);
        أكد(src);

        اگر (size == 0) {
                اگر (ret_truncated)
                        *ret_truncated = len > 0;
                بازگشت 0;
        }

        اگر (len >= size) {
                اگر (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } وگرنه اگر (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        اگر (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        بازگشت size;
}

size_t strpcpy_full(نویسه **dest, size_t size, ثابت نویسه *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        بازگشت strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(نویسه **dest, size_t size, bool *ret_truncated, ثابت نویسه *src, ...) {
        bool truncated = false;
        va_list va;
        صحیح i;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        أكد(i >= 0);

        اگر (i < (صحیح) size) {
                *dest += i;
                size -= i;
        } وگرنه {
                size = 0;
                truncated = i > 0;
        }

        اگر (ret_truncated)
                *ret_truncated = truncated;

        بازگشت size;
}

size_t strpcpyl_full(نویسه **dest, size_t size, bool *ret_truncated, ثابت نویسه *src, ...) {
        bool truncated = false;
        va_list va;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        انجامبده {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, نویسه *);
        } وقتی (src);
        va_end(va);

        اگر (ret_truncated)
                *ret_truncated = truncated;
        بازگشت size;
}

size_t strnscpy_full(نویسه *dest, size_t size, ثابت نویسه *src, size_t len, bool *ret_truncated) {
        نویسه *s;

        أكد(dest);
        أكد(src);

        s = dest;
        بازگشت strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(نویسه *dest, size_t size, ثابت نویسه *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        بازگشت strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(نویسه *dest, size_t size, bool *ret_truncated, ثابت نویسه *src, ...) {
        bool truncated = false;
        va_list va;
        نویسه *s;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        s = dest;
        انجامبده {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, نویسه *);
        } وقتی (src);
        va_end(va);

        اگر (ret_truncated)
                *ret_truncated = truncated;

        بازگشت size;
}
