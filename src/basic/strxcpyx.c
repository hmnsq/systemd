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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "strxcpyx.h"

size_t strnpcpy_full(ವರ್ಣ **dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        ಯದಿ (size == 0) {
                ಯದಿ (ret_truncated)
                        *ret_truncated = len > 0;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (len >= size) {
                ಯದಿ (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } ಅನ್ಯಥಾ ಯದಿ (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        ಯದಿ (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        ಪ್ರತ್ಯಾಗಮ size;
}

size_t strpcpy_full(ವರ್ಣ **dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, bool *ret_truncated) {
        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        ಪ್ರತ್ಯಾಗಮ strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(ವರ್ಣ **dest, size_t size, bool *ret_truncated, ಧ್ರುವ ವರ್ಣ *src, ...) {
        bool truncated = false;
        va_list va;
        ಪೂರ್ಣಾಙ್ಕ i;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        ಪ್ರತಿಜ್ಞಾ(i >= 0);

        ಯದಿ (i < (ಪೂರ್ಣಾಙ್ಕ) size) {
                *dest += i;
                size -= i;
        } ಅನ್ಯಥಾ {
                size = 0;
                truncated = i > 0;
        }

        ಯದಿ (ret_truncated)
                *ret_truncated = truncated;

        ಪ್ರತ್ಯಾಗಮ size;
}

size_t strpcpyl_full(ವರ್ಣ **dest, size_t size, bool *ret_truncated, ಧ್ರುವ ವರ್ಣ *src, ...) {
        bool truncated = false;
        va_list va;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        va_start(va, src);
        ಕುರು {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ವರ್ಣ *);
        } ಯಾವತ್ (src);
        va_end(va);

        ಯದಿ (ret_truncated)
                *ret_truncated = truncated;
        ಪ್ರತ್ಯಾಗಮ size;
}

size_t strnscpy_full(ವರ್ಣ *dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, size_t len, bool *ret_truncated) {
        ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        s = dest;
        ಪ್ರತ್ಯಾಗಮ strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(ವರ್ಣ *dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, bool *ret_truncated) {
        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        ಪ್ರತ್ಯಾಗಮ strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(ವರ್ಣ *dest, size_t size, bool *ret_truncated, ಧ್ರುವ ವರ್ಣ *src, ...) {
        bool truncated = false;
        va_list va;
        ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        va_start(va, src);
        s = dest;
        ಕುರು {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ವರ್ಣ *);
        } ಯಾವತ್ (src);
        va_end(va);

        ಯದಿ (ret_truncated)
                *ret_truncated = truncated;

        ಪ್ರತ್ಯಾಗಮ size;
}
