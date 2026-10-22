/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
শূন্য *xbsearch_r(ধ্রুব শূন্য *চাবি, ধ্রুব শূন্য *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, শূন্য *arg) {
        size_t l, u, idx;
        ধ্রুব শূন্য *p;
        পূর্ণাঙ্ক comparison;

        প্রতিজ্ঞা(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        যাবত্ (l < u) {
                idx = (l + u) / 2;
                p = (ধ্রুব uint8_t*) base + idx * size;
                comparison = compar(চাবি, p, arg);
                যদি (comparison < 0)
                        u = idx;
                অন্যথা যদি (comparison > 0)
                        l = idx + 1;
                অন্যথা
                        প্রত্যাগম (শূন্য *)p;
        }
        প্রত্যাগম রিক্ত;
}

শূন্য* bsearch_safe_internal(ধ্রুব শূন্য *চাবি, ধ্রুব শূন্য *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        যদি (nmemb <= 0)
                প্রত্যাগম রিক্ত;

        প্রতিজ্ঞা(base);
        প্রত্যাগম (শূন্য*) bsearch(চাবি, base, nmemb, size, compar);
}

শূন্য qsort_safe(শূন্য *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        যদি (nmemb <= 1)
                প্রত্যাগম;

        প্রতিজ্ঞা(base);
        qsort(base, nmemb, size, compar);
}

শূন্য qsort_r_safe(শূন্য *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, শূন্য *userdata) {
        যদি (nmemb <= 1)
                প্রত্যাগম;

        প্রতিজ্ঞা(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

পূর্ণাঙ্ক cmp_int(ধ্রুব পূর্ণাঙ্ক *a, ধ্রুব পূর্ণাঙ্ক *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        প্রত্যাগম CMP(*a, *b);
}

পূর্ণাঙ্ক cmp_uint16(ধ্রুব uint16_t *a, ধ্রুব uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        প্রত্যাগম CMP(*a, *b);
}

পূর্ণাঙ্ক cmp_unsigned(ধ্রুব unsigned *a, ধ্রুব unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        প্রত্যাগম CMP(*a, *b);
}
