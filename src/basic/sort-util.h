/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

فراغ *xbsearch_r(ثابت فراغ *کلید, ثابت فراغ *base, size_t nmemb, size_t ناپ,
                 comparison_userdata_fn_t compar, فراغ *arg);

#تعریف_کرو typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحيح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ثابت فراغ*) _k, (b), (n), حجم((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

فراغ* bsearch_محفوظ_اندرونی(ثابت فراغ *کلید, ثابت فراغ *base, size_t nmemb, size_t ناپ, comparison_fn_t compar);
#تعریف_کرو bsearch_محفوظ(کلید, base, nmemb, ناپ, compar) \
        const_generic((base), bsearch_محفوظ_اندرونی(کلید, base, nmemb, ناپ, compar))

#تعریف_کرو typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ثابت typeof((b)[0]) *_k = k;                           \
                صحيح (*_func_)(ثابت typeof((b)[0])*, ثابت typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_محفوظ((ثابت فراغ*) _k, (b), (n), حجم((b)[0]), (comparison_fn_t) _func_); \
        })

فراغ qsort_محفوظ(فراغ *base, size_t nmemb, size_t ناپ, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#تعریف_کرو typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*) = func; \
                qsort_محفوظ((p), (n), حجم((p)[0]), (comparison_fn_t) _func_); \
        })

فراغ qsort_r_محفوظ(فراغ *base, size_t nmemb, size_t ناپ, comparison_userdata_fn_t compar, فراغ *userdata);

#تعریف_کرو typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((p)[0])*, ثابت typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_محفوظ((p), (n), حجم((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

صحيح cmp_int(ثابت صحيح *a, ثابت صحيح *b);
صحيح cmp_uint16(ثابت uint16_t *a, ثابت uint16_t *b);
صحيح cmp_unsigned(ثابت بنانشانی *a, ثابت بنانشانی *b);
