/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
فراغ *xbsearch_r(ثابت فراغ *کلید, ثابت فراغ *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, فراغ *arg) {
        size_t l, u, idx;
        ثابت فراغ *p;
        صحيح comparison;

        أكد(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        طالما (l < u) {
                idx = (l + u) / 2;
                p = (ثابت uint8_t*) base + idx * size;
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

فراغ* bsearch_safe_internal(ثابت فراغ *کلید, ثابت فراغ *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        إذا (nmemb <= 0)
                أرجع عدم;

        أكد(base);
        أرجع (فراغ*) bsearch(کلید, base, nmemb, size, compar);
}

فراغ qsort_safe(فراغ *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        إذا (nmemb <= 1)
                أرجع;

        أكد(base);
        qsort(base, nmemb, size, compar);
}

فراغ qsort_r_safe(فراغ *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, فراغ *userdata) {
        إذا (nmemb <= 1)
                أرجع;

        أكد(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

صحيح cmp_int(ثابت صحيح *a, ثابت صحيح *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}

صحيح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}

صحيح cmp_unsigned(ثابت unsigned *a, ثابت unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        أرجع CMP(*a, *b);
}
