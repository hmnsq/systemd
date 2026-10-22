/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/uio.h>                /* IWYU pragma: export */

#شامل "forward.h"

#شامل "../fundamental/iovec-util.h" /* IWYU pragma: export */

بیرونی ثابت ساختار iovec iovec_nul_byte; /* Points to a single NUL byte */
بیرونی ثابت ساختار iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

صحیح iovec_alloc(size_t n, ساختار iovec *ret);

size_t iovec_total_size(ثابت ساختار iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(ساختار iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

ساختار iovec* iovec_make_string(ساختار iovec *iovec, ثابت نویسه *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(ساختار iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (ثابت ساختار iovec) {                  \
                .iov_base = (نویسه*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (ثابت ساختار iovec) {                                  \
                .iov_base = (نویسه*) ((ثابت نویسه[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

تهی iovec_erase(ساختار iovec *iovec);
تهی iovec_done_erase(ساختار iovec *iovec);

نویسه* set_iovec_string_field(ساختار iovec *iovec, size_t *n_iovec, ثابت نویسه *field, ثابت نویسه *مقدار);
نویسه* set_iovec_string_field_free(ساختار iovec *iovec, size_t *n_iovec, ثابت نویسه *field, نویسه *مقدار);

تهی iovec_array_free(ساختار iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

صحیح iovec_memcmp(ثابت ساختار iovec *a, ثابت ساختار iovec *b) _pure_;
ایستا درونخطی bool iovec_equal(ثابت ساختار iovec *a, ثابت ساختار iovec *b) {
        بازگشت iovec_memcmp(a, b) == 0;
}

ساختار iovec* iovec_memdup(ثابت ساختار iovec *source, ساختار iovec *ret);
صحیح iovec_done_and_memdup(ساختار iovec *iovec, ثابت ساختار iovec *source);

ساختار iovec* iovec_append(ساختار iovec *iovec, ثابت ساختار iovec *append);

ساختار iovec* iovec_consume(ساختار iovec *iovec, size_t n);
