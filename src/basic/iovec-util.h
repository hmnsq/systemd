/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/uio.h>                /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

#అన్తర్భావ "../fundamental/iovec-util.h" /* IWYU pragma: export */

బాహ్య స్థిరరాశి నిర్మాణం iovec iovec_nul_byte; /* Points to a single NUL byte */
బాహ్య స్థిరరాశి నిర్మాణం iovec iovec_ఖాళీలు;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

పూర్ణాంకం iovec_alloc(size_t n, నిర్మాణం iovec *ret);

size_t iovec_మొత్తం_కొలత(స్థిరరాశి నిర్మాణం iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(నిర్మాణం iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

నిర్మాణం iovec* iovec_make_string(నిర్మాణం iovec *iovec, స్థిరరాశి అక్షరం *s);

#నిర్వచించు IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(నిర్మాణం iovec) {}, s)

#నిర్వచించు CONST_IOVEC_MAKE_STRING(s)              \
        (స్థిరరాశి నిర్మాణం iovec) {                  \
                .iov_base = (అక్షరం*) s,          \
                .iov_len = STRLEN(s),           \
        }

#నిర్వచించు IOVEC_MAKE_BYTE(c)                                      \
        (స్థిరరాశి నిర్మాణం iovec) {                                  \
                .iov_base = (అక్షరం*) ((స్థిరరాశి అక్షరం[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#నిర్వచించు IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_సురక్షిత(_n_), _n_);   \
        })

శూన్య iovec_erase(నిర్మాణం iovec *iovec);
శూన్య iovec_పూర్తి_erase(నిర్మాణం iovec *iovec);

అక్షరం* సమితి_iovec_string_క్షేత్రం(నిర్మాణం iovec *iovec, size_t *n_iovec, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *విలువ);
అక్షరం* సమితి_iovec_string_క్షేత్రం_free(నిర్మాణం iovec *iovec, size_t *n_iovec, స్థిరరాశి అక్షరం *క్షేత్రం, అక్షరం *విలువ);

శూన్య iovec_array_free(నిర్మాణం iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

పూర్ణాంకం iovec_memcmp(స్థిరరాశి నిర్మాణం iovec *a, స్థిరరాశి నిర్మాణం iovec *b) శుద్ధ;
స్థిర అంతఃవరుస bool iovec_equal(స్థిరరాశి నిర్మాణం iovec *a, స్థిరరాశి నిర్మాణం iovec *b) {
        తిరిగి iovec_memcmp(a, b) == 0;
}

నిర్మాణం iovec* iovec_memdup(స్థిరరాశి నిర్మాణం iovec *ఆధారం, నిర్మాణం iovec *ret);
పూర్ణాంకం iovec_పూర్తి_and_memdup(నిర్మాణం iovec *iovec, స్థిరరాశి నిర్మాణం iovec *ఆధారం);

నిర్మాణం iovec* iovec_append(నిర్మాణం iovec *iovec, స్థిరరాశి నిర్మాణం iovec *append);

నిర్మాణం iovec* iovec_consume(నిర్మాణం iovec *iovec, size_t n);
