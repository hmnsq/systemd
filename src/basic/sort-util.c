/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
ଶୂନ୍ୟ *xbsearch_r(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଅଚଳ ଶୂନ୍ୟ *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, ଶୂନ୍ୟ *arg) {
        size_t l, u, idx;
        ଅଚଳ ଶୂନ୍ୟ *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା comparison;

        ପ୍ରତିଜ୍ଞା(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        ଯେତେବେଳେ (l < u) {
                idx = (l + u) / 2;
                p = (ଅଚଳ uint8_t*) base + idx * size;
                comparison = compar(ଚାବି, p, arg);
                ଯଦି (comparison < 0)
                        u = idx;
                ଅନ୍ୟଥା ଯଦି (comparison > 0)
                        l = idx + 1;
                ଅନ୍ୟଥା
                        ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)p;
        }
        ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ* bsearch_safe_internal(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଅଚଳ ଶୂନ୍ୟ *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        ଯଦି (nmemb <= 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(base);
        ଫେରନ୍ତୁ (ଶୂନ୍ୟ*) bsearch(ଚାବି, base, nmemb, size, compar);
}

ଶୂନ୍ୟ qsort_safe(ଶୂନ୍ୟ *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        ଯଦି (nmemb <= 1)
                ଫେରନ୍ତୁ;

        ପ୍ରତିଜ୍ଞା(base);
        qsort(base, nmemb, size, compar);
}

ଶୂନ୍ୟ qsort_r_safe(ଶୂନ୍ୟ *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, ଶୂନ୍ୟ *userdata) {
        ଯଦି (nmemb <= 1)
                ଫେରନ୍ତୁ;

        ପ୍ରତିଜ୍ଞା(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_int(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *a, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ଫେରନ୍ତୁ CMP(*a, *b);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_uint16(ଅଚଳ uint16_t *a, ଅଚଳ uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ଫେରନ୍ତୁ CMP(*a, *b);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_unsigned(ଅଚଳ unsigned *a, ଅଚଳ unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ଫେରନ୍ତୁ CMP(*a, *b);
}
