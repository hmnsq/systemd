/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

शून्य *xbsearch_r(ध्रुव शून्य *कुंजी, ध्रुव शून्य *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, शून्य *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ध्रुव typeof((b)[0]) *_k = k;                           \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((b)[0])*, ध्रुव typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ध्रुव शून्य*) _k, (b), (n), परिमाण((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

शून्य* bsearch_safe_internal(ध्रुव शून्य *कुंजी, ध्रुव शून्य *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(कुंजी, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(कुंजी, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ध्रुव typeof((b)[0]) *_k = k;                           \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((b)[0])*, ध्रुव typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((ध्रुव शून्य*) _k, (b), (n), परिमाण((b)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_safe(शून्य *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((p)[0])*, ध्रुव typeof((p)[0])*) = func; \
                qsort_safe((p), (n), परिमाण((p)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_r_safe(शून्य *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, शून्य *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((p)[0])*, ध्रुव typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), परिमाण((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

पूर्णाङ्क cmp_int(ध्रुव पूर्णाङ्क *a, ध्रुव पूर्णाङ्क *b);
पूर्णाङ्क cmp_uint16(ध्रुव uint16_t *a, ध्रुव uint16_t *b);
पूर्णाङ्क cmp_unsigned(ध्रुव unsigned *a, ध्रुव unsigned *b);
