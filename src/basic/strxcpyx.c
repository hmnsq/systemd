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

#include <stdio.h>
#include <string.h>

#include "strxcpyx.h"

size_t strnpcpy_full(var_na **dest, size_t size, const var_na *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        assert(dest);
        assert(src);

        නම් (size == 0) {
                නම් (ret_truncated)
                        *ret_truncated = len > 0;
                ආපසු 0;
        }

        නම් (len >= size) {
                නම් (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } නැතහොත් නම් (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        නම් (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        ආපසු size;
}

size_t strpcpy_full(var_na **dest, size_t size, const var_na *src, bool *ret_truncated) {
        assert(dest);
        assert(src);

        ආපසු strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(var_na **dest, size_t size, bool *ret_truncated, const var_na *src, ...) {
        bool truncated = false;
        va_list va;
        පූර්ණ_සංඛ්‍යා i;

        assert(dest);
        assert(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        assert(i >= 0);

        නම් (i < (පූර්ණ_සංඛ්‍යා) size) {
                *dest += i;
                size -= i;
        } නැතහොත් {
                size = 0;
                truncated = i > 0;
        }

        නම් (ret_truncated)
                *ret_truncated = truncated;

        ආපසු size;
}

size_t strpcpyl_full(var_na **dest, size_t size, bool *ret_truncated, const var_na *src, ...) {
        bool truncated = false;
        va_list va;

        assert(dest);
        assert(src);

        va_start(va, src);
        do {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, var_na *);
        } අතරතුර (src);
        va_end(va);

        නම් (ret_truncated)
                *ret_truncated = truncated;
        ආපසු size;
}

size_t strnscpy_full(var_na *dest, size_t size, const var_na *src, size_t len, bool *ret_truncated) {
        var_na *s;

        assert(dest);
        assert(src);

        s = dest;
        ආපසු strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(var_na *dest, size_t size, const var_na *src, bool *ret_truncated) {
        assert(dest);
        assert(src);

        ආපසු strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(var_na *dest, size_t size, bool *ret_truncated, const var_na *src, ...) {
        bool truncated = false;
        va_list va;
        var_na *s;

        assert(dest);
        assert(src);

        va_start(va, src);
        s = dest;
        do {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, var_na *);
        } අතරතුර (src);
        va_end(va);

        නම් (ret_truncated)
                *ret_truncated = truncated;

        ආපසු size;
}
