/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/uio.h>                /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

#அந்தர்பாவ "../fundamental/iovec-util.h" /* IWYU pragma: export */

வெளி மாறிலி கட்டமைப்பு iovec iovec_nul_byte; /* Points to a single NUL byte */
வெளி மாறிலி கட்டமைப்பு iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

முழு iovec_alloc(size_t n, கட்டமைப்பு iovec *ret);

size_t iovec_total_size(மாறிலி கட்டமைப்பு iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(கட்டமைப்பு iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

கட்டமைப்பு iovec* iovec_make_string(கட்டமைப்பு iovec *iovec, மாறிலி எழுத்து *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(கட்டமைப்பு iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (மாறிலி கட்டமைப்பு iovec) {                  \
                .iov_base = (எழுத்து*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (மாறிலி கட்டமைப்பு iovec) {                                  \
                .iov_base = (எழுத்து*) ((மாறிலி எழுத்து[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

வெறுமை iovec_erase(கட்டமைப்பு iovec *iovec);
வெறுமை iovec_done_erase(கட்டமைப்பு iovec *iovec);

எழுத்து* set_iovec_string_field(கட்டமைப்பு iovec *iovec, size_t *n_iovec, மாறிலி எழுத்து *field, மாறிலி எழுத்து *மதிப்பு);
எழுத்து* set_iovec_string_field_free(கட்டமைப்பு iovec *iovec, size_t *n_iovec, மாறிலி எழுத்து *field, எழுத்து *மதிப்பு);

வெறுமை iovec_array_free(கட்டமைப்பு iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

முழு iovec_memcmp(மாறிலி கட்டமைப்பு iovec *a, மாறிலி கட்டமைப்பு iovec *b) _pure_;
நிலையான உள்வரி bool iovec_equal(மாறிலி கட்டமைப்பு iovec *a, மாறிலி கட்டமைப்பு iovec *b) {
        திரும்பு iovec_memcmp(a, b) == 0;
}

கட்டமைப்பு iovec* iovec_memdup(மாறிலி கட்டமைப்பு iovec *source, கட்டமைப்பு iovec *ret);
முழு iovec_done_and_memdup(கட்டமைப்பு iovec *iovec, மாறிலி கட்டமைப்பு iovec *source);

கட்டமைப்பு iovec* iovec_append(கட்டமைப்பு iovec *iovec, மாறிலி கட்டமைப்பு iovec *append);

கட்டமைப்பு iovec* iovec_consume(கட்டமைப்பு iovec *iovec, size_t n);
