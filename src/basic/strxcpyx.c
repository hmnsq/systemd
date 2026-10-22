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

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "strxcpyx.h"

size_t strnpcpy_full(ৱর্ণ **dest, size_t size, ধ্রুৱ ৱর্ণ *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        যদি (size == 0) {
                যদি (ret_truncated)
                        *ret_truncated = len > 0;
                ঘূৰাই 0;
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
        ঘূৰাই size;
}

size_t strpcpy_full(ৱর্ণ **dest, size_t size, ধ্রুৱ ৱর্ণ *src, bool *ret_truncated) {
        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        ঘূৰাই strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(ৱর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুৱ ৱর্ণ *src, ...) {
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

        ঘূৰাই size;
}

size_t strpcpyl_full(ৱর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুৱ ৱর্ণ *src, ...) {
        bool truncated = false;
        va_list va;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        কৰা {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ৱর্ণ *);
        } যেতিয়ালৈ (src);
        va_end(va);

        যদি (ret_truncated)
                *ret_truncated = truncated;
        ঘূৰাই size;
}

size_t strnscpy_full(ৱর্ণ *dest, size_t size, ধ্রুৱ ৱর্ণ *src, size_t len, bool *ret_truncated) {
        ৱর্ণ *s;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        s = dest;
        ঘূৰাই strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(ৱর্ণ *dest, size_t size, ধ্রুৱ ৱর্ণ *src, bool *ret_truncated) {
        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        ঘূৰাই strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(ৱর্ণ *dest, size_t size, bool *ret_truncated, ধ্রুৱ ৱর্ণ *src, ...) {
        bool truncated = false;
        va_list va;
        ৱর্ণ *s;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        s = dest;
        কৰা {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ৱর্ণ *);
        } যেতিয়ালৈ (src);
        va_end(va);

        যদি (ret_truncated)
                *ret_truncated = truncated;

        ঘূৰাই size;
}
