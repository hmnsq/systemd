/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

ശൂന്യം *xbsearch_r(സ്ഥിരം ശൂന്യം *താക്കോൽ, സ്ഥിരം ശൂന്യം *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, ശൂന്യം *arg);

#define typesafe_bsearch_r(k, b, n, func, userdata)                     \
        ({                                                              \
                സ്ഥിരം typeof((b)[0]) *_k = k;                           \
                പൂർണ്ണസംഖ്യ (*_func_)(സ്ഥിരം typeof((b)[0])*, സ്ഥിരം typeof((b)[0])*, typeof(userdata)) = func; \
                (typeof((b)[0])*) xbsearch_r((സ്ഥിരം ശൂന്യം*) _k, (b), (n), വലുപ്പം((b)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

ശൂന്യം* bsearch_safe_internal(സ്ഥിരം ശൂന്യം *താക്കോൽ, സ്ഥിരം ശൂന്യം *base, size_t nmemb, size_t size, comparison_fn_t compar);
#define bsearch_safe(താക്കോൽ, base, nmemb, size, compar) \
        const_generic((base), bsearch_safe_internal(താക്കോൽ, base, nmemb, size, compar))

#define typesafe_bsearch(k, b, n, func)                                 \
        ({                                                              \
                സ്ഥിരം typeof((b)[0]) *_k = k;                           \
                പൂർണ്ണസംഖ്യ (*_func_)(സ്ഥിരം typeof((b)[0])*, സ്ഥിരം typeof((b)[0])*) = func; \
                (typeof((b)[0])*) bsearch_safe((സ്ഥിരം ശൂന്യം*) _k, (b), (n), വലുപ്പം((b)[0]), (comparison_fn_t) _func_); \
        })

ശൂന്യം qsort_safe(ശൂന്യം *base, size_t nmemb, size_t size, comparison_fn_t compar);

/* A wrapper around the above, but that adds typesafety: the element size is automatically derived from the type and so
 * is the prototype for the comparison function */
#define typesafe_qsort(p, n, func)                                      \
        ({                                                              \
                പൂർണ്ണസംഖ്യ (*_func_)(സ്ഥിരം typeof((p)[0])*, സ്ഥിരം typeof((p)[0])*) = func; \
                qsort_safe((p), (n), വലുപ്പം((p)[0]), (comparison_fn_t) _func_); \
        })

ശൂന്യം qsort_r_safe(ശൂന്യം *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, ശൂന്യം *userdata);

#define typesafe_qsort_r(p, n, func, userdata)                          \
        ({                                                              \
                പൂർണ്ണസംഖ്യ (*_func_)(സ്ഥിരം typeof((p)[0])*, സ്ഥിരം typeof((p)[0])*, typeof(userdata)) = func; \
                qsort_r_safe((p), (n), വലുപ്പം((p)[0]), (comparison_userdata_fn_t) _func_, userdata); \
        })

പൂർണ്ണസംഖ്യ cmp_int(സ്ഥിരം പൂർണ്ണസംഖ്യ *a, സ്ഥിരം പൂർണ്ണസംഖ്യ *b);
പൂർണ്ണസംഖ്യ cmp_uint16(സ്ഥിരം uint16_t *a, സ്ഥിരം uint16_t *b);
പൂർണ്ണസംഖ്യ cmp_unsigned(സ്ഥിരം unsigned *a, സ്ഥിരം unsigned *b);
