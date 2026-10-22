/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
শূন্য *xbsearch_r(ধ্রুবক শূন্য *চাবি, ধ্রুবক শূন্য *base, size_t nmemb, size_t আকার_2,
                 comparison_userdata_fn_t compar, শূন্য *arg) {
        size_t l, u, idx;
        ধ্রুবক শূন্য *p;
        পূর্ণসংখ্যা comparison;

        প্রতিজ্ঞা(!আকার_multiply_overflow(nmemb, আকার_2));

        l = 0;
        u = nmemb;
        যখন (l < u) {
                idx = (l + u) / 2;
                p = (ধ্রুবক uint8_t*) base + idx * আকার_2;
                comparison = compar(চাবি, p, arg);
                যদি (comparison < 0)
                        u = idx;
                অন্যথা যদি (comparison > 0)
                        l = idx + 1;
                অন্যথা
                        ফেরত (শূন্য *)p;
        }
        ফেরত ফাঁকা;
}

শূন্য* bsearch_safe_internal(ধ্রুবক শূন্য *চাবি, ধ্রুবক শূন্য *base, size_t nmemb, size_t আকার_2, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        যদি (nmemb <= 0)
                ফেরত ফাঁকা;

        প্রতিজ্ঞা(base);
        ফেরত (শূন্য*) bsearch(চাবি, base, nmemb, আকার_2, compar);
}

শূন্য qsort_safe(শূন্য *base, size_t nmemb, size_t আকার_2, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        যদি (nmemb <= 1)
                ফেরত;

        প্রতিজ্ঞা(base);
        qsort(base, nmemb, আকার_2, compar);
}

শূন্য qsort_r_safe(শূন্য *base, size_t nmemb, size_t আকার_2, comparison_userdata_fn_t compar, শূন্য *userdata) {
        যদি (nmemb <= 1)
                ফেরত;

        প্রতিজ্ঞা(base);
        qsort_r(base, nmemb, আকার_2, compar, userdata);
}

পূর্ণসংখ্যা cmp_int(ধ্রুবক পূর্ণসংখ্যা *a, ধ্রুবক পূর্ণসংখ্যা *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ফেরত CMP(*a, *b);
}

পূর্ণসংখ্যা cmp_uint16(ধ্রুবক uint16_t *a, ধ্রুবক uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ফেরত CMP(*a, *b);
}

পূর্ণসংখ্যা cmp_unsigned(ধ্রুবক unsigned *a, ধ্রুবক unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ফেরত CMP(*a, *b);
}
