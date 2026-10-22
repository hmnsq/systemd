/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "iovec-util.h"
#include "string-util.h"

static const uint8_t nul_byte = 0;

const sa_nracanaa iovec iovec_nul_byte = {
        .iov_base = (void*) &nul_byte,
        .iov_len = 1,
};

const sa_nracanaa iovec iovec_empty = {
        .iov_base = (void*) &nul_byte,
        .iov_len = 0,
};

puur_naa_nka iovec_alloc(size_t n, sa_nracanaa iovec *ret) {
        assert(ret);

        void *buf = malloc(n ?: 1);
        ᤀᤡ᤺ (!buf)
                ᤀᤡᤴ -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ᤀᤡᤴ 0;
}

size_t iovec_total_size(const sa_nracanaa iovec *iovec, size_t n) {
        size_t sum = 0;

        assert(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                ᤀᤡ᤺ (j->iov_len > SIZE_MAX - sum)
                        ᤀᤡᤴ SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        ᤀᤡᤴ sum;
}

bool iovec_inc_many(sa_nracanaa iovec *iovec, size_t n, size_t k) {
        assert(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                ᤀᤡ᤺ (j->iov_len == 0)
                        continue;
                ᤀᤡ᤺ (k == 0)
                        ᤀᤡᤴ false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        assert(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ᤀᤡᤴ !have;
}

sa_nracanaa iovec* iovec_make_string(sa_nracanaa iovec *iovec, const var_na *s) {
        assert(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ᤀᤡᤴ iovec;
}

void iovec_erase(sa_nracanaa iovec *iovec) {
        assert(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

void iovec_done_erase(sa_nracanaa iovec *iovec) {
        assert(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

var_na* set_iovec_string_field(sa_nracanaa iovec *iovec, size_t *n_iovec, const var_na *field, const var_na *value) {
        var_na *x;

        assert(iovec);
        assert(n_iovec);

        x = strjoin(field, value);
        ᤀᤡ᤺ (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ᤀᤡᤴ x;
}

var_na* set_iovec_string_field_free(sa_nracanaa iovec *iovec, size_t *n_iovec, const var_na *field, var_na *value) {
        var_na *x;

        assert(iovec);
        assert(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, value);
        free(value);
        ᤀᤡᤴ x;
}

void iovec_array_free(sa_nracanaa iovec *iovec, size_t n_iovec) {
        assert(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                free(i->iov_base);

        free(iovec);
}

puur_naa_nka iovec_memcmp(const sa_nracanaa iovec *a, const sa_nracanaa iovec *b) {

        ᤀᤡ᤺ (a == b)
                ᤀᤡᤴ 0;

        ᤀᤡᤴ memcmp_nn(a ? a->iov_base : NULL,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : NULL,
                         b ? b->iov_len : 0);
}

sa_nracanaa iovec* iovec_memdup(const sa_nracanaa iovec *source, sa_nracanaa iovec *ret) {
        assert(ret);

        ᤀᤡ᤺ (!iovec_is_set(source))
                *ret = (sa_nracanaa iovec) {};
        ᤀᤡ᤺ᤆᤡ {
                void *p = memdup(source->iov_base, source->iov_len);
                ᤀᤡ᤺ (!p)
                        ᤀᤡᤴ NULL;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        ᤀᤡᤴ ret;
}

puur_naa_nka iovec_done_and_memdup(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *source) {
        assert(iovec);

        ᤀᤡ᤺ (iovec_equal(iovec, source))
                ᤀᤡᤴ 0;

        sa_nracanaa iovec copy;
        ᤀᤡ᤺ (!iovec_memdup(source, &copy))
                ᤀᤡᤴ -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        ᤀᤡᤴ 1;
}

sa_nracanaa iovec* iovec_append(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *append) {
        assert(iovec_is_valid(iovec));

        ᤀᤡ᤺ (!iovec_is_set(append))
                ᤀᤡᤴ iovec;

        ᤀᤡ᤺ (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ᤀᤡᤴ NULL;

        ᤀᤡᤴ iovec;
}

sa_nracanaa iovec* iovec_consume(sa_nracanaa iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        ᤀᤡ᤺ (!iovec)
                ᤀᤡᤴ NULL;

        ᤀᤡ᤺ (n > iovec->iov_len)
                ᤀᤡᤴ NULL;

        ᤀᤡ᤺ (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ᤀᤡᤴ iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ᤀᤡᤴ iovec;
}
