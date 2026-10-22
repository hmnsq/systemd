/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

শূন্য *xbsearch_r(ধ্রুবক শূন্য *চাবি, ধ্রুবক শূন্য *base, size_t nmemb, size_t আকার_2,
                 comparison_userdata_fn_t compar, শূন্য *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ধ্রুবক typeof((b)[0]) *_k = k;                           \
                পূর্ণসংখ্যা (*_func_)(ধ্রুবক typeof((b)[0])*, ধ্রুবক typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ধ্রুবক শূন্য*) _k, (b), (n), আকার((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

শূন্য* bsearch_safe_internal(ধ্রুবক শূন্য *চাবি, ধ্রুবক শূন্য *base, size_t nmemb, size_t আকার_2, comparison_fn_t compar);
#define bsearch_safe(চাবি, base, nmemb, আকার_2, compar) \
        const_generic((base), bsearch_safe_internal(চাবি, base, nmemb, আকার_2, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ধ্রুবক typeof((b)[0]) *_k = k;                           \
                পূর্ণসংখ্যা (*_func_)(ধ্রুবক typeof((b)[0])*, ধ্রুবক typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((ধ্রুবক শূন্য*) _k, (b), (n), আকার((b)[0]), (comparison_fn_t) _func_); \
        })

শূন্য qsort_safe(শূন্য *base, size_t nmemb, size_t আকার_2, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                পূর্ণসংখ্যা (*_func_)(ধ্রুবক typeof((p)[0])*, ধ্রুবক typeof((p)[0])*) = func; \
                qsort_safe((p), (n), আকার((p)[0]), (comparison_fn_t) _func_); \
        })

শূন্য qsort_r_safe(শূন্য *base, size_t nmemb, size_t আকার_2, comparison_userdata_fn_t compar, শূন্য *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                পূর্ণসংখ্যা (*_func_)(ধ্রুবক typeof((p)[0])*, ধ্রুবক typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), আকার((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

পূর্ণসংখ্যা cmp_int(ধ্রুবক পূর্ণসংখ্যা *a, ধ্রুবক পূর্ণসংখ্যা *b);
পূর্ণসংখ্যা cmp_uint16(ধ্রুবক uint16_t *a, ধ্রুবক uint16_t *b);
পূর্ণসংখ্যা cmp_unsigned(ধ্রুবক unsigned *a, ধ্রুবক unsigned *b);
