/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <sys/uio.h>                /* IWYU pragma: export */

#ضمن "forward.h"

#ضمن "../fundamental/iovec-util.h" /* IWYU pragma: export */

خارجي ثابت بنية iovec iovec_nul_byte; /* Points to a single NUL byte */
خارجي ثابت بنية iovec iovec_خالی_تھاواں;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

صحيح iovec_alloc(size_t n, بنية iovec *ret);

size_t iovec_کل_ناپ(ثابت بنية iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(بنية iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

بنية iovec* iovec_make_string(بنية iovec *iovec, ثابت حرف *s);

#تعریف_کرو IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(بنية iovec) {}, s)

#تعریف_کرو CONST_IOVEC_MAKE_STRING(s)              \
        (ثابت بنية iovec) {                  \
                .iov_base = (حرف*) s,          \
                .iov_len = STRLEN(s),           \
        }

#تعریف_کرو IOVEC_MAKE_BYTE(c)                                      \
        (ثابت بنية iovec) {                                  \
                .iov_base = (حرف*) ((ثابت حرف[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#تعریف_کرو IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_محفوظ(_n_), _n_);   \
        })

فراغ iovec_erase(بنية iovec *iovec);
فراغ iovec_پورا_erase(بنية iovec *iovec);

حرف* سیٹ_iovec_string_میدان(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *میدان, ثابت حرف *قدر);
حرف* سیٹ_iovec_string_میدان_free(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *میدان, حرف *قدر);

فراغ iovec_array_free(بنية iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

صحيح iovec_memcmp(ثابت بنية iovec *a, ثابت بنية iovec *b) خالص;
ساكن مضمن bool iovec_equal(ثابت بنية iovec *a, ثابت بنية iovec *b) {
        أرجع iovec_memcmp(a, b) == 0;
}

بنية iovec* iovec_memdup(ثابت بنية iovec *سروت, بنية iovec *ret);
صحيح iovec_پورا_and_memdup(بنية iovec *iovec, ثابت بنية iovec *سروت);

بنية iovec* iovec_append(بنية iovec *iovec, ثابت بنية iovec *append);

بنية iovec* iovec_consume(بنية iovec *iovec, size_t n);
