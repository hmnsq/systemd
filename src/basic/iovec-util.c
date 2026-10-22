/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਸਥਿਰ ਅਚਲ uint8_t nul_byte = 0;

ਅਚਲ ਬਣਤਰ iovec iovec_nul_byte = {
        .iov_base = (ਖਾਲੀ*) &nul_byte,
        .iov_len = 1,
};

ਅਚਲ ਬਣਤਰ iovec iovec_ਖਾਲੀ_ਥਾਵਾਂ = {
        .iov_base = (ਖਾਲੀ*) &nul_byte,
        .iov_len = 0,
};

ਪੂਰਨ iovec_alloc(size_t n, ਬਣਤਰ iovec *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਖਾਲੀ *buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(n ?: 1);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ਵਾਪਸ 0;
}

size_t iovec_ਕੁੱਲ_ਮਾਪ(ਅਚਲ ਬਣਤਰ iovec *iovec, size_t n) {
        size_t ਜੋੜ = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                ਜੇ (j->iov_len > SIZE_MAX - ਜੋੜ)
                        ਵਾਪਸ SIZE_MAX; /* Indicate overflow. */
                ਜੋੜ += j->iov_len;
        }

        ਵਾਪਸ ਜੋੜ;
}

bool iovec_inc_many(ਬਣਤਰ iovec *iovec, size_t n, size_t k) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                ਜੇ (j->iov_len == 0)
                        ਜਾਰੀ;
                ਜੇ (k == 0)
                        ਵਾਪਸ false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_ਸੈੱਟ(j);
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ਵਾਪਸ !have;
}

ਬਣਤਰ iovec* iovec_make_string(ਬਣਤਰ iovec *iovec, ਅਚਲ ਅੱਖਰ *s) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ਵਾਪਸ iovec;
}

ਖਾਲੀ iovec_erase(ਬਣਤਰ iovec *iovec) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_ਸੁਰੱਖਿਅਤ(iovec->iov_base, iovec->iov_len);
}

ਖਾਲੀ iovec_ਪੂਰਾ_erase(ਬਣਤਰ iovec *iovec) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

ਅੱਖਰ* ਸੈੱਟ_iovec_string_ਖੇਤਰ(ਬਣਤਰ iovec *iovec, size_t *n_iovec, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਅੱਖਰ *x;

        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);
        ਪ੍ਰਤਿਜ੍ਞਾ(n_iovec);

        x = strjoin(ਖੇਤਰ, ਮੁੱਲ);
        ਜੇ (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ਵਾਪਸ x;
}

ਅੱਖਰ* ਸੈੱਟ_iovec_string_ਖੇਤਰ_free(ਬਣਤਰ iovec *iovec, size_t *n_iovec, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅੱਖਰ *ਮੁੱਲ) {
        ਅੱਖਰ *x;

        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);
        ਪ੍ਰਤਿਜ੍ਞਾ(n_iovec);

        x = ਸੈੱਟ_iovec_string_ਖੇਤਰ(iovec, n_iovec, ਖੇਤਰ, ਮੁੱਲ);
        ਮੁਕਤ(ਮੁੱਲ);
        ਵਾਪਸ x;
}

ਖਾਲੀ iovec_array_free(ਬਣਤਰ iovec *iovec, size_t n_iovec) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                ਮੁਕਤ(i->iov_base);

        ਮੁਕਤ(iovec);
}

ਪੂਰਨ iovec_memcmp(ਅਚਲ ਬਣਤਰ iovec *a, ਅਚਲ ਬਣਤਰ iovec *b) {

        ਜੇ (a == b)
                ਵਾਪਸ 0;

        ਵਾਪਸ memcmp_nn(a ? a->iov_base : ਨਿਰਾ,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ਨਿਰਾ,
                         b ? b->iov_len : 0);
}

ਬਣਤਰ iovec* iovec_memdup(ਅਚਲ ਬਣਤਰ iovec *ਸਰੋਤ, ਬਣਤਰ iovec *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!iovec_is_ਸੈੱਟ(ਸਰੋਤ))
                *ret = (ਬਣਤਰ iovec) {};
        ਨਹੀਂਤਾਂ {
                ਖਾਲੀ *p = memdup(ਸਰੋਤ->iov_base, ਸਰੋਤ->iov_len);
                ਜੇ (!p)
                        ਵਾਪਸ ਨਿਰਾ;

                *ret = IOVEC_MAKE(p, ਸਰੋਤ->iov_len);
        }

        ਵਾਪਸ ret;
}

ਪੂਰਨ iovec_ਪੂਰਾ_and_memdup(ਬਣਤਰ iovec *iovec, ਅਚਲ ਬਣਤਰ iovec *ਸਰੋਤ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);

        ਜੇ (iovec_equal(iovec, ਸਰੋਤ))
                ਵਾਪਸ 0;

        ਬਣਤਰ iovec copy;
        ਜੇ (!iovec_memdup(ਸਰੋਤ, &copy))
                ਵਾਪਸ -ENOMEM;

        iovec_ਪੂਰਾ(iovec);
        *iovec = copy;
        ਵਾਪਸ 1;
}

ਬਣਤਰ iovec* iovec_append(ਬਣਤਰ iovec *iovec, ਅਚਲ ਬਣਤਰ iovec *append) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec_is_valid(iovec));

        ਜੇ (!iovec_is_ਸੈੱਟ(append))
                ਵਾਪਸ iovec;

        ਜੇ (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ iovec;
}

ਬਣਤਰ iovec* iovec_consume(ਬਣਤਰ iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        ਜੇ (!iovec)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (n > iovec->iov_len)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ਵਾਪਸ iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ਵਾਪਸ iovec;
}
