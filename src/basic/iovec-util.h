/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/uio.h>                /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

#અન્તર્ભાવ "../fundamental/iovec-util.h" /* IWYU pragma: export */

બાહ્ય અચલ સંરચના iovec iovec_nul_byte; /* Points to a single NUL byte */
બાહ્ય અચલ સંરચના iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

પૂર્ણાંક iovec_alloc(size_t n, સંરચના iovec *ret);

size_t iovec_total_size(અચલ સંરચના iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(સંરચના iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

સંરચના iovec* iovec_make_string(સંરચના iovec *iovec, અચલ અક્ષર *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(સંરચના iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (અચલ સંરચના iovec) {                  \
                .iov_base = (અક્ષર*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (અચલ સંરચના iovec) {                                  \
                .iov_base = (અક્ષર*) ((અચલ અક્ષર[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

શૂન્ય iovec_erase(સંરચના iovec *iovec);
શૂન્ય iovec_done_erase(સંરચના iovec *iovec);

અક્ષર* set_iovec_string_field(સંરચના iovec *iovec, size_t *n_iovec, અચલ અક્ષર *field, અચલ અક્ષર *કિંમત);
અક્ષર* set_iovec_string_field_free(સંરચના iovec *iovec, size_t *n_iovec, અચલ અક્ષર *field, અક્ષર *કિંમત);

શૂન્ય iovec_array_free(સંરચના iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

પૂર્ણાંક iovec_memcmp(અચલ સંરચના iovec *a, અચલ સંરચના iovec *b) _pure_;
સ્થિર અંતઃરેખ bool iovec_equal(અચલ સંરચના iovec *a, અચલ સંરચના iovec *b) {
        પાછા iovec_memcmp(a, b) == 0;
}

સંરચના iovec* iovec_memdup(અચલ સંરચના iovec *source, સંરચના iovec *ret);
પૂર્ણાંક iovec_done_and_memdup(સંરચના iovec *iovec, અચલ સંરચના iovec *source);

સંરચના iovec* iovec_append(સંરચના iovec *iovec, અચલ સંરચના iovec *append);

સંરચના iovec* iovec_consume(સંરચના iovec *iovec, size_t n);
