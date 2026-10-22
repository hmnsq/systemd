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

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "strxcpyx.h"

माप_प्रकार strnpcpy_full(अक्षर **dest, माप_प्रकार आकार, स्थिर अक्षर *src, माप_प्रकार len, bool *ret_truncated) {
        bool truncated = false;

        निश्चित(dest);
        निश्चित(src);

        अगर (आकार == 0) {
                अगर (ret_truncated)
                        *ret_truncated = len > 0;
                वापस 0;
        }

        अगर (len >= आकार) {
                अगर (आकार > 1)
                        *dest = mempcpy(*dest, src, आकार-1);
                आकार = 0;
                truncated = true;
        } अन्यथा अगर (len > 0) {
                *dest = mempcpy(*dest, src, len);
                आकार -= len;
        }

        अगर (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        वापस आकार;
}

माप_प्रकार strpcpy_full(अक्षर **dest, माप_प्रकार आकार, स्थिर अक्षर *src, bool *ret_truncated) {
        निश्चित(dest);
        निश्चित(src);

        वापस strnpcpy_full(dest, आकार, src, म_माप(src), ret_truncated);
}

माप_प्रकार strpcpyf_full(अक्षर **dest, माप_प्रकार आकार, bool *ret_truncated, स्थिर अक्षर *src, ...) {
        bool truncated = false;
        बहु_सूची va;
        पूर्णांक i;

        निश्चित(dest);
        निश्चित(src);

        बहु_शुरू(va, src);
        i = vsnprintf(*dest, आकार, src, va);
        बहु_पूर्ण(va);

        निश्चित(i >= 0);

        अगर (i < (पूर्णांक) आकार) {
                *dest += i;
                आकार -= i;
        } अन्यथा {
                आकार = 0;
                truncated = i > 0;
        }

        अगर (ret_truncated)
                *ret_truncated = truncated;

        वापस आकार;
}

माप_प्रकार strpcpyl_full(अक्षर **dest, माप_प्रकार आकार, bool *ret_truncated, स्थिर अक्षर *src, ...) {
        bool truncated = false;
        बहु_सूची va;

        निश्चित(dest);
        निश्चित(src);

        बहु_शुरू(va, src);
        करो {
                bool t;

                आकार = strpcpy_full(dest, आकार, src, &t);
                truncated = truncated || t;
                src = बहु_तर्क(va, अक्षर *);
        } जबतक (src);
        बहु_पूर्ण(va);

        अगर (ret_truncated)
                *ret_truncated = truncated;
        वापस आकार;
}

माप_प्रकार strnscpy_full(अक्षर *dest, माप_प्रकार आकार, स्थिर अक्षर *src, माप_प्रकार len, bool *ret_truncated) {
        अक्षर *s;

        निश्चित(dest);
        निश्चित(src);

        s = dest;
        वापस strnpcpy_full(&s, आकार, src, len, ret_truncated);
}

माप_प्रकार strscpy_full(अक्षर *dest, माप_प्रकार आकार, स्थिर अक्षर *src, bool *ret_truncated) {
        निश्चित(dest);
        निश्चित(src);

        वापस strnscpy_full(dest, आकार, src, म_माप(src), ret_truncated);
}

माप_प्रकार strscpyl_full(अक्षर *dest, माप_प्रकार आकार, bool *ret_truncated, स्थिर अक्षर *src, ...) {
        bool truncated = false;
        बहु_सूची va;
        अक्षर *s;

        निश्चित(dest);
        निश्चित(src);

        बहु_शुरू(va, src);
        s = dest;
        करो {
                bool t;

                आकार = strpcpy_full(&s, आकार, src, &t);
                truncated = truncated || t;
                src = बहु_तर्क(va, अक्षर *);
        } जबतक (src);
        बहु_पूर्ण(va);

        अगर (ret_truncated)
                *ret_truncated = truncated;

        वापस आकार;
}
