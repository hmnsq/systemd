/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/uio.h>                /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/iovec-util.h" /* IWYU pragma: export */

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ iovec iovec_nul_byte; /* Points to a single NUL byte */
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

ಪೂರ್ಣಾಙ್ಕ iovec_alloc(size_t n, ಸಂರಚನಾ iovec *ret);

size_t iovec_total_size(ಧ್ರುವ ಸಂರಚನಾ iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(ಸಂರಚನಾ iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

ಸಂರಚನಾ iovec* iovec_make_string(ಸಂರಚನಾ iovec *iovec, ಧ್ರುವ ವರ್ಣ *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(ಸಂರಚನಾ iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (ಧ್ರುವ ಸಂರಚನಾ iovec) {                  \
                .iov_base = (ವರ್ಣ*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (ಧ್ರುವ ಸಂರಚನಾ iovec) {                                  \
                .iov_base = (ವರ್ಣ*) ((ಧ್ರುವ ವರ್ಣ[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

ಶೂನ್ಯ iovec_erase(ಸಂರಚನಾ iovec *iovec);
ಶೂನ್ಯ iovec_done_erase(ಸಂರಚನಾ iovec *iovec);

ವರ್ಣ* set_iovec_string_field(ಸಂರಚನಾ iovec *iovec, size_t *n_iovec, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ವರ್ಣ* set_iovec_string_field_free(ಸಂರಚನಾ iovec *iovec, size_t *n_iovec, ಧ್ರುವ ವರ್ಣ *field, ವರ್ಣ *ಮೌಲ್ಯ);

ಶೂನ್ಯ iovec_array_free(ಸಂರಚನಾ iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

ಪೂರ್ಣಾಙ್ಕ iovec_memcmp(ಧ್ರುವ ಸಂರಚನಾ iovec *a, ಧ್ರುವ ಸಂರಚನಾ iovec *b) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool iovec_equal(ಧ್ರುವ ಸಂರಚನಾ iovec *a, ಧ್ರುವ ಸಂರಚನಾ iovec *b) {
        ಪ್ರತ್ಯಾಗಮ iovec_memcmp(a, b) == 0;
}

ಸಂರಚನಾ iovec* iovec_memdup(ಧ್ರುವ ಸಂರಚನಾ iovec *source, ಸಂರಚನಾ iovec *ret);
ಪೂರ್ಣಾಙ್ಕ iovec_done_and_memdup(ಸಂರಚನಾ iovec *iovec, ಧ್ರುವ ಸಂರಚನಾ iovec *source);

ಸಂರಚನಾ iovec* iovec_append(ಸಂರಚನಾ iovec *iovec, ಧ್ರುವ ಸಂರಚನಾ iovec *append);

ಸಂರಚನಾ iovec* iovec_consume(ಸಂರಚನಾ iovec *iovec, size_t n);
