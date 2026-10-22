/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/uio.h>                /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/iovec-util.h" /* IWYU pragma: export */

बाह्य अचर संरचना iovec iovec_nul_byte; /* Points to a single NUL byte */
बाह्य अचर संरचना iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

पूर्णाङ्क iovec_alloc(माप_प्रकार n, संरचना iovec *ret);

माप_प्रकार iovec_total_size(अचर संरचना iovec *iovec, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(संरचना iovec *iovec, माप_प्रकार n, माप_प्रकार k) _nonnull_if_nonzero_(1, 2);

संरचना iovec* iovec_make_string(संरचना iovec *iovec, अचर वर्ण *s);

#define IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(संरचना iovec) {}, s)

#define CONST_IOVEC_MAKE_STRING(s)              \
        (अचर संरचना iovec) {                  \
                .iov_base = (वर्ण*) s,          \
                .iov_len = STRLEN(s),           \
        }

#define IOVEC_MAKE_BYTE(c)                                      \
        (अचर संरचना iovec) {                                  \
                .iov_base = (वर्ण*) ((अचर वर्ण[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#define IOVEC_ALLOCA(n)                              \
        ({                                           \
                माप_प्रकार _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

शून्य iovec_erase(संरचना iovec *iovec);
शून्य iovec_done_erase(संरचना iovec *iovec);

वर्ण* set_iovec_string_field(संरचना iovec *iovec, माप_प्रकार *n_iovec, अचर वर्ण *field, अचर वर्ण *मान);
वर्ण* set_iovec_string_field_free(संरचना iovec *iovec, माप_प्रकार *n_iovec, अचर वर्ण *field, वर्ण *मान);

शून्य iovec_array_free(संरचना iovec *iovec, माप_प्रकार n_iovec) _nonnull_if_nonzero_(1, 2);

पूर्णाङ्क iovec_memcmp(अचर संरचना iovec *a, अचर संरचना iovec *b) _pure_;
स्थिर अन्तःस्थ bool iovec_equal(अचर संरचना iovec *a, अचर संरचना iovec *b) {
        फर्काऊ iovec_memcmp(a, b) == 0;
}

संरचना iovec* iovec_memdup(अचर संरचना iovec *source, संरचना iovec *ret);
पूर्णाङ्क iovec_done_and_memdup(संरचना iovec *iovec, अचर संरचना iovec *source);

संरचना iovec* iovec_append(संरचना iovec *iovec, अचर संरचना iovec *append);

संरचना iovec* iovec_consume(संरचना iovec *iovec, माप_प्रकार n);
