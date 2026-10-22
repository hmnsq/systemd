/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

શૂન્ય *xbsearch_r(અચલ શૂન્ય *ચાવી, અચલ શૂન્ય *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, શૂન્ય *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                અચલ typeof((b)[0]) *_k = k;                           \
                પૂર્ણાંક (*_func_)(અચલ typeof((b)[0])*, અચલ typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((અચલ શૂન્ય*) _k, (b), (n), કદ((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

શૂન્ય* bsearch_safe_internal(અચલ શૂન્ય *ચાવી, અચલ શૂન્ય *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(ચાવી, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(ચાવી, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                અચલ typeof((b)[0]) *_k = k;                           \
                પૂર્ણાંક (*_func_)(અચલ typeof((b)[0])*, અચલ typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((અચલ શૂન્ય*) _k, (b), (n), કદ((b)[0]), (comparison_fn_t) _func_); \
        })

શૂન્ય qsort_safe(શૂન્ય *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                પૂર્ણાંક (*_func_)(અચલ typeof((p)[0])*, અચલ typeof((p)[0])*) = func; \
                qsort_safe((p), (n), કદ((p)[0]), (comparison_fn_t) _func_); \
        })

શૂન્ય qsort_r_safe(શૂન્ય *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, શૂન્ય *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                પૂર્ણાંક (*_func_)(અચલ typeof((p)[0])*, અચલ typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), કદ((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

પૂર્ણાંક cmp_int(અચલ પૂર્ણાંક *a, અચલ પૂર્ણાંક *b);
પૂર્ણાંક cmp_uint16(અચલ uint16_t *a, અચલ uint16_t *b);
પૂર્ણાંક cmp_unsigned(અચલ unsigned *a, અચલ unsigned *b);
