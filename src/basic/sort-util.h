/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

शून्य *xbsearch_r(ध्रुव शून्य *कुञ्चिका, ध्रुव शून्य *base, size_t nmemb, size_t आकार,
                 comparison_userdata_fn_t compar, शून्य *arg);

#परिभाषय typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ध्रुव typeof((b)[0]) *_k = k;                           \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((b)[0])*, ध्रुव typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ध्रुव शून्य*) _k, (b), (n), परिमाण((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

शून्य* bsearch_safe_आन्तरिकम्(ध्रुव शून्य *कुञ्चिका, ध्रुव शून्य *base, size_t nmemb, size_t आकार, comparison_fn_t compar);
#परिभाषय bsearch_safe(कुञ्चिका, base, nmemb, आकार, compar) \
        const_generic((base), bsearch_safe_आन्तरिकम्(कुञ्चिका, base, nmemb, आकार, compar))

#परिभाषय typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ध्रुव typeof((b)[0]) *_k = k;                           \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((b)[0])*, ध्रुव typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((ध्रुव शून्य*) _k, (b), (n), परिमाण((b)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_safe(शून्य *base, size_t nmemb, size_t आकार, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#परिभाषय typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((p)[0])*, ध्रुव typeof((p)[0])*) = func; \
                qsort_safe((p), (n), परिमाण((p)[0]), (comparison_fn_t) _func_); \
        })

शून्य qsort_r_safe(शून्य *base, size_t nmemb, size_t आकार, comparison_userdata_fn_t compar, शून्य *userdata);

#परिभाषय typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((p)[0])*, ध्रुव typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), परिमाण((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

पूर्णाङ्क cmp_int(ध्रुव पूर्णाङ्क *a, ध्रुव पूर्णाङ्क *b);
पूर्णाङ्क cmp_uint16(ध्रुव uint16_t *a, ध्रुव uint16_t *b);
पूर्णाङ्क cmp_unsigned(ध्रुव अचिह्नित *a, ध्रुव अचिह्नित *b);
