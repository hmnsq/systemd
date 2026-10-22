/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

शून्य *xbsearch_r(अचर शून्य *साँचो, अचर शून्य *base, माप_प्रकार nmemb, माप_प्रकार size,
                 comparison_userdata_fn_t compar, शून्य *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                अचर typeof((b)[0]) *_k = k;                           \
                पूर्णाङ्क (*_func_)(अचर typeof((b)[0])*, अचर typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((अचर शून्य*) _k, (b), (n), परिमाण((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

शून्य* bsearch_safe_internal(अचर शून्य *साँचो, अचर शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar);
#define bsearch_safe(साँचो, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(साँचो, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                अचर typeof((b)[0]) *_k = k;                           \
                पूर्णाङ्क (*_func_)(अचर typeof((b)[0])*, अचर typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((अचर शून्य*) _k, (b), (n), परिमाण((b)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                पूर्णाङ्क (*_func_)(अचर typeof((p)[0])*, अचर typeof((p)[0])*) = func; \
                qsort_safe((p), (n), परिमाण((p)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_r_safe(शून्य *base, माप_प्रकार nmemb, माप_प्रकार size, comparison_userdata_fn_t compar, शून्य *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                पूर्णाङ्क (*_func_)(अचर typeof((p)[0])*, अचर typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), परिमाण((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

पूर्णाङ्क cmp_int(अचर पूर्णाङ्क *a, अचर पूर्णाङ्क *b);
पूर्णाङ्क cmp_uint16(अचर uint16_t *a, अचर uint16_t *b);
पूर्णाङ्क cmp_unsigned(अचर अचिन्हित *a, अचर अचिन्हित *b);
