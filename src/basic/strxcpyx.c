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

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "strxcpyx.h"

size_t strnpcpy_full(અક્ષર **dest, size_t size, અચલ અક્ષર *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        પ્રતિજ્ઞા(dest);
        પ્રતિજ્ઞા(src);

        જો (size == 0) {
                જો (ret_truncated)
                        *ret_truncated = len > 0;
                પાછા 0;
        }

        જો (len >= size) {
                જો (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } નહીંતર જો (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        જો (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        પાછા size;
}

size_t strpcpy_full(અક્ષર **dest, size_t size, અચલ અક્ષર *src, bool *ret_truncated) {
        પ્રતિજ્ઞા(dest);
        પ્રતિજ્ઞા(src);

        પાછા strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(અક્ષર **dest, size_t size, bool *ret_truncated, અચલ અક્ષર *src, ...) {
        bool truncated = false;
        va_list va;
        પૂર્ણાંક i;

        પ્રતિજ્ઞા(dest);
        પ્રતિજ્ઞા(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        પ્રતિજ્ઞા(i >= 0);

        જો (i < (પૂર્ણાંક) size) {
                *dest += i;
                size -= i;
        } નહીંતર {
                size = 0;
                truncated = i > 0;
        }

        જો (ret_truncated)
                *ret_truncated = truncated;

        પાછા size;
}

size_t strpcpyl_full(અક્ષર **dest, size_t size, bool *ret_truncated, અચલ અક્ષર *src, ...) {
        bool truncated = false;
        va_list va;

        પ્રતિજ્ઞા(dest);
        પ્રતિજ્ઞા(src);

        va_start(va, src);
        કરો {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, અક્ષર *);
        } જ્યારે (src);
        va_end(va);

        જો (ret_truncated)
                *ret_truncated = truncated;
        પાછા size;
}

size_t strnscpy_full(અક્ષર *dest, size_t size, અચલ અક્ષર *src, size_t len, bool *ret_truncated) {
        અક્ષર *s;

        પ્રતિજ્ઞા(dest);
        પ્રતિજ્ઞા(src);

        s = dest;
        પાછા strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(અક્ષર *dest, size_t size, અચલ અક્ષર *src, bool *ret_truncated) {
        પ્રતિજ્ઞા(dest);
        પ્રતિજ્ઞા(src);

        પાછા strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(અક્ષર *dest, size_t size, bool *ret_truncated, અચલ અક્ષર *src, ...) {
        bool truncated = false;
        va_list va;
        અક્ષર *s;

        પ્રતિજ્ઞા(dest);
        પ્રતિજ્ઞા(src);

        va_start(va, src);
        s = dest;
        કરો {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, અક્ષર *);
        } જ્યારે (src);
        va_end(va);

        જો (ret_truncated)
                *ret_truncated = truncated;

        પાછા size;
}
