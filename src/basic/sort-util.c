/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
வெறுமை *xbsearch_r(மாறிலி வெறுமை *சாவி, மாறிலி வெறுமை *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, வெறுமை *arg) {
        size_t l, u, idx;
        மாறிலி வெறுமை *p;
        முழு comparison;

        ப்ரதிஜ்ஞா(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        போது (l < u) {
                idx = (l + u) / 2;
                p = (மாறிலி uint8_t*) base + idx * size;
                comparison = compar(சாவி, p, arg);
                எனில் (comparison < 0)
                        u = idx;
                இல்லையெனில் எனில் (comparison > 0)
                        l = idx + 1;
                இல்லையெனில்
                        திரும்பு (வெறுமை *)p;
        }
        திரும்பு வெற்று;
}

வெறுமை* bsearch_safe_internal(மாறிலி வெறுமை *சாவி, மாறிலி வெறுமை *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        எனில் (nmemb <= 0)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(base);
        திரும்பு (வெறுமை*) bsearch(சாவி, base, nmemb, size, compar);
}

வெறுமை qsort_safe(வெறுமை *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        எனில் (nmemb <= 1)
                திரும்பு;

        ப்ரதிஜ்ஞா(base);
        qsort(base, nmemb, size, compar);
}

வெறுமை qsort_r_safe(வெறுமை *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, வெறுமை *userdata) {
        எனில் (nmemb <= 1)
                திரும்பு;

        ப்ரதிஜ்ஞா(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

முழு cmp_int(மாறிலி முழு *a, மாறிலி முழு *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        திரும்பு CMP(*a, *b);
}

முழு cmp_uint16(மாறிலி uint16_t *a, மாறிலி uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        திரும்பு CMP(*a, *b);
}

முழு cmp_unsigned(மாறிலி unsigned *a, மாறிலி unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        திரும்பு CMP(*a, *b);
}
