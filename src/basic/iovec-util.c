/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "iovec-util.h"
#ضمن "string-util.h"

ساكن ثابت uint8_t nul_byte = 0;

ثابت بنية iovec iovec_nul_byte = {
        .iov_base = (فراغ*) &nul_byte,
        .iov_len = 1,
};

ثابت بنية iovec iovec_تشه = {
        .iov_base = (فراغ*) &nul_byte,
        .iov_len = 0,
};

صحيح iovec_alloc(size_t n, بنية iovec *ret) {
        أكد(ret);

        فراغ *buf = حافظه_ورکړه(n ?: 1);
        إذا (!buf)
                أرجع -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        أرجع 0;
}

size_t iovec_ټول_اندازه(ثابت بنية iovec *iovec, size_t n) {
        size_t مجموعه = 0;

        أكد(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                إذا (j->iov_len > SIZE_MAX - مجموعه)
                        أرجع SIZE_MAX; /* Indicate overflow. */
                مجموعه += j->iov_len;
        }

        أرجع مجموعه;
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
                        أرجع false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_سېټ(j);
        }

        أكد(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        أرجع !have;
}

بنية iovec* iovec_make_string(بنية iovec *iovec, ثابت حرف *s) {
        أكد(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        أرجع iovec;
}

فراغ iovec_erase(بنية iovec *iovec) {
        أكد(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_خوندي(iovec->iov_base, iovec->iov_len);
}

فراغ iovec_بشپړ_erase(بنية iovec *iovec) {
        أكد(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

حرف* سېټ_iovec_string_ډګر(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *ډګر, ثابت حرف *ارزښت) {
        حرف *x;

        أكد(iovec);
        أكد(n_iovec);

        x = strjoin(ډګر, ارزښت);
        إذا (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        أرجع x;
}

حرف* سېټ_iovec_string_ډګر_free(بنية iovec *iovec, size_t *n_iovec, ثابت حرف *ډګر, حرف *ارزښت) {
        حرف *x;

        أكد(iovec);
        أكد(n_iovec);

        x = سېټ_iovec_string_ډګر(iovec, n_iovec, ډګر, ارزښت);
        حرر(ارزښت);
        أرجع x;
}

فراغ iovec_array_free(بنية iovec *iovec, size_t n_iovec) {
        أكد(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                حرر(i->iov_base);

        حرر(iovec);
}

صحيح iovec_memcmp(ثابت بنية iovec *a, ثابت بنية iovec *b) {

        إذا (a == b)
                أرجع 0;

        أرجع memcmp_nn(a ? a->iov_base : عدم,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : عدم,
                         b ? b->iov_len : 0);
}

بنية iovec* iovec_memdup(ثابت بنية iovec *سرچینه, بنية iovec *ret) {
        أكد(ret);

        إذا (!iovec_is_سېټ(سرچینه))
                *ret = (بنية iovec) {};
        وإلا {
                فراغ *p = memdup(سرچینه->iov_base, سرچینه->iov_len);
                إذا (!p)
                        أرجع عدم;

                *ret = IOVEC_MAKE(p, سرچینه->iov_len);
        }

        أرجع ret;
}

صحيح iovec_بشپړ_and_memdup(بنية iovec *iovec, ثابت بنية iovec *سرچینه) {
        أكد(iovec);

        إذا (iovec_equal(iovec, سرچینه))
                أرجع 0;

        بنية iovec copy;
        إذا (!iovec_memdup(سرچینه, &copy))
                أرجع -ENOMEM;

        iovec_بشپړ(iovec);
        *iovec = copy;
        أرجع 1;
}

بنية iovec* iovec_append(بنية iovec *iovec, ثابت بنية iovec *append) {
        أكد(iovec_is_valid(iovec));

        إذا (!iovec_is_سېټ(append))
                أرجع iovec;

        إذا (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                أرجع عدم;

        أرجع iovec;
}

بنية iovec* iovec_consume(بنية iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        إذا (!iovec)
                أرجع عدم;

        إذا (n > iovec->iov_len)
                أرجع عدم;

        إذا (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                أرجع iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        أرجع iovec;
}
