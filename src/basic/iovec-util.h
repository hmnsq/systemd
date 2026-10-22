/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/uio.h>                /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/iovec-util.h" /* IWYU pragma: export */

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ iovec iovec_nul_byte; /* Points to a single NUL byte */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ iovec iovec_ਖਾਲੀ_ਥਾਵਾਂ;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

ਪੂਰਨ iovec_alloc(size_t n, ਬਣਤਰ iovec *ret);

size_t iovec_ਕੁੱਲ_ਮਾਪ(ਅਚਲ ਬਣਤਰ iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(ਬਣਤਰ iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

ਬਣਤਰ iovec* iovec_make_string(ਬਣਤਰ iovec *iovec, ਅਚਲ ਅੱਖਰ *s);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(ਬਣਤਰ iovec) {}, s)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONST_IOVEC_MAKE_STRING(s)              \
        (ਅਚਲ ਬਣਤਰ iovec) {                  \
                .iov_base = (ਅੱਖਰ*) s,          \
                .iov_len = STRLEN(s),           \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOVEC_MAKE_BYTE(c)                                      \
        (ਅਚਲ ਬਣਤਰ iovec) {                                  \
                .iov_base = (ਅੱਖਰ*) ((ਅਚਲ ਅੱਖਰ[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_ਸੁਰੱਖਿਅਤ(_n_), _n_);   \
        })

ਖਾਲੀ iovec_erase(ਬਣਤਰ iovec *iovec);
ਖਾਲੀ iovec_ਪੂਰਾ_erase(ਬਣਤਰ iovec *iovec);

ਅੱਖਰ* ਸੈੱਟ_iovec_string_ਖੇਤਰ(ਬਣਤਰ iovec *iovec, size_t *n_iovec, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਅੱਖਰ* ਸੈੱਟ_iovec_string_ਖੇਤਰ_free(ਬਣਤਰ iovec *iovec, size_t *n_iovec, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅੱਖਰ *ਮੁੱਲ);

ਖਾਲੀ iovec_array_free(ਬਣਤਰ iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

ਪੂਰਨ iovec_memcmp(ਅਚਲ ਬਣਤਰ iovec *a, ਅਚਲ ਬਣਤਰ iovec *b) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool iovec_equal(ਅਚਲ ਬਣਤਰ iovec *a, ਅਚਲ ਬਣਤਰ iovec *b) {
        ਵਾਪਸ iovec_memcmp(a, b) == 0;
}

ਬਣਤਰ iovec* iovec_memdup(ਅਚਲ ਬਣਤਰ iovec *ਸਰੋਤ, ਬਣਤਰ iovec *ret);
ਪੂਰਨ iovec_ਪੂਰਾ_and_memdup(ਬਣਤਰ iovec *iovec, ਅਚਲ ਬਣਤਰ iovec *ਸਰੋਤ);

ਬਣਤਰ iovec* iovec_append(ਬਣਤਰ iovec *iovec, ਅਚਲ ਬਣਤਰ iovec *append);

ਬਣਤਰ iovec* iovec_consume(ਬਣਤਰ iovec *iovec, size_t n);
