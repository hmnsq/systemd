/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "iovec-util.h"
#شامل "string-util.h"

ایستا ثابت uint8_t nul_byte = 0;

ثابت ساختار iovec iovec_nul_byte = {
        .iov_base = (تهی*) &nul_byte,
        .iov_len = 1,
};

ثابت ساختار iovec iovec_empty = {
        .iov_base = (تهی*) &nul_byte,
        .iov_len = 0,
};

صحیح iovec_alloc(size_t n, ساختار iovec *ret) {
        أكد(ret);

        تهی *buf = malloc(n ?: 1);
        اگر (!buf)
                بازگشت -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        بازگشت 0;
}

size_t iovec_total_size(ثابت ساختار iovec *iovec, size_t n) {
        size_t sum = 0;

        أكد(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                اگر (j->iov_len > SIZE_MAX - sum)
                        بازگشت SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        بازگشت sum;
}

bool iovec_inc_many(ساختار iovec *iovec, size_t n, size_t k) {
        أكد(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                اگر (j->iov_len == 0)
                        ادامه;
                اگر (k == 0)
                        بازگشت false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        أكد(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        بازگشت !have;
}

ساختار iovec* iovec_make_string(ساختار iovec *iovec, ثابت نویسه *s) {
        أكد(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        بازگشت iovec;
}

تهی iovec_erase(ساختار iovec *iovec) {
        أكد(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

تهی iovec_done_erase(ساختار iovec *iovec) {
        أكد(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

نویسه* set_iovec_string_field(ساختار iovec *iovec, size_t *n_iovec, ثابت نویسه *field, ثابت نویسه *مقدار) {
        نویسه *x;

        أكد(iovec);
        أكد(n_iovec);

        x = strjoin(field, مقدار);
        اگر (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        بازگشت x;
}

نویسه* set_iovec_string_field_free(ساختار iovec *iovec, size_t *n_iovec, ثابت نویسه *field, نویسه *مقدار) {
        نویسه *x;

        أكد(iovec);
        أكد(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, مقدار);
        آزاد(مقدار);
        بازگشت x;
}

تهی iovec_array_free(ساختار iovec *iovec, size_t n_iovec) {
        أكد(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                آزاد(i->iov_base);

        آزاد(iovec);
}

صحیح iovec_memcmp(ثابت ساختار iovec *a, ثابت ساختار iovec *b) {

        اگر (a == b)
                بازگشت 0;

        بازگشت memcmp_nn(a ? a->iov_base : پوچ,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : پوچ,
                         b ? b->iov_len : 0);
}

ساختار iovec* iovec_memdup(ثابت ساختار iovec *source, ساختار iovec *ret) {
        أكد(ret);

        اگر (!iovec_is_set(source))
                *ret = (ساختار iovec) {};
        وگرنه {
                تهی *p = memdup(source->iov_base, source->iov_len);
                اگر (!p)
                        بازگشت پوچ;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        بازگشت ret;
}

صحیح iovec_done_and_memdup(ساختار iovec *iovec, ثابت ساختار iovec *source) {
        أكد(iovec);

        اگر (iovec_equal(iovec, source))
                بازگشت 0;

        ساختار iovec copy;
        اگر (!iovec_memdup(source, &copy))
                بازگشت -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        بازگشت 1;
}

ساختار iovec* iovec_append(ساختار iovec *iovec, ثابت ساختار iovec *append) {
        أكد(iovec_is_valid(iovec));

        اگر (!iovec_is_set(append))
                بازگشت iovec;

        اگر (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                بازگشت پوچ;

        بازگشت iovec;
}

ساختار iovec* iovec_consume(ساختار iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        اگر (!iovec)
                بازگشت پوچ;

        اگر (n > iovec->iov_len)
                بازگشت پوچ;

        اگر (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                بازگشت iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        بازگشت iovec;
}
