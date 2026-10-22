/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/uio.h>                /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/iovec-util.h" /* IWYU pragma: export */

বাহ্য ধ্রুব সংরচনা iovec iovec_nul_byte; /* Points to a single NUL byte */
বাহ্য ধ্রুব সংরচনা iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

পূর্ণাঙ্ক iovec_alloc(size_t n, সংরচনা iovec *ret);

size_t iovec_total_size(ধ্রুব সংরচনা iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(সংরচনা iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

সংরচনা iovec* iovec_make_string(সংরচনা iovec *iovec, ধ্রুব বর্ণ *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(সংরচনা iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (ধ্রুব সংরচনা iovec) {                  \
                .iov_base = (বর্ণ*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (ধ্রুব সংরচনা iovec) {                                  \
                .iov_base = (বর্ণ*) ((ধ্রুব বর্ণ[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

শূন্য iovec_erase(সংরচনা iovec *iovec);
শূন্য iovec_done_erase(সংরচনা iovec *iovec);

বর্ণ* set_iovec_string_field(সংরচনা iovec *iovec, size_t *n_iovec, ধ্রুব বর্ণ *field, ধ্রুব বর্ণ *মান);
বর্ণ* set_iovec_string_field_free(সংরচনা iovec *iovec, size_t *n_iovec, ধ্রুব বর্ণ *field, বর্ণ *মান);

শূন্য iovec_array_free(সংরচনা iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

পূর্ণাঙ্ক iovec_memcmp(ধ্রুব সংরচনা iovec *a, ধ্রুব সংরচনা iovec *b) _pure_;
স্থির অন্তঃস্থ bool iovec_equal(ধ্রুব সংরচনা iovec *a, ধ্রুব সংরচনা iovec *b) {
        প্রত্যাগম iovec_memcmp(a, b) == 0;
}

সংরচনা iovec* iovec_memdup(ধ্রুব সংরচনা iovec *source, সংরচনা iovec *ret);
পূর্ণাঙ্ক iovec_done_and_memdup(সংরচনা iovec *iovec, ধ্রুব সংরচনা iovec *source);

সংরচনা iovec* iovec_append(সংরচনা iovec *iovec, ধ্রুব সংরচনা iovec *append);

সংরচনা iovec* iovec_consume(সংরচনা iovec *iovec, size_t n);
