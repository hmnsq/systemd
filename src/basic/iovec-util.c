/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t nul_byte = 0;

ಸ್ಥಿರಾಂಕ ರಚನೆ iovec iovec_nul_byte = {
        .iov_base = (ಶೂನ್ಯ*) &nul_byte,
        .iov_len = 1,
};

ಸ್ಥಿರಾಂಕ ರಚನೆ iovec iovec_ಖಾಲಿಗಳು = {
        .iov_base = (ಶೂನ್ಯ*) &nul_byte,
        .iov_len = 0,
};

ಪೂರ್ಣಾಂಕ iovec_alloc(size_t n, ರಚನೆ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಶೂನ್ಯ *buf = ಸ್ಮರಣೆ_ಕೊಡು(n ?: 1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ಹಿಂತಿರುಗಿ 0;
}

size_t iovec_ಒಟ್ಟು_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iovec, size_t n) {
        size_t ಮೊತ್ತ = 0;

        ಪ್ರತಿಜ್ಞಾ(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                ಆದರೆ (j->iov_len > SIZE_MAX - ಮೊತ್ತ)
                        ಹಿಂತಿರುಗಿ SIZE_MAX; /* Indicate overflow. */
                ಮೊತ್ತ += j->iov_len;
        }

        ಹಿಂತಿರುಗಿ ಮೊತ್ತ;
}

bool iovec_inc_many(ರಚನೆ iovec *iovec, size_t n, size_t k) {
        ಪ್ರತಿಜ್ಞಾ(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                ಆದರೆ (j->iov_len == 0)
                        ಮುಂದುವರಿ;
                ಆದರೆ (k == 0)
                        ಹಿಂತಿರುಗಿ false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_ಗಣ(j);
        }

        ಪ್ರತಿಜ್ಞಾ(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ಹಿಂತಿರುಗಿ !have;
}

ರಚನೆ iovec* iovec_make_string(ರಚನೆ iovec *iovec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ಹಿಂತಿರುಗಿ iovec;
}

ಶೂನ್ಯ iovec_erase(ರಚನೆ iovec *iovec) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_ಸುರಕ್ಷಿತ(iovec->iov_base, iovec->iov_len);
}

ಶೂನ್ಯ iovec_ಮುಗಿದ_erase(ರಚನೆ iovec *iovec) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

ಅಕ್ಷರ* ಗಣ_iovec_string_ಕ್ಷೇತ್ರ(ರಚನೆ iovec *iovec, size_t *n_iovec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಅಕ್ಷರ *x;

        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n_iovec);

        x = strjoin(ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
        ಆದರೆ (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ಹಿಂತಿರುಗಿ x;
}

ಅಕ್ಷರ* ಗಣ_iovec_string_ಕ್ಷೇತ್ರ_free(ರಚನೆ iovec *iovec, size_t *n_iovec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಅಕ್ಷರ *x;

        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n_iovec);

        x = ಗಣ_iovec_string_ಕ್ಷೇತ್ರ(iovec, n_iovec, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
        ಬಿಡುಗಡೆ(ಮೌಲ್ಯ);
        ಹಿಂತಿರುಗಿ x;
}

ಶೂನ್ಯ iovec_array_free(ರಚನೆ iovec *iovec, size_t n_iovec) {
        ಪ್ರತಿಜ್ಞಾ(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                ಬಿಡುಗಡೆ(i->iov_base);

        ಬಿಡುಗಡೆ(iovec);
}

ಪೂರ್ಣಾಂಕ iovec_memcmp(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *a, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *b) {

        ಆದರೆ (a == b)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ memcmp_nn(a ? a->iov_base : ಖಾಲಿ,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ಖಾಲಿ,
                         b ? b->iov_len : 0);
}

ರಚನೆ iovec* iovec_memdup(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *ಮೂಲ, ರಚನೆ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!iovec_is_ಗಣ(ಮೂಲ))
                *ret = (ರಚನೆ iovec) {};
        ಇಲ್ಲದಿದ್ದರೆ {
                ಶೂನ್ಯ *p = memdup(ಮೂಲ->iov_base, ಮೂಲ->iov_len);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                *ret = IOVEC_MAKE(p, ಮೂಲ->iov_len);
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ iovec_ಮುಗಿದ_and_memdup(ರಚನೆ iovec *iovec, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *ಮೂಲ) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        ಆದರೆ (iovec_equal(iovec, ಮೂಲ))
                ಹಿಂತಿರುಗಿ 0;

        ರಚನೆ iovec copy;
        ಆದರೆ (!iovec_memdup(ಮೂಲ, &copy))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        iovec_ಮುಗಿದ(iovec);
        *iovec = copy;
        ಹಿಂತಿರುಗಿ 1;
}

ರಚನೆ iovec* iovec_append(ರಚನೆ iovec *iovec, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *append) {
        ಪ್ರತಿಜ್ಞಾ(iovec_is_valid(iovec));

        ಆದರೆ (!iovec_is_ಗಣ(append))
                ಹಿಂತಿರುಗಿ iovec;

        ಆದರೆ (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ iovec;
}

ರಚನೆ iovec* iovec_consume(ರಚನೆ iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        ಆದರೆ (!iovec)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (n > iovec->iov_len)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ಹಿಂತಿರುಗಿ iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ಹಿಂತಿರುಗಿ iovec;
}
