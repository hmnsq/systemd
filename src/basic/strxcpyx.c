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

माप_प्रकार strnpcpy_full(अक्षर **dest, माप_प्रकार size, स्थिरांक अक्षर *src, माप_प्रकार len, bool *ret_truncated) {
        bool truncated = false;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        जर (size == 0) {
                जर (ret_truncated)
                        *ret_truncated = len > 0;
                परत 0;
        }

        जर (len >= size) {
                जर (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } अन्यथा जर (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        जर (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        परत size;
}

माप_प्रकार strpcpy_full(अक्षर **dest, माप_प्रकार size, स्थिरांक अक्षर *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        परत strnpcpy_full(dest, size, src, म_माप(src), ret_truncated);
}

माप_प्रकार strpcpyf_full(अक्षर **dest, माप_प्रकार size, bool *ret_truncated, स्थिरांक अक्षर *src, ...) {
        bool truncated = false;
        बहु_सूची va;
        पूर्णांक i;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        बहु_शुरू(va, src);
        i = vsnprintf(*dest, size, src, va);
        बहु_पूर्ण(va);

        प्रतिज्ञा(i >= 0);

        जर (i < (पूर्णांक) size) {
                *dest += i;
                size -= i;
        } अन्यथा {
                size = 0;
                truncated = i > 0;
        }

        जर (ret_truncated)
                *ret_truncated = truncated;

        परत size;
}

माप_प्रकार strpcpyl_full(अक्षर **dest, माप_प्रकार size, bool *ret_truncated, स्थिरांक अक्षर *src, ...) {
        bool truncated = false;
        बहु_सूची va;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        बहु_शुरू(va, src);
        करा {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = बहु_तर्क(va, अक्षर *);
        } पर्यंत (src);
        बहु_पूर्ण(va);

        जर (ret_truncated)
                *ret_truncated = truncated;
        परत size;
}

माप_प्रकार strnscpy_full(अक्षर *dest, माप_प्रकार size, स्थिरांक अक्षर *src, माप_प्रकार len, bool *ret_truncated) {
        अक्षर *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        s = dest;
        परत strnpcpy_full(&s, size, src, len, ret_truncated);
}

माप_प्रकार strscpy_full(अक्षर *dest, माप_प्रकार size, स्थिरांक अक्षर *src, bool *ret_truncated) {
        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        परत strnscpy_full(dest, size, src, म_माप(src), ret_truncated);
}

माप_प्रकार strscpyl_full(अक्षर *dest, माप_प्रकार size, bool *ret_truncated, स्थिरांक अक्षर *src, ...) {
        bool truncated = false;
        बहु_सूची va;
        अक्षर *s;

        प्रतिज्ञा(dest);
        प्रतिज्ञा(src);

        बहु_शुरू(va, src);
        s = dest;
        करा {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = बहु_तर्क(va, अक्षर *);
        } पर्यंत (src);
        बहु_पूर्ण(va);

        जर (ret_truncated)
                *ret_truncated = truncated;

        परत size;
}
