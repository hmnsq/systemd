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

ཧྲིལ iovec_alloc(size_t n, sa_nracanaa iovec *ret) {
        assert(ret);

        void *buf = malloc(n ?: 1);
        གལ་སྲིད (!buf)
                ལོག -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ལོག 0;
}

size_t iovec_total_size(const sa_nracanaa iovec *iovec, size_t n) {
        size_t sum = 0;

        assert(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                གལ་སྲིད (j->iov_len > SIZE_MAX - sum)
                        ལོག SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        ལོག sum;
}

bool iovec_inc_many(sa_nracanaa iovec *iovec, size_t n, size_t k) {
        assert(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                གལ་སྲིད (j->iov_len == 0)
                        continue;
                གལ་སྲིད (k == 0)
                        ལོག false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        assert(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ལོག !have;
}

sa_nracanaa iovec* iovec_make_string(sa_nracanaa iovec *iovec, const var_na *s) {
        assert(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ལོག iovec;
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
        གལ་སྲིད (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ལོག x;
}

var_na* set_iovec_string_field_free(sa_nracanaa iovec *iovec, size_t *n_iovec, const var_na *field, var_na *value) {
        var_na *x;

        assert(iovec);
        assert(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, value);
        free(value);
        ལོག x;
}

void iovec_array_free(sa_nracanaa iovec *iovec, size_t n_iovec) {
        assert(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                free(i->iov_base);

        free(iovec);
}

ཧྲིལ iovec_memcmp(const sa_nracanaa iovec *a, const sa_nracanaa iovec *b) {

        གལ་སྲིད (a == b)
                ལོག 0;

        ལོག memcmp_nn(a ? a->iov_base : NULL,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : NULL,
                         b ? b->iov_len : 0);
}

sa_nracanaa iovec* iovec_memdup(const sa_nracanaa iovec *source, sa_nracanaa iovec *ret) {
        assert(ret);

        གལ་སྲིད (!iovec_is_set(source))
                *ret = (sa_nracanaa iovec) {};
        དེ་མིན {
                void *p = memdup(source->iov_base, source->iov_len);
                གལ་སྲིད (!p)
                        ལོག NULL;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        ལོག ret;
}

ཧྲིལ iovec_done_and_memdup(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *source) {
        assert(iovec);

        གལ་སྲིད (iovec_equal(iovec, source))
                ལོག 0;

        sa_nracanaa iovec copy;
        གལ་སྲིད (!iovec_memdup(source, &copy))
                ལོག -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        ལོག 1;
}

sa_nracanaa iovec* iovec_append(sa_nracanaa iovec *iovec, const sa_nracanaa iovec *append) {
        assert(iovec_is_valid(iovec));

        གལ་སྲིད (!iovec_is_set(append))
                ལོག iovec;

        གལ་སྲིད (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ལོག NULL;

        ལོག iovec;
}

sa_nracanaa iovec* iovec_consume(sa_nracanaa iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        གལ་སྲིད (!iovec)
                ལོག NULL;

        གལ་སྲིད (n > iovec->iov_len)
                ལོག NULL;

        གལ་སྲིད (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ལོག iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ལོག iovec;
}
