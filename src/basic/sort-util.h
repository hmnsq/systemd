/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಶೂನ್ಯ *xbsearch_r(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, ಶೂನ್ಯ *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ಧ್ರುವ typeof((b)[0]) *_k = k;                           \
                ಪೂರ್ಣಾಙ್ಕ (*_func_)(ಧ್ರುವ typeof((b)[0])*, ಧ್ರುವ typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ಧ್ರುವ ಶೂನ್ಯ*) _k, (b), (n), ಪರಿಮಾಣ((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ಶೂನ್ಯ* bsearch_safe_internal(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(ಕೀಲಿ, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(ಕೀಲಿ, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ಧ್ರುವ typeof((b)[0]) *_k = k;                           \
                ಪೂರ್ಣಾಙ್ಕ (*_func_)(ಧ್ರುವ typeof((b)[0])*, ಧ್ರುವ typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((ಧ್ರುವ ಶೂನ್ಯ*) _k, (b), (n), ಪರಿಮಾಣ((b)[0]), (comparison_fn_t) _func_); \
        })

ಶೂನ್ಯ qsort_safe(ಶೂನ್ಯ *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                ಪೂರ್ಣಾಙ್ಕ (*_func_)(ಧ್ರುವ typeof((p)[0])*, ಧ್ರುವ typeof((p)[0])*) = func; \
                qsort_safe((p), (n), ಪರಿಮಾಣ((p)[0]), (comparison_fn_t) _func_); \
        })

ಶೂನ್ಯ qsort_r_safe(ಶೂನ್ಯ *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, ಶೂನ್ಯ *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                ಪೂರ್ಣಾಙ್ಕ (*_func_)(ಧ್ರುವ typeof((p)[0])*, ಧ್ರುವ typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), ಪರಿಮಾಣ((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ಪೂರ್ಣಾಙ್ಕ cmp_int(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *a, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *b);
ಪೂರ್ಣಾಙ್ಕ cmp_uint16(ಧ್ರುವ uint16_t *a, ಧ್ರುವ uint16_t *b);
ಪೂರ್ಣಾಙ್ಕ cmp_unsigned(ಧ್ರುವ unsigned *a, ಧ್ರುವ unsigned *b);
