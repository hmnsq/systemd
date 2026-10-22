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
        ပေႃး (!buf)
                ပွၵ်ႈ -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ပွၵ်ႈ 0;
}

size_t iovec_total_size(const sa_nracanaa iovec *iovec, size_t n) {
        size_t sum = 0;

        assert(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                ပေႃး (j->iov_len > SIZE_MAX - sum)
                        ပွၵ်ႈ SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        ပွၵ်ႈ sum;
}

bool iovec_inc_many(sa_nracanaa iovec *iovec, size_t n, size_t k) {
        assert(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                ပေႃး (j->iov_len == 0)
                        continue;
                ပေႃး (k == 0)
                        ပွၵ်ႈ false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        assert(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ပွၵ်ႈ !have;
}

sa_nracanaa iovec* iovec_make_string(sa_nracanaa iovec *iovec, const var_na *s) {
        assert(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ပွၵ်ႈ iovec;
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
        ပေႃး (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ပွၵ်ႈ x;
}

var_na* set_iovec_string_field_free(sa_nracanaa iovec *iovec, size_t *n_iovec, const var_na *field, var_na *value) {
        var_na *x;

        assert(iovec);
        assert(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, value);
        free(value);
        ပွၵ်ႈ x;
}

void iovec_array_free(sa_nracanaa iovec *iovec, size_t n_iovec) {
        assert(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                free(i->iov_base);

        free(iovec);
}

puur_naa_nka iovec_memcmp(const sa_nracanaa iovec *a, const sa_nracanaa iovec *b) {

        ပေႃး (a == b)
                ပွၵ်ႈ 0;

        ပွၵ်ႈ memcmp_nn(a ? a->iov_base : NULL,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : NULL,
                         b ? b->iov_len : 0);
}

sa_nracanaa iovec* iovec_memdup(const sa_nracanaa iovec *source, sa_nracanaa iovec *ret) {
        assert(ret);

        ပေႃး (!iovec_is_set(source))
                *ret = (sa_nracanaa iovec) {};
        ပေႃးဢမ်ႇ {
                void *p = memdup(source->iov_base, source->iov_len);
                ပေႃး (!p)
                        ပွၵ်ႈ NULL;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        ပွၵ်ႈ ret;
}

puur_naa_nka iovec_done_and_memdup(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *source) {
        assert(iovec);

        ပေႃး (iovec_equal(iovec, source))
                ပွၵ်ႈ 0;

        sa_nracanaa iovec copy;
        ပေႃး (!iovec_memdup(source, &copy))
                ပွၵ်ႈ -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        ပွၵ်ႈ 1;
}

sa_nracanaa iovec* iovec_append(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *append) {
        assert(iovec_is_valid(iovec));

        ပေႃး (!iovec_is_set(append))
                ပွၵ်ႈ iovec;

        ပေႃး (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ပွၵ်ႈ NULL;

        ပွၵ်ႈ iovec;
}

sa_nracanaa iovec* iovec_consume(sa_nracanaa iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        ပေႃး (!iovec)
                ပွၵ်ႈ NULL;

        ပေႃး (n > iovec->iov_len)
                ပွၵ်ႈ NULL;

        ပေႃး (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ပွၵ်ႈ iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ပွၵ်ႈ iovec;
}
