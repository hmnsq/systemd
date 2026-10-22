/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
શૂન્ય *xbsearch_r(અચલ શૂન્ય *ચાવી, અચલ શૂન્ય *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, શૂન્ય *arg) {
        size_t l, u, idx;
        અચલ શૂન્ય *p;
        પૂર્ણાંક comparison;

        પ્રતિજ્ઞા(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        જ્યારે (l < u) {
                idx = (l + u) / 2;
                p = (અચલ uint8_t*) base + idx * size;
                comparison = compar(ચાવી, p, arg);
                જો (comparison < 0)
                        u = idx;
                નહીંતર જો (comparison > 0)
                        l = idx + 1;
                નહીંતર
                        પાછા (શૂન્ય *)p;
        }
        પાછા ખાલી;
}

શૂન્ય* bsearch_safe_internal(અચલ શૂન્ય *ચાવી, અચલ શૂન્ય *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        જો (nmemb <= 0)
                પાછા ખાલી;

        પ્રતિજ્ઞા(base);
        પાછા (શૂન્ય*) bsearch(ચાવી, base, nmemb, size, compar);
}

શૂન્ય qsort_safe(શૂન્ય *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        જો (nmemb <= 1)
                પાછા;

        પ્રતિજ્ઞા(base);
        qsort(base, nmemb, size, compar);
}

શૂન્ય qsort_r_safe(શૂન્ય *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, શૂન્ય *userdata) {
        જો (nmemb <= 1)
                પાછા;

        પ્રતિજ્ઞા(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

પૂર્ણાંક cmp_int(અચલ પૂર્ણાંક *a, અચલ પૂર્ણાંક *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        પાછા CMP(*a, *b);
}

પૂર્ણાંક cmp_uint16(અચલ uint16_t *a, અચલ uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        પાછા CMP(*a, *b);
}

પૂર્ણાંક cmp_unsigned(અચલ unsigned *a, અચલ unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        પાછા CMP(*a, *b);
}
