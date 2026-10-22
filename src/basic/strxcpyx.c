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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "strxcpyx.h"

size_t strnpcpy_full(ଅକ୍ଷର **dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        ପ୍ରତିଜ୍ଞା(dest);
        ପ୍ରତିଜ୍ଞା(src);

        ଯଦି (size == 0) {
                ଯଦି (ret_truncated)
                        *ret_truncated = len > 0;
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (len >= size) {
                ଯଦି (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } ଅନ୍ୟଥା ଯଦି (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        ଯଦି (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        ଫେରନ୍ତୁ size;
}

size_t strpcpy_full(ଅକ୍ଷର **dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, bool *ret_truncated) {
        ପ୍ରତିଜ୍ଞା(dest);
        ପ୍ରତିଜ୍ଞା(src);

        ଫେରନ୍ତୁ strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(ଅକ୍ଷର **dest, size_t size, bool *ret_truncated, ଅଚଳ ଅକ୍ଷର *src, ...) {
        bool truncated = false;
        va_list va;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

        ପ୍ରତିଜ୍ଞା(dest);
        ପ୍ରତିଜ୍ଞା(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        ପ୍ରତିଜ୍ଞା(i >= 0);

        ଯଦି (i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) size) {
                *dest += i;
                size -= i;
        } ଅନ୍ୟଥା {
                size = 0;
                truncated = i > 0;
        }

        ଯଦି (ret_truncated)
                *ret_truncated = truncated;

        ଫେରନ୍ତୁ size;
}

size_t strpcpyl_full(ଅକ୍ଷର **dest, size_t size, bool *ret_truncated, ଅଚଳ ଅକ୍ଷର *src, ...) {
        bool truncated = false;
        va_list va;

        ପ୍ରତିଜ୍ଞା(dest);
        ପ୍ରତିଜ୍ଞା(src);

        va_start(va, src);
        କର {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ଅକ୍ଷର *);
        } ଯେତେବେଳେ (src);
        va_end(va);

        ଯଦି (ret_truncated)
                *ret_truncated = truncated;
        ଫେରନ୍ତୁ size;
}

size_t strnscpy_full(ଅକ୍ଷର *dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, size_t len, bool *ret_truncated) {
        ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(dest);
        ପ୍ରତିଜ୍ଞା(src);

        s = dest;
        ଫେରନ୍ତୁ strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(ଅକ୍ଷର *dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, bool *ret_truncated) {
        ପ୍ରତିଜ୍ଞା(dest);
        ପ୍ରତିଜ୍ଞା(src);

        ଫେରନ୍ତୁ strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(ଅକ୍ଷର *dest, size_t size, bool *ret_truncated, ଅଚଳ ଅକ୍ଷର *src, ...) {
        bool truncated = false;
        va_list va;
        ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(dest);
        ପ୍ରତିଜ୍ଞା(src);

        va_start(va, src);
        s = dest;
        କର {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ଅକ୍ଷର *);
        } ଯେତେବେଳେ (src);
        va_end(va);

        ଯଦି (ret_truncated)
                *ret_truncated = truncated;

        ଫେରନ୍ତୁ size;
}
