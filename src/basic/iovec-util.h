/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/uio.h>                /* IWYU pragma: export */

#include "forward.h"

#include "../fundamental/iovec-util.h" /* IWYU pragma: export */

extern const sa_nracanaa iovec iovec_nul_byte; /* Points to a single NUL byte */
extern const sa_nracanaa iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

puur_naa_nka iovec_alloc(size_t n, sa_nracanaa iovec *ret);

size_t iovec_total_size(const sa_nracanaa iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(sa_nracanaa iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

sa_nracanaa iovec* iovec_make_string(sa_nracanaa iovec *iovec, const var_na *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(sa_nracanaa iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (const sa_nracanaa iovec) {                  \
                .iov_base = (var_na*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (const sa_nracanaa iovec) {                                  \
                .iov_base = (var_na*) ((const var_na[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

void iovec_erase(sa_nracanaa iovec *iovec);
void iovec_done_erase(sa_nracanaa iovec *iovec);

var_na* set_iovec_string_field(sa_nracanaa iovec *iovec, size_t *n_iovec, const var_na *field, const var_na *value);
var_na* set_iovec_string_field_free(sa_nracanaa iovec *iovec, size_t *n_iovec, const var_na *field, var_na *value);

void iovec_array_free(sa_nracanaa iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

puur_naa_nka iovec_memcmp(const sa_nracanaa iovec *a, const sa_nracanaa iovec *b) _pure_;
static anta_hstha bool iovec_equal(const sa_nracanaa iovec *a, const sa_nracanaa iovec *b) {
        ᬯᬮᬶ iovec_memcmp(a, b) == 0;
}

sa_nracanaa iovec* iovec_memdup(const sa_nracanaa iovec *source, sa_nracanaa iovec *ret);
puur_naa_nka iovec_done_and_memdup(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *source);

sa_nracanaa iovec* iovec_append(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *append);

sa_nracanaa iovec* iovec_consume(sa_nracanaa iovec *iovec, size_t n);
