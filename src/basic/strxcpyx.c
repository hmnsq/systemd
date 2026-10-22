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

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "strxcpyx.h"

size_t strnpcpy_full(वर्ण **dest, size_t size, ध्रुव वर्ण *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        सचे (size == 0) {
                सचे (ret_truncated)
                        *ret_truncated = len > 0;
                पटिनिवत्त 0;
        }

        सचे (len >= size) {
                सचे (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } अञ्ञथा सचे (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        सचे (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        पटिनिवत्त size;
}

size_t strpcpy_full(वर्ण **dest, size_t size, ध्रुव वर्ण *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        पटिनिवत्त strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(वर्ण **dest, size_t size, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        va_list va;
        पूर्णाङ्क i;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        प्रतिज्ञा(i >= 0);

        सचे (i < (पूर्णाङ्क) size) {
                *dest += i;
                size -= i;
        } अञ्ञथा {
                size = 0;
                truncated = i > 0;
        }

        सचे (ret_truncated)
                *ret_truncated = truncated;

        पटिनिवत्त size;
}

size_t strpcpyl_full(वर्ण **dest, size_t size, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        va_list va;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        va_start(va, src);
        करोहि {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, वर्ण *);
        } याव (src);
        va_end(va);

        सचे (ret_truncated)
                *ret_truncated = truncated;
        पटिनिवत्त size;
}

size_t strnscpy_full(वर्ण *dest, size_t size, ध्रुव वर्ण *src, size_t len, bool *ret_truncated) {
        वर्ण *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        s = dest;
        पटिनिवत्त strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(वर्ण *dest, size_t size, ध्रुव वर्ण *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        पटिनिवत्त strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(वर्ण *dest, size_t size, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        va_list va;
        वर्ण *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        va_start(va, src);
        s = dest;
        करोहि {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, वर्ण *);
        } याव (src);
        va_end(va);

        सचे (ret_truncated)
                *ret_truncated = truncated;

        पटिनिवत्त size;
}
