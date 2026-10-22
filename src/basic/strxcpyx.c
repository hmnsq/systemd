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

माप_प्रकार strnpcpy_full(वर्ण **dest, माप_प्रकार size, ध्रुव वर्ण *src, माप_प्रकार len, bool *ret_truncated) {
        bool truncated = false;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        यदि (size == 0) {
                यदि (ret_truncated)
                        *ret_truncated = len > 0;
                प्रत्यागम 0;
        }

        यदि (len >= size) {
                यदि (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } अन्यथा यदि (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        यदि (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        प्रत्यागम size;
}

माप_प्रकार strpcpy_full(वर्ण **dest, माप_प्रकार size, ध्रुव वर्ण *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        प्रत्यागम strnpcpy_full(dest, size, src, म_माप(src), ret_truncated);
}

माप_प्रकार strpcpyf_full(वर्ण **dest, माप_प्रकार size, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        बहु_सूची va;
        पूर्णाङ्क i;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        बहु_शुरू(va, src);
        i = vsnprintf(*dest, size, src, va);
        बहु_पूर्ण(va);

        प्रतिज्ञा(i >= 0);

        यदि (i < (पूर्णाङ्क) size) {
                *dest += i;
                size -= i;
        } अन्यथा {
                size = 0;
                truncated = i > 0;
        }

        यदि (ret_truncated)
                *ret_truncated = truncated;

        प्रत्यागम size;
}

माप_प्रकार strpcpyl_full(वर्ण **dest, माप_प्रकार size, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        बहु_सूची va;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        बहु_शुरू(va, src);
        कुरु {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = बहु_तर्क(va, वर्ण *);
        } यावत् (src);
        बहु_पूर्ण(va);

        यदि (ret_truncated)
                *ret_truncated = truncated;
        प्रत्यागम size;
}

माप_प्रकार strnscpy_full(वर्ण *dest, माप_प्रकार size, ध्रुव वर्ण *src, माप_प्रकार len, bool *ret_truncated) {
        वर्ण *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        s = dest;
        प्रत्यागम strnpcpy_full(&s, size, src, len, ret_truncated);
}

माप_प्रकार strscpy_full(वर्ण *dest, माप_प्रकार size, ध्रुव वर्ण *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        प्रत्यागम strnscpy_full(dest, size, src, म_माप(src), ret_truncated);
}

माप_प्रकार strscpyl_full(वर्ण *dest, माप_प्रकार size, bool *ret_truncated, ध्रुव वर्ण *src, ...) {
        bool truncated = false;
        बहु_सूची va;
        वर्ण *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        बहु_शुरू(va, src);
        s = dest;
        कुरु {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = बहु_तर्क(va, वर्ण *);
        } यावत् (src);
        बहु_पूर्ण(va);

        यदि (ret_truncated)
                *ret_truncated = truncated;

        प्रत्यागम size;
}
