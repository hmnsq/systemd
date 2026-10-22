/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வெறுமை *xbsearch_r(மாறிலி வெறுமை *சாவி, மாறிலி வெறுமை *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, வெறுமை *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                மாறிலி typeof((b)[0]) *_k = k;                           \
                முழு (*_func_)(மாறிலி typeof((b)[0])*, மாறிலி typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((மாறிலி வெறுமை*) _k, (b), (n), அளவு((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

வெறுமை* bsearch_safe_internal(மாறிலி வெறுமை *சாவி, மாறிலி வெறுமை *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(சாவி, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(சாவி, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                மாறிலி typeof((b)[0]) *_k = k;                           \
                முழு (*_func_)(மாறிலி typeof((b)[0])*, மாறிலி typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((மாறிலி வெறுமை*) _k, (b), (n), அளவு((b)[0]), (comparison_fn_t) _func_); \
        })

வெறுமை qsort_safe(வெறுமை *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                முழு (*_func_)(மாறிலி typeof((p)[0])*, மாறிலி typeof((p)[0])*) = func; \
                qsort_safe((p), (n), அளவு((p)[0]), (comparison_fn_t) _func_); \
        })

வெறுமை qsort_r_safe(வெறுமை *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, வெறுமை *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                முழு (*_func_)(மாறிலி typeof((p)[0])*, மாறிலி typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), அளவு((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

முழு cmp_int(மாறிலி முழு *a, மாறிலி முழு *b);
முழு cmp_uint16(மாறிலி uint16_t *a, மாறிலி uint16_t *b);
முழு cmp_unsigned(மாறிலி unsigned *a, மாறிலி unsigned *b);
