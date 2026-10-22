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

        𐡄𐡍 (size == 0) {
                𐡄𐡍 (ret_truncated)
                        *ret_truncated = len > 0;
                أرجع 0;
        }

        𐡄𐡍 (len >= size) {
                𐡄𐡍 (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } وإلا 𐡄𐡍 (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        𐡄𐡍 (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        أرجع size;
}

size_t strpcpy_full(حرف **dest, size_t size, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        أرجع strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
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

        𐡄𐡍 (i < (صحيح) size) {
                *dest += i;
                size -= i;
        } وإلا {
                size = 0;
                truncated = i > 0;
        }

        𐡄𐡍 (ret_truncated)
                *ret_truncated = truncated;

        أرجع size;
}

size_t strpcpyl_full(حرف **dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        𐡏𐡁𐡃 {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } طالما (src);
        va_end(va);

        𐡄𐡍 (ret_truncated)
                *ret_truncated = truncated;
        أرجع size;
}

size_t strnscpy_full(حرف *dest, size_t size, ثابت حرف *src, size_t len, bool *ret_truncated) {
        حرف *s;

        أكد(dest);
        أكد(src);

        s = dest;
        أرجع strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(حرف *dest, size_t size, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        أرجع strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(حرف *dest, size_t size, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        حرف *s;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        s = dest;
        𐡏𐡁𐡃 {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } طالما (src);
        va_end(va);

        𐡄𐡍 (ret_truncated)
                *ret_truncated = truncated;

        أرجع size;
}
