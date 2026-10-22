/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
فراغ *xbsearch_r(ثابت فراغ *کلید, ثابت فراغ *base, size_t nmemb, size_t ناپ,
                 comparison_userdata_fn_t compar, فراغ *arg) {
        size_t l, u, idx;
        ثابت فراغ *p;
        صحيح comparison;

        أكد(!ناپ_multiply_حد_توں_ودھ(nmemb, ناپ));

        l = 0;
        u = nmemb;
        طالما (l < u) {
                idx = (l + u) / 2;
                p = (ثابت uint8_t*) base + idx * ناپ;
                comparison = compar(کلید, p, arg);
                إذا (comparison < 0)
                        u = idx;
                وإلا إذا (comparison > 0)
                        l = idx + 1;
                وإلا
                        أرجع (فراغ *)p;
        }
        أرجع عدم;
}

فراغ* bsearch_محفوظ_اندرونی(ثابت فراغ *کلید, ثابت فراغ *base, size_t nmemb, size_t ناپ, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        إذا (nmemb <= 0)
                أرجع عدم;

        أكد(base);
        أرجع (فراغ*) bsearch(کلید, base, nmemb, ناپ, compar);
}

فراغ qsort_محفوظ(فراغ *base, size_t nmemb, size_t ناپ, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        إذا (nmemb <= 1)
                أرجع;

        أكد(base);
        qsort(base, nmemb, ناپ, compar);
}

فراغ qsort_r_محفوظ(فراغ *base, size_t nmemb, size_t ناپ, comparison_userdata_fn_t compar, فراغ *userdata) {
        إذا (nmemb <= 1)
                أرجع;

        أكد(base);
        qsort_r(base, nmemb, ناپ, compar, userdata);
}

صحيح cmp_int(ثابت صحيح *a, ثابت صحيح *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}

صحيح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}

صحيح cmp_unsigned(ثابت بنانشانی *a, ثابت بنانشانی *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}
