/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
تهی *xbsearch_r(ثابت تهی *کلید, ثابت تهی *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, تهی *arg) {
        size_t l, u, idx;
        ثابت تهی *p;
        صحیح comparison;

        أكد(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        وقتی (l < u) {
                idx = (l + u) / 2;
                p = (ثابت uint8_t*) base + idx * size;
                comparison = compar(کلید, p, arg);
                اگر (comparison < 0)
                        u = idx;
                وگرنه اگر (comparison > 0)
                        l = idx + 1;
                وگرنه
                        بازگشت (تهی *)p;
        }
        بازگشت پوچ;
}

تهی* bsearch_safe_internal(ثابت تهی *کلید, ثابت تهی *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        اگر (nmemb <= 0)
                بازگشت پوچ;

        أكد(base);
        بازگشت (تهی*) bsearch(کلید, base, nmemb, size, compar);
}

تهی qsort_safe(تهی *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        اگر (nmemb <= 1)
                بازگشت;

        أكد(base);
        qsort(base, nmemb, size, compar);
}

تهی qsort_r_safe(تهی *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, تهی *userdata) {
        اگر (nmemb <= 1)
                بازگشت;

        أكد(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

صحیح cmp_int(ثابت صحیح *a, ثابت صحیح *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        بازگشت CMP(*a, *b);
}

صحیح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        بازگشت CMP(*a, *b);
}

صحیح cmp_unsigned(ثابت unsigned *a, ثابت unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        بازگشت CMP(*a, *b);
}
