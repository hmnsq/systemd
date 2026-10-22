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

size_t strnpcpy_full(অক্ষর **dest, size_t আকার_2, ধ্রুবক অক্ষর *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        যদি (আকার_2 == 0) {
                যদি (ret_truncated)
                        *ret_truncated = len > 0;
                ফেরত 0;
        }

        যদি (len >= আকার_2) {
                যদি (আকার_2 > 1)
                        *dest = mempcpy(*dest, src, আকার_2-1);
                আকার_2 = 0;
                truncated = true;
        } অন্যথা যদি (len > 0) {
                *dest = mempcpy(*dest, src, len);
                আকার_2 -= len;
        }

        যদি (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        ফেরত আকার_2;
}

size_t strpcpy_full(অক্ষর **dest, size_t আকার_2, ধ্রুবক অক্ষর *src, bool *ret_truncated) {
        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        ফেরত strnpcpy_full(dest, আকার_2, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(অক্ষর **dest, size_t আকার_2, bool *ret_truncated, ধ্রুবক অক্ষর *src, ...) {
        bool truncated = false;
        va_list va;
        পূর্ণসংখ্যা i;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        i = vsnprintf(*dest, আকার_2, src, va);
        va_end(va);

        প্রতিজ্ঞা(i >= 0);

        যদি (i < (পূর্ণসংখ্যা) আকার_2) {
                *dest += i;
                আকার_2 -= i;
        } অন্যথা {
                আকার_2 = 0;
                truncated = i > 0;
        }

        যদি (ret_truncated)
                *ret_truncated = truncated;

        ফেরত আকার_2;
}

size_t strpcpyl_full(অক্ষর **dest, size_t আকার_2, bool *ret_truncated, ধ্রুবক অক্ষর *src, ...) {
        bool truncated = false;
        va_list va;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        করো {
                bool t;

                আকার_2 = strpcpy_full(dest, আকার_2, src, &t);
                truncated = truncated || t;
                src = va_arg(va, অক্ষর *);
        } যখন (src);
        va_end(va);

        যদি (ret_truncated)
                *ret_truncated = truncated;
        ফেরত আকার_2;
}

size_t strnscpy_full(অক্ষর *dest, size_t আকার_2, ধ্রুবক অক্ষর *src, size_t len, bool *ret_truncated) {
        অক্ষর *s;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        s = dest;
        ফেরত strnpcpy_full(&s, আকার_2, src, len, ret_truncated);
}

size_t strscpy_full(অক্ষর *dest, size_t আকার_2, ধ্রুবক অক্ষর *src, bool *ret_truncated) {
        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        ফেরত strnscpy_full(dest, আকার_2, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(অক্ষর *dest, size_t আকার_2, bool *ret_truncated, ধ্রুবক অক্ষর *src, ...) {
        bool truncated = false;
        va_list va;
        অক্ষর *s;

        প্রতিজ্ঞা(dest);
        প্রতিজ্ঞা(src);

        va_start(va, src);
        s = dest;
        করো {
                bool t;

                আকার_2 = strpcpy_full(&s, আকার_2, src, &t);
                truncated = truncated || t;
                src = va_arg(va, অক্ষর *);
        } যখন (src);
        va_end(va);

        যদি (ret_truncated)
                *ret_truncated = truncated;

        ফেরত আকার_2;
}
