/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/uio.h>                /* IWYU pragma: export */

#समावेश "forward.h"

#समावेश "../fundamental/iovec-util.h" /* IWYU pragma: export */

बाह्य स्थिर काष्ठा iovec iovec_nul_byte; /* Points to a single NUL byte */
बाह्य स्थिर काष्ठा iovec iovec_empty;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

पूर्णांक iovec_alloc(माप_प्रकार n, काष्ठा iovec *ret);

माप_प्रकार iovec_योग_आकार(स्थिर काष्ठा iovec *iovec, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(काष्ठा iovec *iovec, माप_प्रकार n, माप_प्रकार k) _nonnull_if_nonzero_(1, 2);

काष्ठा iovec* iovec_make_string(काष्ठा iovec *iovec, स्थिर अक्षर *s);

#घोषणा IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(काष्ठा iovec) {}, s)

#घोषणा CONST_IOVEC_MAKE_STRING(s)              \
        (स्थिर काष्ठा iovec) {                  \
                .iov_base = (अक्षर*) s,          \
                .iov_len = STRLEN(s),           \
        }

#घोषणा IOVEC_MAKE_BYTE(c)                                      \
        (स्थिर काष्ठा iovec) {                                  \
                .iov_base = (अक्षर*) ((स्थिर अक्षर[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#घोषणा IOVEC_ALLOCA(n)                              \
        ({                                           \
                माप_प्रकार _n_ = (n);                    \
                IOVEC_MAKE(alloca_safe(_n_), _n_);   \
        })

व्योम iovec_erase(काष्ठा iovec *iovec);
व्योम iovec_done_erase(काष्ठा iovec *iovec);

अक्षर* set_iovec_string_field(काष्ठा iovec *iovec, माप_प्रकार *n_iovec, स्थिर अक्षर *field, स्थिर अक्षर *मूल्य);
अक्षर* set_iovec_string_field_free(काष्ठा iovec *iovec, माप_प्रकार *n_iovec, स्थिर अक्षर *field, अक्षर *मूल्य);

व्योम iovec_array_free(काष्ठा iovec *iovec, माप_प्रकार n_iovec) _nonnull_if_nonzero_(1, 2);

पूर्णांक iovec_memcmp(स्थिर काष्ठा iovec *a, स्थिर काष्ठा iovec *b) _pure_;
अटल अंतरभूत bool iovec_equal(स्थिर काष्ठा iovec *a, स्थिर काष्ठा iovec *b) {
        वापस iovec_memcmp(a, b) == 0;
}

काष्ठा iovec* iovec_memdup(स्थिर काष्ठा iovec *source, काष्ठा iovec *ret);
पूर्णांक iovec_done_and_memdup(काष्ठा iovec *iovec, स्थिर काष्ठा iovec *source);

काष्ठा iovec* iovec_append(काष्ठा iovec *iovec, स्थिर काष्ठा iovec *append);

काष्ठा iovec* iovec_consume(काष्ठा iovec *iovec, माप_प्रकार n);
