/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "iovec-util.h"
#شامل "string-util.h"

ساکن ثابت uint8_t nul_byte = 0;

ثابت ساخت iovec iovec_nul_byte = {
        .iov_base = (خالی*) &nul_byte,
        .iov_len = 1,
};

ثابت ساخت iovec iovec_خالی_خانے = {
        .iov_base = (خالی*) &nul_byte,
        .iov_len = 0,
};

صحیح iovec_alloc(size_t n, ساخت iovec *ret) {
        أكد(ret);

        خالی *buf = حافظہ_دو(n ?: 1);
        اگر (!buf)
                واپس -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        واپس 0;
}

size_t iovec_کل_سائز(ثابت ساخت iovec *iovec, size_t n) {
        size_t مجموع = 0;

        أكد(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                اگر (j->iov_len > SIZE_MAX - مجموع)
                        واپس SIZE_MAX; /* Indicate overflow. */
                مجموع += j->iov_len;
        }

        واپس مجموع;
}

bool iovec_inc_many(ساخت iovec *iovec, size_t n, size_t k) {
        أكد(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                اگر (j->iov_len == 0)
                        جاری;
                اگر (k == 0)
                        واپس false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_سیٹ(j);
        }

        أكد(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        واپس !have;
}

ساخت iovec* iovec_make_string(ساخت iovec *iovec, ثابت حرف *s) {
        أكد(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        واپس iovec;
}

خالی iovec_erase(ساخت iovec *iovec) {
        أكد(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_محفوظ(iovec->iov_base, iovec->iov_len);
}

خالی iovec_مکمل_erase(ساخت iovec *iovec) {
        أكد(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

حرف* سیٹ_iovec_string_میدان(ساخت iovec *iovec, size_t *n_iovec, ثابت حرف *میدان, ثابت حرف *قدر) {
        حرف *x;

        أكد(iovec);
        أكد(n_iovec);

        x = strjoin(میدان, قدر);
        اگر (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        واپس x;
}

حرف* سیٹ_iovec_string_میدان_free(ساخت iovec *iovec, size_t *n_iovec, ثابت حرف *میدان, حرف *قدر) {
        حرف *x;

        أكد(iovec);
        أكد(n_iovec);

        x = سیٹ_iovec_string_میدان(iovec, n_iovec, میدان, قدر);
        آزاد(قدر);
        واپس x;
}

خالی iovec_array_free(ساخت iovec *iovec, size_t n_iovec) {
        أكد(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                آزاد(i->iov_base);

        آزاد(iovec);
}

صحیح iovec_memcmp(ثابت ساخت iovec *a, ثابت ساخت iovec *b) {

        اگر (a == b)
                واپس 0;

        واپس memcmp_nn(a ? a->iov_base : تہی,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : تہی,
                         b ? b->iov_len : 0);
}

ساخت iovec* iovec_memdup(ثابت ساخت iovec *ماخذ, ساخت iovec *ret) {
        أكد(ret);

        اگر (!iovec_is_سیٹ(ماخذ))
                *ret = (ساخت iovec) {};
        ورنہ {
                خالی *p = memdup(ماخذ->iov_base, ماخذ->iov_len);
                اگر (!p)
                        واپس تہی;

                *ret = IOVEC_MAKE(p, ماخذ->iov_len);
        }

        واپس ret;
}

صحیح iovec_مکمل_and_memdup(ساخت iovec *iovec, ثابت ساخت iovec *ماخذ) {
        أكد(iovec);

        اگر (iovec_equal(iovec, ماخذ))
                واپس 0;

        ساخت iovec copy;
        اگر (!iovec_memdup(ماخذ, &copy))
                واپس -ENOMEM;

        iovec_مکمل(iovec);
        *iovec = copy;
        واپس 1;
}

ساخت iovec* iovec_append(ساخت iovec *iovec, ثابت ساخت iovec *append) {
        أكد(iovec_is_valid(iovec));

        اگر (!iovec_is_سیٹ(append))
                واپس iovec;

        اگر (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                واپس تہی;

        واپس iovec;
}

ساخت iovec* iovec_consume(ساخت iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        اگر (!iovec)
                واپس تہی;

        اگر (n > iovec->iov_len)
                واپس تہی;

        اگر (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                واپس iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        واپس iovec;
}
