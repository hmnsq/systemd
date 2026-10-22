/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/uio.h>                /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/iovec-util.h" /* IWYU pragma: export */

बाह्य ध्रुव संरचना iovec iovec_nul_byte; /* Points to a single NUL byte */
बाह्य ध्रुव संरचना iovec iovec_रिक्तानि;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

पूर्णाङ्क iovec_alloc(size_t n, संरचना iovec *ret);

size_t iovec_योग_आकार(ध्रुव संरचना iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(संरचना iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

संरचना iovec* iovec_make_string(संरचना iovec *iovec, ध्रुव वर्ण *s);

#परिभाषय IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(संरचना iovec) {}, s)

#परिभाषय CONST_IOVEC_MAKE_STRING(s)              \
        (ध्रुव संरचना iovec) {                  \
                .iov_base = (वर्ण*) s,          \
                .iov_len = STRLEN(s),           \
        }

#परिभाषय IOVEC_MAKE_BYTE(c)                                      \
        (ध्रुव संरचना iovec) {                                  \
                .iov_base = (वर्ण*) ((ध्रुव वर्ण[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#परिभाषय IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

शून्य iovec_erase(संरचना iovec *iovec);
शून्य iovec_समाप्तम्_erase(संरचना iovec *iovec);

वर्ण* समुच्चयः_iovec_string_क्षेत्रम्(संरचना iovec *iovec, size_t *n_iovec, ध्रुव वर्ण *क्षेत्रम्, ध्रुव वर्ण *मानम्);
वर्ण* समुच्चयः_iovec_string_क्षेत्रम्_free(संरचना iovec *iovec, size_t *n_iovec, ध्रुव वर्ण *क्षेत्रम्, वर्ण *मानम्);

शून्य iovec_array_free(संरचना iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

पूर्णाङ्क iovec_memcmp(ध्रुव संरचना iovec *a, ध्रुव संरचना iovec *b) विशुद्धम्;
स्थिर अन्तःस्थ bool iovec_equal(ध्रुव संरचना iovec *a, ध्रुव संरचना iovec *b) {
        प्रत्यागम iovec_memcmp(a, b) == 0;
}

संरचना iovec* iovec_memdup(ध्रुव संरचना iovec *source, संरचना iovec *ret);
पूर्णाङ्क iovec_समाप्तम्_and_memdup(संरचना iovec *iovec, ध्रुव संरचना iovec *source);

संरचना iovec* iovec_append(संरचना iovec *iovec, ध्रुव संरचना iovec *append);

संरचना iovec* iovec_consume(संरचना iovec *iovec, size_t n);
