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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "strxcpyx.h"

size_t strnpcpy_full(বর্ণ **dest, size_t size, ধ্রুব বর্ণ *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        যদি (size == 0) {
                যদি (ret_truncated)
                        *ret_truncated = len > 0;
                প্রত্যাগম 0;
        }

        যদি (len >= size) {
                যদি (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } অন্যথা যদি (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        যদি (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        প্রত্যাগম size;
}

size_t strpcpy_full(বর্ণ **dest, size_t size, ধ্রুব বর্ণ *src, bool *ret_truncated) {
        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        প্রত্যাগম strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(বর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুব বর্ণ *src, ...) {
        bool truncated = false;
        va_list va;
        পূর্ণাঙ্ক i;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        প্রতিজ্ঞা(i >= 0);

        যদি (i < (পূর্ণাঙ্ক) size) {
                *dest += i;
                size -= i;
        } অন্যথা {
                size = 0;
                truncated = i > 0;
        }

        যদি (ret_truncated)
                *ret_truncated = truncated;

        প্রত্যাগম size;
}

size_t strpcpyl_full(বর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুব বর্ণ *src, ...) {
        bool truncated = false;
        va_list va;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        কুরু {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, বর্ণ *);
        } যাবত্ (src);
        va_end(va);

        যদি (ret_truncated)
                *ret_truncated = truncated;
        প্রত্যাগম size;
}

size_t strnscpy_full(বর্ণ *dest, size_t size, ধ্রুব বর্ণ *src, size_t len, bool *ret_truncated) {
        বর্ণ *s;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        s = dest;
        প্রত্যাগম strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(বর্ণ *dest, size_t size, ধ্রুব বর্ণ *src, bool *ret_truncated) {
        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        প্রত্যাগম strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(বর্ণ *dest, size_t size, bool *ret_truncated, ধ্রুব বর্ণ *src, ...) {
        bool truncated = false;
        va_list va;
        বর্ণ *s;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        s = dest;
        কুরু {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, বর্ণ *);
        } যাবত্ (src);
        va_end(va);

        যদি (ret_truncated)
                *ret_truncated = truncated;

        প্রত্যাগম size;
}
