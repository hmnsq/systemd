/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/uio.h>                /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

#അന്തര്ഭാവ "../fundamental/iovec-util.h" /* IWYU pragma: export */

ബാഹ്യ സ്ഥിരം ഘടന iovec iovec_nul_byte; /* Points to a single NUL byte */
ബാഹ്യ സ്ഥിരം ഘടന iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

പൂർണ്ണസംഖ്യ iovec_alloc(size_t n, ഘടന iovec *ret);

size_t iovec_total_size(സ്ഥിരം ഘടന iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(ഘടന iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

ഘടന iovec* iovec_make_string(ഘടന iovec *iovec, സ്ഥിരം അക്ഷരം *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(ഘടന iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (സ്ഥിരം ഘടന iovec) {                  \
                .iov_base = (അക്ഷരം*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (സ്ഥിരം ഘടന iovec) {                                  \
                .iov_base = (അക്ഷരം*) ((സ്ഥിരം അക്ഷരം[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

ശൂന്യം iovec_erase(ഘടന iovec *iovec);
ശൂന്യം iovec_done_erase(ഘടന iovec *iovec);

അക്ഷരം* set_iovec_string_field(ഘടന iovec *iovec, size_t *n_iovec, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *മൂല്യം);
അക്ഷരം* set_iovec_string_field_free(ഘടന iovec *iovec, size_t *n_iovec, സ്ഥിരം അക്ഷരം *field, അക്ഷരം *മൂല്യം);

ശൂന്യം iovec_array_free(ഘടന iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

പൂർണ്ണസംഖ്യ iovec_memcmp(സ്ഥിരം ഘടന iovec *a, സ്ഥിരം ഘടന iovec *b) _pure_;
സ്ഥിതം വരിയിൽ bool iovec_equal(സ്ഥിരം ഘടന iovec *a, സ്ഥിരം ഘടന iovec *b) {
        തിരികെ iovec_memcmp(a, b) == 0;
}

ഘടന iovec* iovec_memdup(സ്ഥിരം ഘടന iovec *source, ഘടന iovec *ret);
പൂർണ്ണസംഖ്യ iovec_done_and_memdup(ഘടന iovec *iovec, സ്ഥിരം ഘടന iovec *source);

ഘടന iovec* iovec_append(ഘടന iovec *iovec, സ്ഥിരം ഘടന iovec *append);

ഘടന iovec* iovec_consume(ഘടന iovec *iovec, size_t n);
