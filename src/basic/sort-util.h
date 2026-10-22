/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਖਾਲੀ *xbsearch_r(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ,
                 comparison_userdata_fn_t compar, ਖਾਲੀ *arg);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                ਅਚਲ typeof((b)[0]) *_k = k;                           \
                ਪੂਰਨ (*_func_)(ਅਚਲ typeof((b)[0])*, ਅਚਲ typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((ਅਚਲ ਖਾਲੀ*) _k, (b), (n), ਆਕਾਰ((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ਖਾਲੀ* bsearch_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ, comparison_fn_t compar);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bsearch_ਸੁਰੱਖਿਅਤ(ਕੁੰਜੀ, base, nmemb, ਮਾਪ, compar) \
        const_generic((base), bsearch_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(ਕੁੰਜੀ, base, nmemb, ਮਾਪ, compar))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                ਅਚਲ typeof((b)[0]) *_k = k;                           \
                ਪੂਰਨ (*_func_)(ਅਚਲ typeof((b)[0])*, ਅਚਲ typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_ਸੁਰੱਖਿਅਤ((ਅਚਲ ਖਾਲੀ*) _k, (b), (n), ਆਕਾਰ((b)[0]), (comparison_fn_t) _func_); \
        })

ਖਾਲੀ qsort_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                ਪੂਰਨ (*_func_)(ਅਚਲ typeof((p)[0])*, ਅਚਲ typeof((p)[0])*) = func; \
                qsort_ਸੁਰੱਖਿਅਤ((p), (n), ਆਕਾਰ((p)[0]), (comparison_fn_t) _func_); \
        })

ਖਾਲੀ qsort_r_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ *base, size_t nmemb, size_t ਮਾਪ, comparison_userdata_fn_t compar, ਖਾਲੀ *userdata);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                ਪੂਰਨ (*_func_)(ਅਚਲ typeof((p)[0])*, ਅਚਲ typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_ਸੁਰੱਖਿਅਤ((p), (n), ਆਕਾਰ((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ਪੂਰਨ cmp_int(ਅਚਲ ਪੂਰਨ *a, ਅਚਲ ਪੂਰਨ *b);
ਪੂਰਨ cmp_uint16(ਅਚਲ uint16_t *a, ਅਚਲ uint16_t *b);
ਪੂਰਨ cmp_unsigned(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ *a, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ *b);
