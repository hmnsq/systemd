/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <sys/uio.h>                /* IWYU pragma: export */

#شامل "forward.h"

#شامل "../fundamental/iovec-util.h" /* IWYU pragma: export */

خارجی ثابت ساخت iovec iovec_nul_byte; /* Points to a single NUL byte */
خارجی ثابت ساخت iovec iovec_خالی_خانے;    /* Points to an empty, but valid (i.e. non-NULL) pointer */

صحیح iovec_alloc(size_t n, ساخت iovec *ret);

size_t iovec_کل_سائز(ثابت ساخت iovec *iovec, size_t n) _nonnull_if_nonzero_(1, 2);

bool iovec_inc_many(ساخت iovec *iovec, size_t n, size_t k) _nonnull_if_nonzero_(1, 2);

ساخت iovec* iovec_make_string(ساخت iovec *iovec, ثابت حرف *s);

#تعریف_کرو IOVEC_MAKE_STRING(s) \
        *iovec_make_string(&(ساخت iovec) {}, s)

#تعریف_کرو CONST_IOVEC_MAKE_STRING(s)              \
        (ثابت ساخت iovec) {                  \
                .iov_base = (حرف*) s,          \
                .iov_len = STRLEN(s),           \
        }

#تعریف_کرو IOVEC_MAKE_BYTE(c)                                      \
        (ثابت ساخت iovec) {                                  \
                .iov_base = (حرف*) ((ثابت حرف[]) { (c) }),   \
                .iov_len = 1,                                   \
        }

#تعریف_کرو IOVEC_ALLOCA(n)                              \
        ({                                           \
                size_t _n_ = (n);                    \
                IOVEC_MAKE(alloca_محفوظ(_n_), _n_);   \
        })

خالی iovec_erase(ساخت iovec *iovec);
خالی iovec_مکمل_erase(ساخت iovec *iovec);

حرف* سیٹ_iovec_string_میدان(ساخت iovec *iovec, size_t *n_iovec, ثابت حرف *میدان, ثابت حرف *قدر);
حرف* سیٹ_iovec_string_میدان_free(ساخت iovec *iovec, size_t *n_iovec, ثابت حرف *میدان, حرف *قدر);

خالی iovec_array_free(ساخت iovec *iovec, size_t n_iovec) _nonnull_if_nonzero_(1, 2);

صحیح iovec_memcmp(ثابت ساخت iovec *a, ثابت ساخت iovec *b) خالص;
ساکن اندرونی bool iovec_equal(ثابت ساخت iovec *a, ثابت ساخت iovec *b) {
        واپس iovec_memcmp(a, b) == 0;
}

ساخت iovec* iovec_memdup(ثابت ساخت iovec *ماخذ, ساخت iovec *ret);
صحیح iovec_مکمل_and_memdup(ساخت iovec *iovec, ثابت ساخت iovec *ماخذ);

ساخت iovec* iovec_append(ساخت iovec *iovec, ثابت ساخت iovec *append);

ساخت iovec* iovec_consume(ساخت iovec *iovec, size_t n);
