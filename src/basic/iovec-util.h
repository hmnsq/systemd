/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/uio.h>                /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/iovec-util.h" /* IWYU pragma: export */

বাহ্যিক ধ্রুবক কাঠামো iovec iovec_nul_byte; /* Points to a single NUL byte */
বাহ্যিক ধ্রুবক কাঠামো iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

পূর্ণসংখ্যা iovec_alloc(size_t n, কাঠামো iovec *ret);

size_t iovec_যোগ_আকার(ধ্রুবক কাঠামো iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(কাঠামো iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

কাঠামো iovec* iovec_make_string(কাঠামো iovec *iovec, ধ্রুবক অক্ষর *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(কাঠামো iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (ধ্রুবক কাঠামো iovec) {                  \
                .iov_base = (অক্ষর*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (ধ্রুবক কাঠামো iovec) {                                  \
                .iov_base = (অক্ষর*) ((ধ্রুবক অক্ষর[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

শূন্য iovec_erase(কাঠামো iovec *iovec);
শূন্য iovec_done_erase(কাঠামো iovec *iovec);

অক্ষর* set_iovec_string_field(কাঠামো iovec *iovec, size_t *n_iovec, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *মান);
অক্ষর* set_iovec_string_field_free(কাঠামো iovec *iovec, size_t *n_iovec, ধ্রুবক অক্ষর *field, অক্ষর *মান);

শূন্য iovec_array_free(কাঠামো iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

পূর্ণসংখ্যা iovec_memcmp(ধ্রুবক কাঠামো iovec *a, ধ্রুবক কাঠামো iovec *b) _pure_;
স্থির অন্তঃরেখ bool iovec_equal(ধ্রুবক কাঠামো iovec *a, ধ্রুবক কাঠামো iovec *b) {
        ফেরত iovec_memcmp(a, b) == 0;
}

কাঠামো iovec* iovec_memdup(ধ্রুবক কাঠামো iovec *source, কাঠামো iovec *ret);
পূর্ণসংখ্যা iovec_done_and_memdup(কাঠামো iovec *iovec, ধ্রুবক কাঠামো iovec *source);

কাঠামো iovec* iovec_append(কাঠামো iovec *iovec, ধ্রুবক কাঠামো iovec *append);

কাঠামো iovec* iovec_consume(কাঠামো iovec *iovec, size_t n);
