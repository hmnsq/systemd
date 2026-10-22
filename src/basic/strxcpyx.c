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

size_t strnpcpy_ډک(حرف **dest, size_t اندازه, ثابت حرف *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        أكد(dest);
        أكد(src);

        إذا (اندازه == 0) {
                إذا (ret_truncated)
                        *ret_truncated = len > 0;
                أرجع 0;
        }

        إذا (len >= اندازه) {
                إذا (اندازه > 1)
                        *dest = mempcpy(*dest, src, اندازه-1);
                اندازه = 0;
                truncated = true;
        } وإلا إذا (len > 0) {
                *dest = mempcpy(*dest, src, len);
                اندازه -= len;
        }

        إذا (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        أرجع اندازه;
}

size_t strpcpy_ډک(حرف **dest, size_t اندازه, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        أرجع strnpcpy_ډک(dest, اندازه, src, مزي_اوږدوالی(src), ret_truncated);
}

size_t strpcpyf_ډک(حرف **dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        صحيح i;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        i = vsnprintf(*dest, اندازه, src, va);
        va_end(va);

        أكد(i >= 0);

        إذا (i < (صحيح) اندازه) {
                *dest += i;
                اندازه -= i;
        } وإلا {
                اندازه = 0;
                truncated = i > 0;
        }

        إذا (ret_truncated)
                *ret_truncated = truncated;

        أرجع اندازه;
}

size_t strpcpyl_ډک(حرف **dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        افعل {
                bool t;

                اندازه = strpcpy_ډک(dest, اندازه, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } طالما (src);
        va_end(va);

        إذا (ret_truncated)
                *ret_truncated = truncated;
        أرجع اندازه;
}

size_t strnscpy_ډک(حرف *dest, size_t اندازه, ثابت حرف *src, size_t len, bool *ret_truncated) {
        حرف *s;

        أكد(dest);
        أكد(src);

        s = dest;
        أرجع strnpcpy_ډک(&s, اندازه, src, len, ret_truncated);
}

size_t strscpy_ډک(حرف *dest, size_t اندازه, ثابت حرف *src, bool *ret_truncated) {
        أكد(dest);
        أكد(src);

        أرجع strnscpy_ډک(dest, اندازه, src, مزي_اوږدوالی(src), ret_truncated);
}

size_t strscpyl_ډک(حرف *dest, size_t اندازه, bool *ret_truncated, ثابت حرف *src, ...) {
        bool truncated = false;
        va_list va;
        حرف *s;

        أكد(dest);
        أكد(src);

        va_start(va, src);
        s = dest;
        افعل {
                bool t;

                اندازه = strpcpy_ډک(&s, اندازه, src, &t);
                truncated = truncated || t;
                src = va_arg(va, حرف *);
        } طالما (src);
        va_end(va);

        إذا (ret_truncated)
                *ret_truncated = truncated;

        أرجع اندازه;
}
