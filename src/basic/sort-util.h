/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

শূন্য *xbsearch_r(ধ্রুৱ শূন্য *চাবি, ধ্রুৱ শূন্য *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, শূন্য *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ধ্রুৱ typeof((b)[0]) *_k = k;                           \
                পূর্ণাঙ্ক (*_func_)(ধ্রুৱ typeof((b)[0])*, ধ্রুৱ typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ধ্রুৱ শূন্য*) _k, (b), (n), পরিমাণ((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

শূন্য* bsearch_safe_internal(ধ্রুৱ শূন্য *চাবি, ধ্রুৱ শূন্য *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(চাবি, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(চাবি, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ধ্রুৱ typeof((b)[0]) *_k = k;                           \
                পূর্ণাঙ্ক (*_func_)(ধ্রুৱ typeof((b)[0])*, ধ্রুৱ typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((ধ্রুৱ শূন্য*) _k, (b), (n), পরিমাণ((b)[0]), (comparison_fn_t) _func_); \
        })

শূন্য qsort_safe(শূন্য *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                পূর্ণাঙ্ক (*_func_)(ধ্রুৱ typeof((p)[0])*, ধ্রুৱ typeof((p)[0])*) = func; \
                qsort_safe((p), (n), পরিমাণ((p)[0]), (comparison_fn_t) _func_); \
        })

শূন্য qsort_r_safe(শূন্য *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, শূন্য *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                পূর্ণাঙ্ক (*_func_)(ধ্রুৱ typeof((p)[0])*, ধ্রুৱ typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), পরিমাণ((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

পূর্ণাঙ্ক cmp_int(ধ্রুৱ পূর্ণাঙ্ক *a, ধ্রুৱ পূর্ণাঙ্ক *b);
পূর্ণাঙ্ক cmp_uint16(ধ্রুৱ uint16_t *a, ধ্রুৱ uint16_t *b);
পূর্ণাঙ্ক cmp_unsigned(ধ্রুৱ unsigned *a, ধ্রুৱ unsigned *b);
