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

size_t strnpcpy_पूर्णम्(वर्ण **dest, size_t आकार, ध्रुव वर्ण *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        यदि (आकार == 0) {
                यदि (ret_truncated)
                        *ret_truncated = len > 0;
                प्रत्यागम 0;
        }

        यदि (len >= आकार) {
                यदि (आकार > 1)
                        *dest = mempcpy(*dest, src, आकार-1);
                आकार = 0;
                truncated = true;
        } अन्यथा यदि (len > 0) {
                *dest = mempcpy(*dest, src, len);
                आकार -= len;
        }

        यदि (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        प्रत्यागम आकार;
}

size_t strpcpy_पूर्णम्(वर्ण **dest, size_t आकार, ध्रुव वर्ण *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        प्रत्यागम strnpcpy_पूर्णम्(dest, आकार, src, वाक्यदैर्घ्यम्(src), ret_truncated);
}

size_t strpcpyf_पूर्णम्(वर्ण **dest, size_t आकार, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        va_list va;
        पूर्णाङ्क i;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        va_start(va, src);
        i = vsnprintf(*dest, आकार, src, va);
        va_end(va);

        प्रतिज्ञा(i >= 0);

        यदि (i < (पूर्णाङ्क) आकार) {
                *dest += i;
                आकार -= i;
        } अन्यथा {
                आकार = 0;
                truncated = i > 0;
        }

        यदि (ret_truncated)
                *ret_truncated = truncated;

        प्रत्यागम आकार;
}

size_t strpcpyl_पूर्णम्(वर्ण **dest, size_t आकार, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        va_list va;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        va_start(va, src);
        कुरु {
                bool t;

                आकार = strpcpy_पूर्णम्(dest, आकार, src, &t);
                truncated = truncated || t;
                src = va_arg(va, वर्ण *);
        } यावत् (src);
        va_end(va);

        यदि (ret_truncated)
                *ret_truncated = truncated;
        प्रत्यागम आकार;
}

size_t strnscpy_पूर्णम्(वर्ण *dest, size_t आकार, ध्रुव वर्ण *src, size_t len, bool *ret_truncated) {
        वर्ण *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        s = dest;
        प्रत्यागम strnpcpy_पूर्णम्(&s, आकार, src, len, ret_truncated);
}

size_t strscpy_पूर्णम्(वर्ण *dest, size_t आकार, ध्रुव वर्ण *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        प्रत्यागम strnscpy_पूर्णम्(dest, आकार, src, वाक्यदैर्घ्यम्(src), ret_truncated);
}

size_t strscpyl_पूर्णम्(वर्ण *dest, size_t आकार, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        va_list va;
        वर्ण *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        va_start(va, src);
        s = dest;
        कुरु {
                bool t;

                आकार = strpcpy_पूर्णम्(&s, आकार, src, &t);
                truncated = truncated || t;
                src = va_arg(va, वर्ण *);
        } यावत् (src);
        va_end(va);

        यदि (ret_truncated)
                *ret_truncated = truncated;

        प्रत्यागम आकार;
}
