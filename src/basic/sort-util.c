/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
ಶೂನ್ಯ *xbsearch_r(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, ಶೂನ್ಯ *arg) {
        size_t l, u, idx;
        ಧ್ರುವ ಶೂನ್ಯ *p;
        ಪೂರ್ಣಾಙ್ಕ comparison;

        ಪ್ರತಿಜ್ಞಾ(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        ಯಾವತ್ (l < u) {
                idx = (l + u) / 2;
                p = (ಧ್ರುವ uint8_t*) base + idx * size;
                comparison = compar(ಕೀಲಿ, p, arg);
                ಯದಿ (comparison < 0)
                        u = idx;
                ಅನ್ಯಥಾ ಯದಿ (comparison > 0)
                        l = idx + 1;
                ಅನ್ಯಥಾ
                        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)p;
        }
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ* bsearch_safe_internal(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        ಯದಿ (nmemb <= 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(base);
        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ*) bsearch(ಕೀಲಿ, base, nmemb, size, compar);
}

ಶೂನ್ಯ qsort_safe(ಶೂನ್ಯ *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        ಯದಿ (nmemb <= 1)
                ಪ್ರತ್ಯಾಗಮ;

        ಪ್ರತಿಜ್ಞಾ(base);
        qsort(base, nmemb, size, compar);
}

ಶೂನ್ಯ qsort_r_safe(ಶೂನ್ಯ *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, ಶೂನ್ಯ *userdata) {
        ಯದಿ (nmemb <= 1)
                ಪ್ರತ್ಯಾಗಮ;

        ಪ್ರತಿಜ್ಞಾ(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

ಪೂರ್ಣಾಙ್ಕ cmp_int(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *a, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ಪ್ರತ್ಯಾಗಮ CMP(*a, *b);
}

ಪೂರ್ಣಾಙ್ಕ cmp_uint16(ಧ್ರುವ uint16_t *a, ಧ್ರುವ uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ಪ್ರತ್ಯಾಗಮ CMP(*a, *b);
}

ಪೂರ್ಣಾಙ್ಕ cmp_unsigned(ಧ್ರುವ unsigned *a, ಧ್ರುವ unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ಪ್ರತ್ಯಾಗಮ CMP(*a, *b);
}
