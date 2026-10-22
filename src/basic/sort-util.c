/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
ശൂന്യം *xbsearch_r(സ്ഥിരം ശൂന്യം *താക്കോൽ, സ്ഥിരം ശൂന്യം *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, ശൂന്യം *arg) {
        size_t l, u, idx;
        സ്ഥിരം ശൂന്യം *p;
        പൂർണ്ണസംഖ്യ comparison;

        പ്രതിജ്ഞാ(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        ആകുമ്പോൾ (l < u) {
                idx = (l + u) / 2;
                p = (സ്ഥിരം uint8_t*) base + idx * size;
                comparison = compar(താക്കോൽ, p, arg);
                എങ്കിൽ (comparison < 0)
                        u = idx;
                അല്ലെങ്കിൽ എങ്കിൽ (comparison > 0)
                        l = idx + 1;
                അല്ലെങ്കിൽ
                        തിരികെ (ശൂന്യം *)p;
        }
        തിരികെ ശൂന്യ;
}

ശൂന്യം* bsearch_safe_internal(സ്ഥിരം ശൂന്യം *താക്കോൽ, സ്ഥിരം ശൂന്യം *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        എങ്കിൽ (nmemb <= 0)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(base);
        തിരികെ (ശൂന്യം*) bsearch(താക്കോൽ, base, nmemb, size, compar);
}

ശൂന്യം qsort_safe(ശൂന്യം *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        എങ്കിൽ (nmemb <= 1)
                തിരികെ;

        പ്രതിജ്ഞാ(base);
        qsort(base, nmemb, size, compar);
}

ശൂന്യം qsort_r_safe(ശൂന്യം *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, ശൂന്യം *userdata) {
        എങ്കിൽ (nmemb <= 1)
                തിരികെ;

        പ്രതിജ്ഞാ(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

പൂർണ്ണസംഖ്യ cmp_int(സ്ഥിരം പൂർണ്ണസംഖ്യ *a, സ്ഥിരം പൂർണ്ണസംഖ്യ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        തിരികെ CMP(*a, *b);
}

പൂർണ്ണസംഖ്യ cmp_uint16(സ്ഥിരം uint16_t *a, സ്ഥിരം uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        തിരികെ CMP(*a, *b);
}

പൂർണ്ണസംഖ്യ cmp_unsigned(സ്ഥിരം unsigned *a, സ്ഥിരം unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        തിരികെ CMP(*a, *b);
}
