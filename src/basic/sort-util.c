/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
శూన్య *xbsearch_r(స్థిరరాశి శూన్య *కీలకం, స్థిరరాశి శూన్య *base, size_t nmemb, size_t కొలత,
                 comparison_userdata_fn_t compar, శూన్య *arg) {
        size_t l, u, idx;
        స్థిరరాశి శూన్య *p;
        పూర్ణాంకం comparison;

        ప్రతిజ్ఞా(!కొలత_multiply_అధికప్రవాహం(nmemb, కొలత));

        l = 0;
        u = nmemb;
        అయితే (l < u) {
                idx = (l + u) / 2;
                p = (స్థిరరాశి uint8_t*) base + idx * కొలత;
                comparison = compar(కీలకం, p, arg);
                ఒకవేళ (comparison < 0)
                        u = idx;
                లేకపోతే ఒకవేళ (comparison > 0)
                        l = idx + 1;
                లేకపోతే
                        తిరిగి (శూన్య *)p;
        }
        తిరిగి ఖాళీ;
}

శూన్య* bsearch_సురక్షిత_అంతర్గత(స్థిరరాశి శూన్య *కీలకం, స్థిరరాశి శూన్య *base, size_t nmemb, size_t కొలత, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        ఒకవేళ (nmemb <= 0)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(base);
        తిరిగి (శూన్య*) bsearch(కీలకం, base, nmemb, కొలత, compar);
}

శూన్య qsort_సురక్షిత(శూన్య *base, size_t nmemb, size_t కొలత, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        ఒకవేళ (nmemb <= 1)
                తిరిగి;

        ప్రతిజ్ఞా(base);
        qsort(base, nmemb, కొలత, compar);
}

శూన్య qsort_r_సురక్షిత(శూన్య *base, size_t nmemb, size_t కొలత, comparison_userdata_fn_t compar, శూన్య *userdata) {
        ఒకవేళ (nmemb <= 1)
                తిరిగి;

        ప్రతిజ్ఞా(base);
        qsort_r(base, nmemb, కొలత, compar, userdata);
}

పూర్ణాంకం cmp_int(స్థిరరాశి పూర్ణాంకం *a, స్థిరరాశి పూర్ణాంకం *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        సూచిక_MAY_BE_NULL(a);
        సూచిక_MAY_BE_NULL(b);

        తిరిగి CMP(*a, *b);
}

పూర్ణాంకం cmp_uint16(స్థిరరాశి uint16_t *a, స్థిరరాశి uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        సూచిక_MAY_BE_NULL(a);
        సూచిక_MAY_BE_NULL(b);

        తిరిగి CMP(*a, *b);
}

పూర్ణాంకం cmp_unsigned(స్థిరరాశి గుర్తులేని *a, స్థిరరాశి గుర్తులేని *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        సూచిక_MAY_BE_NULL(a);
        సూచిక_MAY_BE_NULL(b);

        తిరిగి CMP(*a, *b);
}
