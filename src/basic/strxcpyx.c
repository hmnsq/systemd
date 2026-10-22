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

size_t strnpcpy_full(حرف **dest, size_t size, ثابت حرف *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        أكد(dest);
        أكد(src);

        אם (size == 0) {
                אם (ret_truncated)
                        *ret_truncated = len > 0;
                החזר 0;
        }

        אם (len >= size) {
                אם (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } אחרת אם (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        אם (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        החזר size;
}

size_t strpcpy_full(حرف **dest, size_t size, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        החזר strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(حرف **dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        صحيح i;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        أكد(i >= 0);

        אם (i < (صحيح) size) {
                *dest += i;
                size -= i;
        } אחרת {
                size = 0;
                truncated = i > 0;
        }

        אם (ret_truncated)
                *ret_truncated = truncated;

        החזר size;
}

size_t strpcpyl_full(حرف **dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        עשה {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } כלעוד (src);
        va_end(va);

        אם (ret_truncated)
                *ret_truncated = truncated;
        החזר size;
}

size_t strnscpy_full(حرف *dest, size_t size, ثابت حرف *src, size_t len, bool *ret_truncated) {
        حرف *s;

        أكد(dest);
        أكد(src);

        s = dest;
        החזר strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(حرف *dest, size_t size, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        החזר strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(حرف *dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        حرف *s;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        s = dest;
        עשה {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } כלעוד (src);
        va_end(va);

        אם (ret_truncated)
                *ret_truncated = truncated;

        החזר size;
}
