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

size_t strnpcpy_full(char **dest, size_t size, const char *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        assert(dest);
        assert(src);

        если (size == 0) {
                если (ret_truncated)
                        *ret_truncated = len > 0;
                вернуть 0;
        }

        если (len >= size) {
                если (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } иначе если (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        если (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        вернуть size;
}

size_t strpcpy_full(char **dest, size_t size, const char *src, bool *ret_truncated) {
        assert(dest);
        assert(src);

        вернуть strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(char **dest, size_t size, bool *ret_truncated, const char *src, ...) {
        bool truncated = false;
        va_list va;
        целое i;

        assert(dest);
        assert(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        assert(i >= 0);

        если (i < (целое) size) {
                *dest += i;
                size -= i;
        } иначе {
                size = 0;
                truncated = i > 0;
        }

        если (ret_truncated)
                *ret_truncated = truncated;

        вернуть size;
}

size_t strpcpyl_full(char **dest, size_t size, bool *ret_truncated, const char *src, ...) {
        bool truncated = false;
        va_list va;

        assert(dest);
        assert(src);

        va_start(va, src);
        do {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, char *);
        } пока (src);
        va_end(va);

        если (ret_truncated)
                *ret_truncated = truncated;
        вернуть size;
}

size_t strnscpy_full(char *dest, size_t size, const char *src, size_t len, bool *ret_truncated) {
        char *s;

        assert(dest);
        assert(src);

        s = dest;
        вернуть strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(char *dest, size_t size, const char *src, bool *ret_truncated) {
        assert(dest);
        assert(src);

        вернуть strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(char *dest, size_t size, bool *ret_truncated, const char *src, ...) {
        bool truncated = false;
        va_list va;
        char *s;

        assert(dest);
        assert(src);

        va_start(va, src);
        s = dest;
        do {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, char *);
        } пока (src);
        va_end(va);

        если (ret_truncated)
                *ret_truncated = truncated;

        вернуть size;
}
