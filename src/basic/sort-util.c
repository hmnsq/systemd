/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
शून्य *xbsearch_r(अचर शून्य *साँचो, अचर शून्य *base, माप_प्रकार nmemb, माप_प्रकार size,
                 comparison_userdata_fn_t compar, शून्य *arg) {
        माप_प्रकार l, u, idx;
        अचर शून्य *p;
        पूर्णाङ्क comparison;

        प्रतिज्ञा(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        जबसम्म (l < u) {
                idx = (l + u) / 2;
                p = (अचर uint8_t*) base + idx * size;
                comparison = compar(साँचो, p, arg);
                यदि (comparison < 0)
                        u = idx;
                अन्यथा यदि (comparison > 0)
                        l = idx + 1;
                अन्यथा
                        फर्काऊ (शून्य *)p;
        }
        फर्काऊ रिक्त;
}

शून्य* bsearch_safe_internal(अचर शून्य *साँचो, अचर शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        यदि (nmemb <= 0)
                फर्काऊ रिक्त;

        प्रतिज्ञा(base);
        फर्काऊ (शून्य*) द्वा_खोज(साँचो, base, nmemb, size, compar);
}

शून्य qsort_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        यदि (nmemb <= 1)
                फर्काऊ;

        प्रतिज्ञा(base);
        क्विक(base, nmemb, size, compar);
}

शून्य qsort_r_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_userdata_fn_t compar, शून्य *userdata) {
        यदि (nmemb <= 1)
                फर्काऊ;

        प्रतिज्ञा(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

पूर्णाङ्क cmp_int(अचर पूर्णाङ्क *a, अचर पूर्णाङ्क *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        फर्काऊ CMP(*a, *b);
}

पूर्णाङ्क cmp_uint16(अचर uint16_t *a, अचर uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        फर्काऊ CMP(*a, *b);
}

पूर्णाङ्क cmp_unsigned(अचर अचिन्हित *a, अचर अचिन्हित *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        फर्काऊ CMP(*a, *b);
}
