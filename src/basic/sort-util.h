/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

خالی *xbsearch_r(ثابت خالی *کلید, ثابت خالی *base, size_t nmemb, size_t سائز,
                 comparison_userdata_fn_t compar, خالی *arg);

#تعریف_کرو typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحیح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ثابت خالی*) _k, (b), (n), حجم((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

خالی* bsearch_محفوظ_اندرونی(ثابت خالی *کلید, ثابت خالی *base, size_t nmemb, size_t سائز, comparison_fn_t compar);
#تعریف_کرو bsearch_محفوظ(کلید, base, nmemb, سائز, compar) \
        const_generic((base), bsearch_محفوظ_اندرونی(کلید, base, nmemb, سائز, compar))

#تعریف_کرو typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحیح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_محفوظ((ثابت خالی*) _k, (b), (n), حجم((b)[0]), (comparison_fn_t) _func_); \
        })

خالی qsort_محفوظ(خالی *base, size_t nmemb, size_t سائز, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#تعریف_کرو typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                صحیح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*) = func; \
                qsort_محفوظ((p), (n), حجم((p)[0]), (comparison_fn_t) _func_); \
        })

خالی qsort_r_محفوظ(خالی *base, size_t nmemb, size_t سائز, comparison_userdata_fn_t compar, خالی *userdata);

#تعریف_کرو typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                صحیح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_محفوظ((p), (n), حجم((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

صحیح cmp_int(ثابت صحیح *a, ثابت صحیح *b);
صحیح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b);
صحیح cmp_unsigned(ثابت غیرعلامتی *a, ثابت غیرعلامتی *b);
