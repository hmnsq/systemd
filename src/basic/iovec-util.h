/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/uio.h>                /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/iovec-util.h" /* IWYU pragma: export */

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ iovec iovec_nul_byte; /* Points to a single NUL byte */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ iovec iovec_ಖಾಲಿಗಳು;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

ಪೂರ್ಣಾಂಕ iovec_alloc(size_t n, ರಚನೆ iovec *ret);

size_t iovec_ಒಟ್ಟು_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(ರಚನೆ iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

ರಚನೆ iovec* iovec_make_string(ರಚನೆ iovec *iovec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);

#ವ್ಯಾಖ್ಯಾನಿಸು IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(ರಚನೆ iovec) {}, s)

#ವ್ಯಾಖ್ಯಾನಿಸು CONST_IOVEC_MAKE_STRING(s)              \
        (ಸ್ಥಿರಾಂಕ ರಚನೆ iovec) {                  \
                .iov_base = (ಅಕ್ಷರ*) s,          \
                .iov_len = STRLEN(s),           \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು IOVEC_MAKE_BYTE(c)                                      \
        (ಸ್ಥಿರಾಂಕ ರಚನೆ iovec) {                                  \
                .iov_base = (ಅಕ್ಷರ*) ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_ಸುರಕ್ಷಿತ(_n_), _n_);   \
        })

ಶೂನ್ಯ iovec_erase(ರಚನೆ iovec *iovec);
ಶೂನ್ಯ iovec_ಮುಗಿದ_erase(ರಚನೆ iovec *iovec);

ಅಕ್ಷರ* ಗಣ_iovec_string_ಕ್ಷೇತ್ರ(ರಚನೆ iovec *iovec, size_t *n_iovec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಅಕ್ಷರ* ಗಣ_iovec_string_ಕ್ಷೇತ್ರ_free(ರಚನೆ iovec *iovec, size_t *n_iovec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಅಕ್ಷರ *ಮೌಲ್ಯ);

ಶೂನ್ಯ iovec_array_free(ರಚನೆ iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

ಪೂರ್ಣಾಂಕ iovec_memcmp(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *a, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *b) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool iovec_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *a, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *b) {
        ಹಿಂತಿರುಗಿ iovec_memcmp(a, b) == 0;
}

ರಚನೆ iovec* iovec_memdup(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *ಮೂಲ, ರಚನೆ iovec *ret);
ಪೂರ್ಣಾಂಕ iovec_ಮುಗಿದ_and_memdup(ರಚನೆ iovec *iovec, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *ಮೂಲ);

ರಚನೆ iovec* iovec_append(ರಚನೆ iovec *iovec, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *append);

ರಚನೆ iovec* iovec_consume(ರಚನೆ iovec *iovec, size_t n);
