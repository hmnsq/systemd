/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ଶୂନ୍ୟ *xbsearch_r(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଅଚଳ ଶୂନ୍ୟ *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, ଶୂନ୍ୟ *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ଅଚଳ typeof((b)[0]) *_k = k;                           \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*_func_)(ଅଚଳ typeof((b)[0])*, ଅଚଳ typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ଅଚଳ ଶୂନ୍ୟ*) _k, (b), (n), ଆକାର((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ଶୂନ୍ୟ* bsearch_safe_internal(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଅଚଳ ଶୂନ୍ୟ *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(ଚାବି, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(ଚାବି, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ଅଚଳ typeof((b)[0]) *_k = k;                           \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*_func_)(ଅଚଳ typeof((b)[0])*, ଅଚଳ typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((ଅଚଳ ଶୂନ୍ୟ*) _k, (b), (n), ଆକାର((b)[0]), (comparison_fn_t) _func_); \
        })

ଶୂନ୍ୟ qsort_safe(ଶୂନ୍ୟ *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*_func_)(ଅଚଳ typeof((p)[0])*, ଅଚଳ typeof((p)[0])*) = func; \
                qsort_safe((p), (n), ଆକାର((p)[0]), (comparison_fn_t) _func_); \
        })

ଶୂନ୍ୟ qsort_r_safe(ଶୂନ୍ୟ *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, ଶୂନ୍ୟ *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*_func_)(ଅଚଳ typeof((p)[0])*, ଅଚଳ typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), ଆକାର((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_int(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *a, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *b);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_uint16(ଅଚଳ uint16_t *a, ଅଚଳ uint16_t *b);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_unsigned(ଅଚଳ unsigned *a, ଅଚଳ unsigned *b);
