/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "iovec-util.h"
#include "string-util.h"

static const uint8_t nul_byte = 0;

const struct iovec iovec_nul_byte = {
        .iov_base = (void*) &nul_byte,
        .iov_len = 1,
};

const struct iovec iovec_empty = {
        .iov_base = (void*) &nul_byte,
        .iov_len = 0,
};

정수 iovec_alloc(size_t n, struct iovec *ret) {
        assert(ret);

        void *buf = malloc(n ?: 1);
        만약 (!buf)
                반환 -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        반환 0;
}

size_t iovec_total_size(const struct iovec *iovec, size_t n) {
        size_t sum = 0;

        assert(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                만약 (j->iov_len > SIZE_MAX - sum)
                        반환 SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        반환 sum;
}

bool iovec_inc_many(struct iovec *iovec, size_t n, size_t k) {
        assert(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                만약 (j->iov_len == 0)
                        continue;
                만약 (k == 0)
                        반환 false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        assert(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        반환 !have;
}

struct iovec* iovec_make_string(struct iovec *iovec, const char *s) {
        assert(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        반환 iovec;
}

void iovec_erase(struct iovec *iovec) {
        assert(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

void iovec_done_erase(struct iovec *iovec) {
        assert(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

char* set_iovec_string_field(struct iovec *iovec, size_t *n_iovec, const char *field, const char *value) {
        char *x;

        assert(iovec);
        assert(n_iovec);

        x = strjoin(field, value);
        만약 (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        반환 x;
}

char* set_iovec_string_field_free(struct iovec *iovec, size_t *n_iovec, const char *field, char *value) {
        char *x;

        assert(iovec);
        assert(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, value);
        free(value);
        반환 x;
}

void iovec_array_free(struct iovec *iovec, size_t n_iovec) {
        assert(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                free(i->iov_base);

        free(iovec);
}

정수 iovec_memcmp(const struct iovec *a, const struct iovec *b) {

        만약 (a == b)
                반환 0;

        반환 memcmp_nn(a ? a->iov_base : NULL,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : NULL,
                         b ? b->iov_len : 0);
}

struct iovec* iovec_memdup(const struct iovec *source, struct iovec *ret) {
        assert(ret);

        만약 (!iovec_is_set(source))
                *ret = (struct iovec) {};
        아니면 {
                void *p = memdup(source->iov_base, source->iov_len);
                만약 (!p)
                        반환 NULL;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        반환 ret;
}

정수 iovec_done_and_memdup(struct iovec *iovec, const struct iovec *source) {
        assert(iovec);

        만약 (iovec_equal(iovec, source))
                반환 0;

        struct iovec copy;
        만약 (!iovec_memdup(source, &copy))
                반환 -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        반환 1;
}

struct iovec* iovec_append(struct iovec *iovec, const struct iovec *append) {
        assert(iovec_is_valid(iovec));

        만약 (!iovec_is_set(append))
                반환 iovec;

        만약 (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                반환 NULL;

        반환 iovec;
}

struct iovec* iovec_consume(struct iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        만약 (!iovec)
                반환 NULL;

        만약 (n > iovec->iov_len)
                반환 NULL;

        만약 (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                반환 iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        반환 iovec;
}
