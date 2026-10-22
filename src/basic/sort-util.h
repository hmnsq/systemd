/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تهی *xbsearch_r(ثابت تهی *کلید, ثابت تهی *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, تهی *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحیح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ثابت تهی*) _k, (b), (n), اندازه((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

تهی* bsearch_safe_internal(ثابت تهی *کلید, ثابت تهی *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(کلید, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(کلید, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحیح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((ثابت تهی*) _k, (b), (n), اندازه((b)[0]), (comparison_fn_t) _func_); \
        })

تهی qsort_safe(تهی *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                صحیح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*) = func; \
                qsort_safe((p), (n), اندازه((p)[0]), (comparison_fn_t) _func_); \
        })

تهی qsort_r_safe(تهی *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, تهی *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                صحیح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), اندازه((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

صحیح cmp_int(ثابت صحیح *a, ثابت صحیح *b);
صحیح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b);
صحیح cmp_unsigned(ثابت unsigned *a, ثابت unsigned *b);
