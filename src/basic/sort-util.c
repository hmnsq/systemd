/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>

#समावेश "alloc-util.h"
#समावेश "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
व्योम *xbsearch_r(स्थिर व्योम *कुंजी, स्थिर व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार,
                 comparison_userdata_fn_t compar, व्योम *arg) {
        माप_प्रकार l, u, idx;
        स्थिर व्योम *p;
        पूर्णांक comparison;

        निश्चित(!आकार_multiply_overflow(nmemb, आकार));

        l = 0;
        u = nmemb;
        जबतक (l < u) {
                idx = (l + u) / 2;
                p = (स्थिर uint8_t*) base + idx * आकार;
                comparison = compar(कुंजी, p, arg);
                अगर (comparison < 0)
                        u = idx;
                अन्यथा अगर (comparison > 0)
                        l = idx + 1;
                अन्यथा
                        वापस (व्योम *)p;
        }
        वापस शून्य;
}

व्योम* bsearch_safe_internal(स्थिर व्योम *कुंजी, स्थिर व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        अगर (nmemb <= 0)
                वापस शून्य;

        निश्चित(base);
        वापस (व्योम*) द्वा_खोज(कुंजी, base, nmemb, आकार, compar);
}

व्योम qsort_safe(व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        अगर (nmemb <= 1)
                वापस;

        निश्चित(base);
        क्विक(base, nmemb, आकार, compar);
}

व्योम qsort_r_safe(व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार, comparison_userdata_fn_t compar, व्योम *userdata) {
        अगर (nmemb <= 1)
                वापस;

        निश्चित(base);
        qsort_r(base, nmemb, आकार, compar, userdata);
}

पूर्णांक cmp_int(स्थिर पूर्णांक *a, स्थिर पूर्णांक *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        वापस CMP(*a, *b);
}

पूर्णांक cmp_uint16(स्थिर uint16_t *a, स्थिर uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        वापस CMP(*a, *b);
}

पूर्णांक cmp_unsigned(स्थिर अचिन्हित *a, स्थिर अचिन्हित *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        वापस CMP(*a, *b);
}
