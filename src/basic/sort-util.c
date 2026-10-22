/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
ಶೂನ್ಯ *xbsearch_r(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2,
                 comparison_userdata_fn_t compar, ಶೂನ್ಯ *arg) {
        size_t l, u, idx;
        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p;
        ಪೂರ್ಣಾಂಕ comparison;

        ಪ್ರತಿಜ್ಞಾ(!ಗಾತ್ರ_multiply_ಅತಿಪ್ರವಾಹ(nmemb, ಗಾತ್ರ_2));

        l = 0;
        u = nmemb;
        ಆಗ (l < u) {
                idx = (l + u) / 2;
                p = (ಸ್ಥಿರಾಂಕ uint8_t*) base + idx * ಗಾತ್ರ_2;
                comparison = compar(ಕೀಲಿ, p, arg);
                ಆದರೆ (comparison < 0)
                        u = idx;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (comparison > 0)
                        l = idx + 1;
                ಇಲ್ಲದಿದ್ದರೆ
                        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)p;
        }
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ* bsearch_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        ಆದರೆ (nmemb <= 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(base);
        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*) bsearch(ಕೀಲಿ, base, nmemb, ಗಾತ್ರ_2, compar);
}

ಶೂನ್ಯ qsort_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        ಆದರೆ (nmemb <= 1)
                ಹಿಂತಿರುಗಿ;

        ಪ್ರತಿಜ್ಞಾ(base);
        qsort(base, nmemb, ಗಾತ್ರ_2, compar);
}

ಶೂನ್ಯ qsort_r_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2, comparison_userdata_fn_t compar, ಶೂನ್ಯ *userdata) {
        ಆದರೆ (nmemb <= 1)
                ಹಿಂತಿರುಗಿ;

        ಪ್ರತಿಜ್ಞಾ(base);
        qsort_r(base, nmemb, ಗಾತ್ರ_2, compar, userdata);
}

ಪೂರ್ಣಾಂಕ cmp_int(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *a, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ಸೂಚಕ_MAY_BE_NULL(a);
        ಸೂಚಕ_MAY_BE_NULL(b);

        ಹಿಂತಿರುಗಿ CMP(*a, *b);
}

ಪೂರ್ಣಾಂಕ cmp_uint16(ಸ್ಥಿರಾಂಕ uint16_t *a, ಸ್ಥಿರಾಂಕ uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ಸೂಚಕ_MAY_BE_NULL(a);
        ಸೂಚಕ_MAY_BE_NULL(b);

        ಹಿಂತಿರುಗಿ CMP(*a, *b);
}

ಪೂರ್ಣಾಂಕ cmp_unsigned(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ *a, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ಸೂಚಕ_MAY_BE_NULL(a);
        ಸೂಚಕ_MAY_BE_NULL(b);

        ಹಿಂತಿರುಗಿ CMP(*a, *b);
}
