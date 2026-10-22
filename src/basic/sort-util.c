/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
ਖਾਲੀ *xbsearch_r(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ,
                 comparison_userdata_fn_t compar, ਖਾਲੀ *arg) {
        size_t l, u, idx;
        ਅਚਲ ਖਾਲੀ *p;
        ਪੂਰਨ comparison;

        ਪ੍ਰਤਿਜ੍ਞਾ(!ਮਾਪ_multiply_ਉਛਾਲ(nmemb, ਮਾਪ));

        l = 0;
        u = nmemb;
        ਜਦੋਂ (l < u) {
                idx = (l + u) / 2;
                p = (ਅਚਲ uint8_t*) base + idx * ਮਾਪ;
                comparison = compar(ਕੁੰਜੀ, p, arg);
                ਜੇ (comparison < 0)
                        u = idx;
                ਨਹੀਂਤਾਂ ਜੇ (comparison > 0)
                        l = idx + 1;
                ਨਹੀਂਤਾਂ
                        ਵਾਪਸ (ਖਾਲੀ *)p;
        }
        ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ* bsearch_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        ਜੇ (nmemb <= 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(base);
        ਵਾਪਸ (ਖਾਲੀ*) bsearch(ਕੁੰਜੀ, base, nmemb, ਮਾਪ, compar);
}

ਖਾਲੀ qsort_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        ਜੇ (nmemb <= 1)
                ਵਾਪਸ;

        ਪ੍ਰਤਿਜ੍ਞਾ(base);
        qsort(base, nmemb, ਮਾਪ, compar);
}

ਖਾਲੀ qsort_r_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ, comparison_userdata_fn_t compar, ਖਾਲੀ *userdata) {
        ਜੇ (nmemb <= 1)
                ਵਾਪਸ;

        ਪ੍ਰਤਿਜ੍ਞਾ(base);
        qsort_r(base, nmemb, ਮਾਪ, compar, userdata);
}

ਪੂਰਨ cmp_int(ਅਚਲ ਪੂਰਨ *a, ਅਚਲ ਪੂਰਨ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ਸੂਚਕ_MAY_BE_NULL(a);
        ਸੂਚਕ_MAY_BE_NULL(b);

        ਵਾਪਸ CMP(*a, *b);
}

ਪੂਰਨ cmp_uint16(ਅਚਲ uint16_t *a, ਅਚਲ uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ਸੂਚਕ_MAY_BE_NULL(a);
        ਸੂਚਕ_MAY_BE_NULL(b);

        ਵਾਪਸ CMP(*a, *b);
}

ਪੂਰਨ cmp_unsigned(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ *a, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ਸੂਚਕ_MAY_BE_NULL(a);
        ਸੂਚਕ_MAY_BE_NULL(b);

        ਵਾਪਸ CMP(*a, *b);
}
