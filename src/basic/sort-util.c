/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
خالی *xbsearch_r(ثابت خالی *کلید, ثابت خالی *base, size_t nmemb, size_t سائز,
                 comparison_userdata_fn_t compar, خالی *arg) {
        size_t l, u, idx;
        ثابت خالی *p;
        صحیح comparison;

        أكد(!سائز_multiply_حد_سے_تجاوز(nmemb, سائز));

        l = 0;
        u = nmemb;
        جب (l < u) {
                idx = (l + u) / 2;
                p = (ثابت uint8_t*) base + idx * سائز;
                comparison = compar(کلید, p, arg);
                اگر (comparison < 0)
                        u = idx;
                ورنہ اگر (comparison > 0)
                        l = idx + 1;
                ورنہ
                        واپس (خالی *)p;
        }
        واپس تہی;
}

خالی* bsearch_محفوظ_اندرونی(ثابت خالی *کلید, ثابت خالی *base, size_t nmemb, size_t سائز, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        اگر (nmemb <= 0)
                واپس تہی;

        أكد(base);
        واپس (خالی*) bsearch(کلید, base, nmemb, سائز, compar);
}

خالی qsort_محفوظ(خالی *base, size_t nmemb, size_t سائز, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        اگر (nmemb <= 1)
                واپس;

        أكد(base);
        qsort(base, nmemb, سائز, compar);
}

خالی qsort_r_محفوظ(خالی *base, size_t nmemb, size_t سائز, comparison_userdata_fn_t compar, خالی *userdata) {
        اگر (nmemb <= 1)
                واپس;

        أكد(base);
        qsort_r(base, nmemb, سائز, compar, userdata);
}

صحیح cmp_int(ثابت صحیح *a, ثابت صحیح *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        واپس CMP(*a, *b);
}

صحیح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        واپس CMP(*a, *b);
}

صحیح cmp_unsigned(ثابت غیرعلامتی *a, ثابت غیرعلامتی *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        واپس CMP(*a, *b);
}
