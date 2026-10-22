/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <sys/uio.h>                /* IWYU pragma: export */

#ضمن "forward.h"

#ضمن "../fundamental/iovec-util.h" /* IWYU pragma: export */

خارجي ثابت بنية iovec iovec_nul_byte; /* Points to a single NUL byte */
خارجي ثابت بنية iovec iovec_خالي_جايون;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

صحيح iovec_alloc(size_t n, بنية iovec *ret);

size_t iovec_ڪل_سائيز(ثابت بنية iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(بنية iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

بنية iovec* iovec_make_string(بنية iovec *iovec, ثابت حرف *s);

#وصف_ڪريو IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(بنية iovec) {}, s)

#وصف_ڪريو CONST_IOVEC_MAKE_STRING(s)              \
        (ثابت بنية iovec) {                  \
                .iov_base = (حرف*) s,          \
                .iov_len = STRLEN(s),           \
        }

#وصف_ڪريو IOVEC_MAKE_BYTE(c)                                      \
        (ثابت بنية iovec) {                                  \
                .iov_base = (حرف*) ((ثابت حرف[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#وصف_ڪريو IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_محفوظ(_n_), _n_);   \
        })

فراغ iovec_erase(بنية iovec *iovec);
فراغ iovec_پورو_erase(بنية iovec *iovec);

حرف* سيٽ_iovec_string_ميدان(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *ميدان, ثابت حرف *قدر);
حرف* سيٽ_iovec_string_ميدان_free(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *ميدان, حرف *قدر);

فراغ iovec_array_free(بنية iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

صحيح iovec_memcmp(ثابت بنية iovec *a, ثابت بنية iovec *b) خالص;
ساكن مضمن bool iovec_equal(ثابت بنية iovec *a, ثابت بنية iovec *b) {
        أرجع iovec_memcmp(a, b) == 0;
}

بنية iovec* iovec_memdup(ثابت بنية iovec *ذريعو, بنية iovec *ret);
صحيح iovec_پورو_and_memdup(بنية iovec *iovec, ثابت بنية iovec *ذريعو);

بنية iovec* iovec_append(بنية iovec *iovec, ثابت بنية iovec *append);

بنية iovec* iovec_consume(بنية iovec *iovec, size_t n);
