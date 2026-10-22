/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

فراغ *xbsearch_r(ثابت فراغ *کلی, ثابت فراغ *base, size_t nmemb, size_t اندازه,
                 comparison_userdata_fn_t compar, فراغ *arg);

#تعریف_کړه typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحيح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ثابت فراغ*) _k, (b), (n), حجم((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

فراغ* bsearch_خوندي_دننی(ثابت فراغ *کلی, ثابت فراغ *base, size_t nmemb, size_t اندازه, comparison_fn_t compar);
#تعریف_کړه bsearch_خوندي(کلی, base, nmemb, اندازه, compar) \
        const_generic((base), bsearch_خوندي_دننی(کلی, base, nmemb, اندازه, compar))

#تعریف_کړه typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحيح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_خوندي((ثابت فراغ*) _k, (b), (n), حجم((b)[0]), (comparison_fn_t) _func_); \
        })

فراغ qsort_خوندي(فراغ *base, size_t nmemb, size_t اندازه, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#تعریف_کړه typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*) = func; \
                qsort_خوندي((p), (n), حجم((p)[0]), (comparison_fn_t) _func_); \
        })

فراغ qsort_r_خوندي(فراغ *base, size_t nmemb, size_t اندازه, comparison_userdata_fn_t compar, فراغ *userdata);

#تعریف_کړه typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_خوندي((p), (n), حجم((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

صحيح cmp_int(ثابت صحيح *a, ثابت صحيح *b);
صحيح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b);
صحيح cmp_unsigned(ثابت بینښه *a, ثابت بینښه *b);
