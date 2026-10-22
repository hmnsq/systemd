/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

शून्य *xbsearch_r(स्थिरांक शून्य *किल्ली, स्थिरांक शून्य *base, माप_प्रकार nmemb, माप_प्रकार size,
                 comparison_userdata_fn_t compar, शून्य *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                स्थिरांक typeof((b)[0]) *_k = k;                           \
                पूर्णांक (*_func_)(स्थिरांक typeof((b)[0])*, स्थिरांक typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((स्थिरांक शून्य*) _k, (b), (n), आकार((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

शून्य* bsearch_safe_internal(स्थिरांक शून्य *किल्ली, स्थिरांक शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar);
#define bsearch_safe(किल्ली, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(किल्ली, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                स्थिरांक typeof((b)[0]) *_k = k;                           \
                पूर्णांक (*_func_)(स्थिरांक typeof((b)[0])*, स्थिरांक typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((स्थिरांक शून्य*) _k, (b), (n), आकार((b)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                पूर्णांक (*_func_)(स्थिरांक typeof((p)[0])*, स्थिरांक typeof((p)[0])*) = func; \
                qsort_safe((p), (n), आकार((p)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_r_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_userdata_fn_t compar, शून्य *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                पूर्णांक (*_func_)(स्थिरांक typeof((p)[0])*, स्थिरांक typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), आकार((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

पूर्णांक cmp_int(स्थिरांक पूर्णांक *a, स्थिरांक पूर्णांक *b);
पूर्णांक cmp_uint16(स्थिरांक uint16_t *a, स्थिरांक uint16_t *b);
पूर्णांक cmp_unsigned(स्थिरांक अचिन्हित *a, स्थिरांक अचिन्हित *b);
