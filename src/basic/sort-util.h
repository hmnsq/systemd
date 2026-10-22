/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಶೂನ್ಯ *xbsearch_r(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2,
                 comparison_userdata_fn_t compar, ಶೂನ್ಯ *arg);

#ವ್ಯಾಖ್ಯಾನಿಸು typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ typeof((b)[0]) *_k = k;                           \
                ಪೂರ್ಣಾಂಕ (*_func_)(ಸ್ಥಿರಾಂಕ typeof((b)[0])*, ಸ್ಥಿರಾಂಕ typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*) _k, (b), (n), ಗಾತ್ರ((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ಶೂನ್ಯ* bsearch_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2, comparison_fn_t compar);
#ವ್ಯಾಖ್ಯಾನಿಸು bsearch_ಸುರಕ್ಷಿತ(ಕೀಲಿ, base, nmemb, ಗಾತ್ರ_2, compar) \
        const_generic((base), bsearch_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(ಕೀಲಿ, base, nmemb, ಗಾತ್ರ_2, compar))

#ವ್ಯಾಖ್ಯಾನಿಸು typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ typeof((b)[0]) *_k = k;                           \
                ಪೂರ್ಣಾಂಕ (*_func_)(ಸ್ಥಿರಾಂಕ typeof((b)[0])*, ಸ್ಥಿರಾಂಕ typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_ಸುರಕ್ಷಿತ((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*) _k, (b), (n), ಗಾತ್ರ((b)[0]), (comparison_fn_t) _func_); \
        })

ಶೂನ್ಯ qsort_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#ವ್ಯಾಖ್ಯಾನಿಸು typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ (*_func_)(ಸ್ಥಿರಾಂಕ typeof((p)[0])*, ಸ್ಥಿರಾಂಕ typeof((p)[0])*) = func; \
                qsort_ಸುರಕ್ಷಿತ((p), (n), ಗಾತ್ರ((p)[0]), (comparison_fn_t) _func_); \
        })

ಶೂನ್ಯ qsort_r_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ *base, size_t nmemb, size_t ಗಾತ್ರ_2, comparison_userdata_fn_t compar, ಶೂನ್ಯ *userdata);

#ವ್ಯಾಖ್ಯಾನಿಸು typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ (*_func_)(ಸ್ಥಿರಾಂಕ typeof((p)[0])*, ಸ್ಥಿರಾಂಕ typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_ಸುರಕ್ಷಿತ((p), (n), ಗಾತ್ರ((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ಪೂರ್ಣಾಂಕ cmp_int(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *a, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *b);
ಪೂರ್ಣಾಂಕ cmp_uint16(ಸ್ಥಿರಾಂಕ uint16_t *a, ಸ್ಥಿರಾಂಕ uint16_t *b);
ಪೂರ್ಣಾಂಕ cmp_unsigned(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ *a, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ *b);
