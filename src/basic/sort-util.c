/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
शून्य *xbsearch_r(ध्रुव शून्य *कुंजी, ध्रुव शून्य *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, शून्य *arg) {
        size_t l, u, idx;
        ध्रुव शून्य *p;
        पूर्णाङ्क comparison;

        प्रतिज्ञा(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        यावत् (l < u) {
                idx = (l + u) / 2;
                p = (ध्रुव uint8_t*) base + idx * size;
                comparison = compar(कुंजी, p, arg);
                यदि (comparison < 0)
                        u = idx;
                अन्यथा यदि (comparison > 0)
                        l = idx + 1;
                अन्यथा
                        प्रत्यागम (शून्य *)p;
        }
        प्रत्यागम रिक्त;
}

शून्य* bsearch_safe_internal(ध्रुव शून्य *कुंजी, ध्रुव शून्य *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        यदि (nmemb <= 0)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(base);
        प्रत्यागम (शून्य*) bsearch(कुंजी, base, nmemb, size, compar);
}

शून्य qsort_safe(शून्य *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        यदि (nmemb <= 1)
                प्रत्यागम;

        प्रतिज्ञा(base);
        qsort(base, nmemb, size, compar);
}

शून्य qsort_r_safe(शून्य *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, शून्य *userdata) {
        यदि (nmemb <= 1)
                प्रत्यागम;

        प्रतिज्ञा(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

पूर्णाङ्क cmp_int(ध्रुव पूर्णाङ्क *a, ध्रुव पूर्णाङ्क *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        प्रत्यागम CMP(*a, *b);
}

पूर्णाङ्क cmp_uint16(ध्रुव uint16_t *a, ध्रुव uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        प्रत्यागम CMP(*a, *b);
}

पूर्णाङ्क cmp_unsigned(ध्रुव unsigned *a, ध्रुव unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        प्रत्यागम CMP(*a, *b);
}
