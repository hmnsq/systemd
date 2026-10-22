/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/uio.h>                /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

#ଅନ୍ତର୍ଭାଵ "../fundamental/iovec-util.h" /* IWYU pragma: export */

ବାହ୍ୟ ଅଚଳ ସଂରଚନା iovec iovec_nul_byte; /* Points to a single NUL byte */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_alloc(size_t n, ସଂରଚନା iovec *ret);

size_t iovec_total_size(ଅଚଳ ସଂରଚନା iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(ସଂରଚନା iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

ସଂରଚନା iovec* iovec_make_string(ସଂରଚନା iovec *iovec, ଅଚଳ ଅକ୍ଷର *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(ସଂରଚନା iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (ଅଚଳ ସଂରଚନା iovec) {                  \
                .iov_base = (ଅକ୍ଷର*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (ଅଚଳ ସଂରଚନା iovec) {                                  \
                .iov_base = (ଅକ୍ଷର*) ((ଅଚଳ ଅକ୍ଷର[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

ଶୂନ୍ୟ iovec_erase(ସଂରଚନା iovec *iovec);
ଶୂନ୍ୟ iovec_done_erase(ସଂରଚନା iovec *iovec);

ଅକ୍ଷର* set_iovec_string_field(ସଂରଚନା iovec *iovec, size_t *n_iovec, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ଅକ୍ଷର* set_iovec_string_field_free(ସଂରଚନା iovec *iovec, size_t *n_iovec, ଅଚଳ ଅକ୍ଷର *field, ଅକ୍ଷର *ମୂଲ୍ୟ);

ଶୂନ୍ୟ iovec_array_free(ସଂରଚନା iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_memcmp(ଅଚଳ ସଂରଚନା iovec *a, ଅଚଳ ସଂରଚନା iovec *b) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool iovec_equal(ଅଚଳ ସଂରଚନା iovec *a, ଅଚଳ ସଂରଚନା iovec *b) {
        ଫେରନ୍ତୁ iovec_memcmp(a, b) == 0;
}

ସଂରଚନା iovec* iovec_memdup(ଅଚଳ ସଂରଚନା iovec *source, ସଂରଚନା iovec *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_done_and_memdup(ସଂରଚନା iovec *iovec, ଅଚଳ ସଂରଚନା iovec *source);

ସଂରଚନା iovec* iovec_append(ସଂରଚନା iovec *iovec, ଅଚଳ ସଂରଚନା iovec *append);

ସଂରଚନା iovec* iovec_consume(ସଂରଚନା iovec *iovec, size_t n);
