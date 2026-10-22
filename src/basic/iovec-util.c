/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರ ಧ್ರುವ uint8_t nul_byte = 0;

ಧ್ರುವ ಸಂರಚನಾ iovec iovec_nul_byte = {
        .iov_base = (ಶೂನ್ಯ*) &nul_byte,
        .iov_len = 1,
};

ಧ್ರುವ ಸಂರಚನಾ iovec iovec_empty = {
        .iov_base = (ಶೂನ್ಯ*) &nul_byte,
        .iov_len = 0,
};

ಪೂರ್ಣಾಙ್ಕ iovec_alloc(size_t n, ಸಂರಚನಾ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಶೂನ್ಯ *buf = malloc(n ?: 1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

size_t iovec_total_size(ಧ್ರುವ ಸಂರಚನಾ iovec *iovec, size_t n) {
        size_t sum = 0;

        ಪ್ರತಿಜ್ಞಾ(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                ಯದಿ (j->iov_len > SIZE_MAX - sum)
                        ಪ್ರತ್ಯಾಗಮ SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        ಪ್ರತ್ಯಾಗಮ sum;
}

bool iovec_inc_many(ಸಂರಚನಾ iovec *iovec, size_t n, size_t k) {
        ಪ್ರತಿಜ್ಞಾ(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                ಯದಿ (j->iov_len == 0)
                        ಅನುವರ್ತ;
                ಯದಿ (k == 0)
                        ಪ್ರತ್ಯಾಗಮ false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        ಪ್ರತಿಜ್ಞಾ(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ಪ್ರತ್ಯಾಗಮ !have;
}

ಸಂರಚನಾ iovec* iovec_make_string(ಸಂರಚನಾ iovec *iovec, ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ಪ್ರತ್ಯಾಗಮ iovec;
}

ಶೂನ್ಯ iovec_erase(ಸಂರಚನಾ iovec *iovec) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

ಶೂನ್ಯ iovec_done_erase(ಸಂರಚನಾ iovec *iovec) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

ವರ್ಣ* set_iovec_string_field(ಸಂರಚನಾ iovec *iovec, size_t *n_iovec, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ವರ್ಣ *x;

        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n_iovec);

        x = strjoin(field, ಮೌಲ್ಯ);
        ಯದಿ (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ಪ್ರತ್ಯಾಗಮ x;
}

ವರ್ಣ* set_iovec_string_field_free(ಸಂರಚನಾ iovec *iovec, size_t *n_iovec, ಧ್ರುವ ವರ್ಣ *field, ವರ್ಣ *ಮೌಲ್ಯ) {
        ವರ್ಣ *x;

        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, ಮೌಲ್ಯ);
        ಮುಕ್ತ(ಮೌಲ್ಯ);
        ಪ್ರತ್ಯಾಗಮ x;
}

ಶೂನ್ಯ iovec_array_free(ಸಂರಚನಾ iovec *iovec, size_t n_iovec) {
        ಪ್ರತಿಜ್ಞಾ(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                ಮುಕ್ತ(i->iov_base);

        ಮುಕ್ತ(iovec);
}

ಪೂರ್ಣಾಙ್ಕ iovec_memcmp(ಧ್ರುವ ಸಂರಚನಾ iovec *a, ಧ್ರುವ ಸಂರಚನಾ iovec *b) {

        ಯದಿ (a == b)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ memcmp_nn(a ? a->iov_base : ರಿಕ್ತ,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ರಿಕ್ತ,
                         b ? b->iov_len : 0);
}

ಸಂರಚನಾ iovec* iovec_memdup(ಧ್ರುವ ಸಂರಚನಾ iovec *source, ಸಂರಚನಾ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!iovec_is_set(source))
                *ret = (ಸಂರಚನಾ iovec) {};
        ಅನ್ಯಥಾ {
                ಶೂನ್ಯ *p = memdup(source->iov_base, source->iov_len);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ iovec_done_and_memdup(ಸಂರಚನಾ iovec *iovec, ಧ್ರುವ ಸಂರಚನಾ iovec *source) {
        ಪ್ರತಿಜ್ಞಾ(iovec);

        ಯದಿ (iovec_equal(iovec, source))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಸಂರಚನಾ iovec copy;
        ಯದಿ (!iovec_memdup(source, &copy))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸಂರಚನಾ iovec* iovec_append(ಸಂರಚನಾ iovec *iovec, ಧ್ರುವ ಸಂರಚನಾ iovec *append) {
        ಪ್ರತಿಜ್ಞಾ(iovec_is_valid(iovec));

        ಯದಿ (!iovec_is_set(append))
                ಪ್ರತ್ಯಾಗಮ iovec;

        ಯದಿ (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ iovec;
}

ಸಂರಚನಾ iovec* iovec_consume(ಸಂರಚನಾ iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        ಯದಿ (!iovec)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (n > iovec->iov_len)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ಪ್ರತ್ಯಾಗಮ iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ಪ್ರತ್ಯಾಗಮ iovec;
}
