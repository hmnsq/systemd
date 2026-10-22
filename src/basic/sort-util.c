/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
فراغ *xbsearch_r(ثابت فراغ *کلی, ثابت فراغ *base, size_t nmemb, size_t اندازه,
                 comparison_userdata_fn_t compar, فراغ *arg) {
        size_t l, u, idx;
        ثابت فراغ *p;
        صحيح comparison;

        أكد(!اندازه_multiply_ډکېدنه(nmemb, اندازه));

        l = 0;
        u = nmemb;
        طالما (l < u) {
                idx = (l + u) / 2;
                p = (ثابت uint8_t*) base + idx * اندازه;
                comparison = compar(کلی, p, arg);
                إذا (comparison < 0)
                        u = idx;
                وإلا إذا (comparison > 0)
                        l = idx + 1;
                وإلا
                        أرجع (فراغ *)p;
        }
        أرجع عدم;
}

فراغ* bsearch_خوندي_دننی(ثابت فراغ *کلی, ثابت فراغ *base, size_t nmemb, size_t اندازه, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        إذا (nmemb <= 0)
                أرجع عدم;

        أكد(base);
        أرجع (فراغ*) bsearch(کلی, base, nmemb, اندازه, compar);
}

فراغ qsort_خوندي(فراغ *base, size_t nmemb, size_t اندازه, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        إذا (nmemb <= 1)
                أرجع;

        أكد(base);
        qsort(base, nmemb, اندازه, compar);
}

فراغ qsort_r_خوندي(فراغ *base, size_t nmemb, size_t اندازه, comparison_userdata_fn_t compar, فراغ *userdata) {
        إذا (nmemb <= 1)
                أرجع;

        أكد(base);
        qsort_r(base, nmemb, اندازه, compar, userdata);
}

صحيح cmp_int(ثابت صحيح *a, ثابت صحيح *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشاره_ګر_MAY_BE_NULL(a);
        اشاره_ګر_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}

صحيح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشاره_ګر_MAY_BE_NULL(a);
        اشاره_ګر_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}

صحيح cmp_unsigned(ثابت بینښه *a, ثابت بینښه *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشاره_ګر_MAY_BE_NULL(a);
        اشاره_ګر_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}
