/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

శూన్య *xbsearch_r(స్థిరరాశి శూన్య *కీలకం, స్థిరరాశి శూన్య *base, size_t nmemb, size_t కొలత,
                 comparison_userdata_fn_t compar, శూన్య *arg);

#నిర్వచించు typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                స్థిరరాశి typeof((b)[0]) *_k = k;                           \
                పూర్ణాంకం (*_func_)(స్థిరరాశి typeof((b)[0])*, స్థిరరాశి typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((స్థిరరాశి శూన్య*) _k, (b), (n), పరిమాణం((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

శూన్య* bsearch_సురక్షిత_అంతర్గత(స్థిరరాశి శూన్య *కీలకం, స్థిరరాశి శూన్య *base, size_t nmemb, size_t కొలత, comparison_fn_t compar);
#నిర్వచించు bsearch_సురక్షిత(కీలకం, base, nmemb, కొలత, compar) \
        const_generic((base), bsearch_సురక్షిత_అంతర్గత(కీలకం, base, nmemb, కొలత, compar))

#నిర్వచించు typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                స్థిరరాశి typeof((b)[0]) *_k = k;                           \
                పూర్ణాంకం (*_func_)(స్థిరరాశి typeof((b)[0])*, స్థిరరాశి typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_సురక్షిత((స్థిరరాశి శూన్య*) _k, (b), (n), పరిమాణం((b)[0]), (comparison_fn_t) _func_); \
        })

శూన్య qsort_సురక్షిత(శూన్య *base, size_t nmemb, size_t కొలత, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#నిర్వచించు typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                పూర్ణాంకం (*_func_)(స్థిరరాశి typeof((p)[0])*, స్థిరరాశి typeof((p)[0])*) = func; \
                qsort_సురక్షిత((p), (n), పరిమాణం((p)[0]), (comparison_fn_t) _func_); \
        })

శూన్య qsort_r_సురక్షిత(శూన్య *base, size_t nmemb, size_t కొలత, comparison_userdata_fn_t compar, శూన్య *userdata);

#నిర్వచించు typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                పూర్ణాంకం (*_func_)(స్థిరరాశి typeof((p)[0])*, స్థిరరాశి typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_సురక్షిత((p), (n), పరిమాణం((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

పూర్ణాంకం cmp_int(స్థిరరాశి పూర్ణాంకం *a, స్థిరరాశి పూర్ణాంకం *b);
పూర్ణాంకం cmp_uint16(స్థిరరాశి uint16_t *a, స్థిరరాశి uint16_t *b);
పూర్ణాంకం cmp_unsigned(స్థిరరాశి గుర్తులేని *a, స్థిరరాశి గుర్తులేని *b);
