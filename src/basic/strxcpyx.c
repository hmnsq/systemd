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

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "strxcpyx.h"

size_t strnpcpy_full(எழுத்து **dest, size_t size, மாறிலி எழுத்து *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        ப்ரதிஜ்ஞா(dest);
        ப்ரதிஜ்ஞா(src);

        எனில் (size == 0) {
                எனில் (ret_truncated)
                        *ret_truncated = len > 0;
                திரும்பு 0;
        }

        எனில் (len >= size) {
                எனில் (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } இல்லையெனில் எனில் (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        எனில் (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        திரும்பு size;
}

size_t strpcpy_full(எழுத்து **dest, size_t size, மாறிலி எழுத்து *src, bool *ret_truncated) {
        ப்ரதிஜ்ஞா(dest);
        ப்ரதிஜ்ஞா(src);

        திரும்பு strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(எழுத்து **dest, size_t size, bool *ret_truncated, மாறிலி எழுத்து *src, ...) {
        bool truncated = false;
        va_list va;
        முழு i;

        ப்ரதிஜ்ஞா(dest);
        ப்ரதிஜ்ஞா(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        ப்ரதிஜ்ஞா(i >= 0);

        எனில் (i < (முழு) size) {
                *dest += i;
                size -= i;
        } இல்லையெனில் {
                size = 0;
                truncated = i > 0;
        }

        எனில் (ret_truncated)
                *ret_truncated = truncated;

        திரும்பு size;
}

size_t strpcpyl_full(எழுத்து **dest, size_t size, bool *ret_truncated, மாறிலி எழுத்து *src, ...) {
        bool truncated = false;
        va_list va;

        ப்ரதிஜ்ஞா(dest);
        ப்ரதிஜ்ஞா(src);

        va_start(va, src);
        செய் {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, எழுத்து *);
        } போது (src);
        va_end(va);

        எனில் (ret_truncated)
                *ret_truncated = truncated;
        திரும்பு size;
}

size_t strnscpy_full(எழுத்து *dest, size_t size, மாறிலி எழுத்து *src, size_t len, bool *ret_truncated) {
        எழுத்து *s;

        ப்ரதிஜ்ஞா(dest);
        ப்ரதிஜ்ஞா(src);

        s = dest;
        திரும்பு strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(எழுத்து *dest, size_t size, மாறிலி எழுத்து *src, bool *ret_truncated) {
        ப்ரதிஜ்ஞா(dest);
        ப்ரதிஜ்ஞா(src);

        திரும்பு strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(எழுத்து *dest, size_t size, bool *ret_truncated, மாறிலி எழுத்து *src, ...) {
        bool truncated = false;
        va_list va;
        எழுத்து *s;

        ப்ரதிஜ்ஞா(dest);
        ப்ரதிஜ்ஞா(src);

        va_start(va, src);
        s = dest;
        செய் {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, எழுத்து *);
        } போது (src);
        va_end(va);

        எனில் (ret_truncated)
                *ret_truncated = truncated;

        திரும்பு size;
}
