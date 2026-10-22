/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>

#include "alloc-util.h"
#include "sort-util.h"

/* hey glibc, APIs with callbacks without a user pointer are so useless */
void *xbsearch_r(const void *key, const void *base, size_t nmemb, size_t size,
                 comparison_userdata_fn_t compar, void *arg) {
        size_t l, u, idx;
        const void *p;
        ကိန်းပြည့် comparison;

        assert(!size_multiply_overflow(nmemb, size));

        l = 0;
        u = nmemb;
        နေစဉ် (l < u) {
                idx = (l + u) / 2;
                p = (const uint8_t*) base + idx * size;
                comparison = compar(key, p, arg);
                အကယ်၍ (comparison < 0)
                        u = idx;
                မဟုတ်ပါက အကယ်၍ (comparison > 0)
                        l = idx + 1;
                မဟုတ်ပါက
                        ပြန်ပေး (void *)p;
        }
        ပြန်ပေး NULL;
}

void* bsearch_safe_internal(const void *key, const void *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
        * Normal bsearch requires base to be nonnull. Here were require
        * that only if nmemb > 0.
        */

        အကယ်၍ (nmemb <= 0)
                ပြန်ပေး NULL;

        assert(base);
        ပြန်ပေး (void*) bsearch(key, base, nmemb, size, compar);
}

void qsort_safe(void *base, size_t nmemb, size_t size, comparison_fn_t compar) {
        /**
         * Normal qsort requires base to be nonnull. Here were require
         * that only if nmemb > 0.
         */

        အကယ်၍ (nmemb <= 1)
                ပြန်ပေး;

        assert(base);
        qsort(base, nmemb, size, compar);
}

void qsort_r_safe(void *base, size_t nmemb, size_t size, comparison_userdata_fn_t compar, void *userdata) {
        အကယ်၍ (nmemb <= 1)
                ပြန်ပေး;

        assert(base);
        qsort_r(base, nmemb, size, compar, userdata);
}

ကိန်းပြည့် cmp_int(const ကိန်းပြည့် *a, const ကိန်းပြည့် *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ပြန်ပေး CMP(*a, *b);
}

ကိန်းပြည့် cmp_uint16(const uint16_t *a, const uint16_t *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ပြန်ပေး CMP(*a, *b);
}

ကိန်းပြည့် cmp_unsigned(const unsigned *a, const unsigned *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ပြန်ပေး CMP(*a, *b);
}
