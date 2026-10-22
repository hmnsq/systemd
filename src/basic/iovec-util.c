/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "iovec-util.h"
#ضمن "string-util.h"

ساكن ثابت uint8_t nul_byte = 0;

ثابت بنية iovec iovec_nul_byte = {
        .iov_base = (فراغ*) &nul_byte,
        .iov_len = 1,
};

ثابت بنية iovec iovec_empty = {
        .iov_base = (فراغ*) &nul_byte,
        .iov_len = 0,
};

صحيح iovec_alloc(size_t n, بنية iovec *ret) {
        أكد(ret);

        فراغ *buf = malloc(n ?: 1);
        إذا (!buf)
                إرجاع -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        إرجاع 0;
}

size_t iovec_total_size(ثابت بنية iovec *iovec, size_t n) {
        size_t sum = 0;

        أكد(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                إذا (j->iov_len > SIZE_MAX - sum)
                        إرجاع SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        إرجاع sum;
}

bool iovec_inc_many(بنية iovec *iovec, size_t n, size_t k) {
        أكد(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                إذا (j->iov_len == 0)
                        استمر;
                إذا (k == 0)
                        إرجاع false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        أكد(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        إرجاع !have;
}

بنية iovec* iovec_make_string(بنية iovec *iovec, ثابت حرف *s) {
        أكد(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        إرجاع iovec;
}

فراغ iovec_erase(بنية iovec *iovec) {
        أكد(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

فراغ iovec_done_erase(بنية iovec *iovec) {
        أكد(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

حرف* set_iovec_string_field(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *field, ثابت حرف *قيمة) {
        حرف *x;

        أكد(iovec);
        أكد(n_iovec);

        x = strjoin(field, قيمة);
        إذا (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        إرجاع x;
}

حرف* set_iovec_string_field_free(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *field, حرف *قيمة) {
        حرف *x;

        أكد(iovec);
        أكد(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, قيمة);
        حرر(قيمة);
        إرجاع x;
}

فراغ iovec_array_free(بنية iovec *iovec, size_t n_iovec) {
        أكد(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                حرر(i->iov_base);

        حرر(iovec);
}

صحيح iovec_memcmp(ثابت بنية iovec *a, ثابت بنية iovec *b) {

        إذا (a == b)
                إرجاع 0;

        إرجاع memcmp_nn(a ? a->iov_base : عدم,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : عدم,
                         b ? b->iov_len : 0);
}

بنية iovec* iovec_memdup(ثابت بنية iovec *source, بنية iovec *ret) {
        أكد(ret);

        إذا (!iovec_is_set(source))
                *ret = (بنية iovec) {};
        وإلا {
                فراغ *p = memdup(source->iov_base, source->iov_len);
                إذا (!p)
                        إرجاع عدم;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        إرجاع ret;
}

صحيح iovec_done_and_memdup(بنية iovec *iovec, ثابت بنية iovec *source) {
        أكد(iovec);

        إذا (iovec_equal(iovec, source))
                إرجاع 0;

        بنية iovec copy;
        إذا (!iovec_memdup(source, &copy))
                إرجاع -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        إرجاع 1;
}

بنية iovec* iovec_append(بنية iovec *iovec, ثابت بنية iovec *append) {
        أكد(iovec_is_valid(iovec));

        إذا (!iovec_is_set(append))
                إرجاع iovec;

        إذا (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                إرجاع عدم;

        إرجاع iovec;
}

بنية iovec* iovec_consume(بنية iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        إذا (!iovec)
                إرجاع عدم;

        إذا (n > iovec->iov_len)
                إرجاع عدم;

        إذا (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                إرجاع iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        إرجاع iovec;
}
