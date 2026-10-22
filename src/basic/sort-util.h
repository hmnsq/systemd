/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

व्योम *xbsearch_r(स्थिर व्योम *कुंजी, स्थिर व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार,
                 comparison_userdata_fn_t compar, व्योम *arg);

#घोषणा typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                स्थिर typeof((b)[0]) *_k = k;                           \
                पूर्णांक (*_func_)(स्थिर typeof((b)[0])*, स्थिर typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((स्थिर व्योम*) _k, (b), (n), माप((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

व्योम* bsearch_safe_internal(स्थिर व्योम *कुंजी, स्थिर व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार, comparison_fn_t compar);
#घोषणा bsearch_safe(कुंजी, base, nmemb, आकार, compar) \
        const_generic((base), bsearch_safe_internal(कुंजी, base, nmemb, आकार, compar))

#घोषणा typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                स्थिर typeof((b)[0]) *_k = k;                           \
                पूर्णांक (*_func_)(स्थिर typeof((b)[0])*, स्थिर typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((स्थिर व्योम*) _k, (b), (n), माप((b)[0]), (comparison_fn_t) _func_); \
        })

व्योम qsort_safe(व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#घोषणा typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                पूर्णांक (*_func_)(स्थिर typeof((p)[0])*, स्थिर typeof((p)[0])*) = func; \
                qsort_safe((p), (n), माप((p)[0]), (comparison_fn_t) _func_); \
        })

व्योम qsort_r_safe(व्योम *base, माप_प्रकार nmemb, माप_प्रकार आकार, comparison_userdata_fn_t compar, व्योम *userdata);

#घोषणा typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                पूर्णांक (*_func_)(स्थिर typeof((p)[0])*, स्थिर typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), माप((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

पूर्णांक cmp_int(स्थिर पूर्णांक *a, स्थिर पूर्णांक *b);
पूर्णांक cmp_uint16(स्थिर uint16_t *a, स्थिर uint16_t *b);
पूर्णांक cmp_unsigned(स्थिर अचिन्हित *a, स्थिर अचिन्हित *b);
