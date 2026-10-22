/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "string-util.h"

સ્થિર અચલ uint8_t nul_byte = 0;

અચલ સંરચના iovec iovec_nul_byte = {
        .iov_base = (શૂન્ય*) &nul_byte,
        .iov_len = 1,
};

અચલ સંરચના iovec iovec_empty = {
        .iov_base = (શૂન્ય*) &nul_byte,
        .iov_len = 0,
};

પૂર્ણાંક iovec_alloc(size_t n, સંરચના iovec *ret) {
        પ્રતિજ્ઞા(ret);

        શૂન્ય *buf = malloc(n ?: 1);
        જો (!buf)
                પાછા -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        પાછા 0;
}

size_t iovec_total_size(અચલ સંરચના iovec *iovec, size_t n) {
        size_t sum = 0;

        પ્રતિજ્ઞા(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                જો (j->iov_len > SIZE_MAX - sum)
                        પાછા SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        પાછા sum;
}

bool iovec_inc_many(સંરચના iovec *iovec, size_t n, size_t k) {
        પ્રતિજ્ઞા(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                જો (j->iov_len == 0)
                        ચાલુ;
                જો (k == 0)
                        પાછા false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        પ્રતિજ્ઞા(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        પાછા !have;
}

સંરચના iovec* iovec_make_string(સંરચના iovec *iovec, અચલ અક્ષર *s) {
        પ્રતિજ્ઞા(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        પાછા iovec;
}

શૂન્ય iovec_erase(સંરચના iovec *iovec) {
        પ્રતિજ્ઞા(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

શૂન્ય iovec_done_erase(સંરચના iovec *iovec) {
        પ્રતિજ્ઞા(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

અક્ષર* set_iovec_string_field(સંરચના iovec *iovec, size_t *n_iovec, અચલ અક્ષર *field, અચલ અક્ષર *કિંમત) {
        અક્ષર *x;

        પ્રતિજ્ઞા(iovec);
        પ્રતિજ્ઞા(n_iovec);

        x = strjoin(field, કિંમત);
        જો (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        પાછા x;
}

અક્ષર* set_iovec_string_field_free(સંરચના iovec *iovec, size_t *n_iovec, અચલ અક્ષર *field, અક્ષર *કિંમત) {
        અક્ષર *x;

        પ્રતિજ્ઞા(iovec);
        પ્રતિજ્ઞા(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, કિંમત);
        મુક્ત(કિંમત);
        પાછા x;
}

શૂન્ય iovec_array_free(સંરચના iovec *iovec, size_t n_iovec) {
        પ્રતિજ્ઞા(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                મુક્ત(i->iov_base);

        મુક્ત(iovec);
}

પૂર્ણાંક iovec_memcmp(અચલ સંરચના iovec *a, અચલ સંરચના iovec *b) {

        જો (a == b)
                પાછા 0;

        પાછા memcmp_nn(a ? a->iov_base : ખાલી,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ખાલી,
                         b ? b->iov_len : 0);
}

સંરચના iovec* iovec_memdup(અચલ સંરચના iovec *source, સંરચના iovec *ret) {
        પ્રતિજ્ઞા(ret);

        જો (!iovec_is_set(source))
                *ret = (સંરચના iovec) {};
        નહીંતર {
                શૂન્ય *p = memdup(source->iov_base, source->iov_len);
                જો (!p)
                        પાછા ખાલી;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        પાછા ret;
}

પૂર્ણાંક iovec_done_and_memdup(સંરચના iovec *iovec, અચલ સંરચના iovec *source) {
        પ્રતિજ્ઞા(iovec);

        જો (iovec_equal(iovec, source))
                પાછા 0;

        સંરચના iovec copy;
        જો (!iovec_memdup(source, &copy))
                પાછા -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        પાછા 1;
}

સંરચના iovec* iovec_append(સંરચના iovec *iovec, અચલ સંરચના iovec *append) {
        પ્રતિજ્ઞા(iovec_is_valid(iovec));

        જો (!iovec_is_set(append))
                પાછા iovec;

        જો (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                પાછા ખાલી;

        પાછા iovec;
}

સંરચના iovec* iovec_consume(સંરચના iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        જો (!iovec)
                પાછા ખાલી;

        જો (n > iovec->iov_len)
                પાછા ખાલી;

        જો (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                પાછા iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        પાછા iovec;
}
