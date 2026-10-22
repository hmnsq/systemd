/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <sys/uio.h>                /* IWYU pragma: export */

#অন্তর্ভাৱ "forward.h"

#অন্তর্ভাৱ "../fundamental/iovec-util.h" /* IWYU pragma: export */

বাহ্য ধ্রুৱ সংরচনা iovec iovec_nul_byte; /* Points to a single NUL byte */
বাহ্য ধ্রুৱ সংরচনা iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

পূর্ণাঙ্ক iovec_alloc(size_t n, সংরচনা iovec *ret);

size_t iovec_total_size(ধ্রুৱ সংরচনা iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(সংরচনা iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

সংরচনা iovec* iovec_make_string(সংরচনা iovec *iovec, ধ্রুৱ ৱর্ণ *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(সংরচনা iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (ধ্রুৱ সংরচনা iovec) {                  \
                .iov_base = (ৱর্ণ*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (ধ্রুৱ সংরচনা iovec) {                                  \
                .iov_base = (ৱর্ণ*) ((ধ্রুৱ ৱর্ণ[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

শূন্য iovec_erase(সংরচনা iovec *iovec);
শূন্য iovec_done_erase(সংরচনা iovec *iovec);

ৱর্ণ* set_iovec_string_field(সংরচনা iovec *iovec, size_t *n_iovec, ধ্রুৱ ৱর্ণ *field, ধ্রুৱ ৱর্ণ *মান);
ৱর্ণ* set_iovec_string_field_free(সংরচনা iovec *iovec, size_t *n_iovec, ধ্রুৱ ৱর্ণ *field, ৱর্ণ *মান);

শূন্য iovec_array_free(সংরচনা iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

পূর্ণাঙ্ক iovec_memcmp(ধ্রুৱ সংরচনা iovec *a, ধ্রুৱ সংরচনা iovec *b) _pure_;
স্থির অন্তঃস্থ bool iovec_equal(ধ্রুৱ সংরচনা iovec *a, ধ্রুৱ সংরচনা iovec *b) {
        ঘূৰাই iovec_memcmp(a, b) == 0;
}

সংরচনা iovec* iovec_memdup(ধ্রুৱ সংরচনা iovec *source, সংরচনা iovec *ret);
পূর্ণাঙ্ক iovec_done_and_memdup(সংরচনা iovec *iovec, ধ্রুৱ সংরচনা iovec *source);

সংরচনা iovec* iovec_append(সংরচনা iovec *iovec, ধ্রুৱ সংরচনা iovec *append);

সংরচনা iovec* iovec_consume(সংরচনা iovec *iovec, size_t n);
