/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
शून्य *xbsearch_r(स्थिरांक शून्य *किल्ली, स्थिरांक शून्य *base, माप_प्रकार nmemb, माप_प्रकार size,
                 comparison_userdata_fn_t compar, शून्य *arg) {
        माप_प्रकार l, u, idx;
        स्थिरांक शून्य *p;
        पूर्णांक comparison;

        प्रतिज्ञा(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        पर्यंत (l < u) {
                idx = (l + u) / 2;
                p = (स्थिरांक uint8_t*) base + idx * size;
                comparison = compar(किल्ली, p, arg);
                जर (comparison < 0)
                        u = idx;
                अन्यथा जर (comparison > 0)
                        l = idx + 1;
                अन्यथा
                        परत (शून्य *)p;
        }
        परत रिक्त;
}

शून्य* bsearch_safe_internal(स्थिरांक शून्य *किल्ली, स्थिरांक शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        जर (nmemb <= 0)
                परत रिक्त;

        प्रतिज्ञा(base);
        परत (शून्य*) द्वा_खोज(किल्ली, base, nmemb, size, compar);
}

शून्य qsort_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        जर (nmemb <= 1)
                परत;

        प्रतिज्ञा(base);
        क्विक(base, nmemb, size, compar);
}

शून्य qsort_r_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_userdata_fn_t compar, शून्य *userdata) {
        जर (nmemb <= 1)
                परत;

        प्रतिज्ञा(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

पूर्णांक cmp_int(स्थिरांक पूर्णांक *a, स्थिरांक पूर्णांक *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        परत CMP(*a, *b);
}

पूर्णांक cmp_uint16(स्थिरांक uint16_t *a, स्थिरांक uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        परत CMP(*a, *b);
}

पूर्णांक cmp_unsigned(स्थिरांक अचिन्हित *a, स्थिरांक अचिन्हित *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        परत CMP(*a, *b);
}
