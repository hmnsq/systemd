/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "string-util.h"

স্থির ধ্রুব uint8_t nul_byte = 0;

ধ্রুব সংরচনা iovec iovec_nul_byte = {
        .iov_base = (শূন্য*) &nul_byte,
        .iov_len = 1,
};

ধ্রুব সংরচনা iovec iovec_empty = {
        .iov_base = (শূন্য*) &nul_byte,
        .iov_len = 0,
};

পূর্ণাঙ্ক iovec_alloc(size_t n, সংরচনা iovec *ret) {
        প্রতিজ্ঞা(ret);

        শূন্য *buf = malloc(n ?: 1);
        যদি (!buf)
                প্রত্যাগম -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        প্রত্যাগম 0;
}

size_t iovec_total_size(ধ্রুব সংরচনা iovec *iovec, size_t n) {
        size_t sum = 0;

        প্রতিজ্ঞা(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                যদি (j->iov_len > SIZE_MAX - sum)
                        প্রত্যাগম SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        প্রত্যাগম sum;
}

bool iovec_inc_many(সংরচনা iovec *iovec, size_t n, size_t k) {
        প্রতিজ্ঞা(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                যদি (j->iov_len == 0)
                        অনুবর্ত;
                যদি (k == 0)
                        প্রত্যাগম false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        প্রতিজ্ঞা(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        প্রত্যাগম !have;
}

সংরচনা iovec* iovec_make_string(সংরচনা iovec *iovec, ধ্রুব বর্ণ *s) {
        প্রতিজ্ঞা(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        প্রত্যাগম iovec;
}

শূন্য iovec_erase(সংরচনা iovec *iovec) {
        প্রতিজ্ঞা(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

শূন্য iovec_done_erase(সংরচনা iovec *iovec) {
        প্রতিজ্ঞা(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

বর্ণ* set_iovec_string_field(সংরচনা iovec *iovec, size_t *n_iovec, ধ্রুব বর্ণ *field, ধ্রুব বর্ণ *মান) {
        বর্ণ *x;

        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n_iovec);

        x = strjoin(field, মান);
        যদি (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        প্রত্যাগম x;
}

বর্ণ* set_iovec_string_field_free(সংরচনা iovec *iovec, size_t *n_iovec, ধ্রুব বর্ণ *field, বর্ণ *মান) {
        বর্ণ *x;

        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, মান);
        মুক্ত(মান);
        প্রত্যাগম x;
}

শূন্য iovec_array_free(সংরচনা iovec *iovec, size_t n_iovec) {
        প্রতিজ্ঞা(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                মুক্ত(i->iov_base);

        মুক্ত(iovec);
}

পূর্ণাঙ্ক iovec_memcmp(ধ্রুব সংরচনা iovec *a, ধ্রুব সংরচনা iovec *b) {

        যদি (a == b)
                প্রত্যাগম 0;

        প্রত্যাগম memcmp_nn(a ? a->iov_base : রিক্ত,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : রিক্ত,
                         b ? b->iov_len : 0);
}

সংরচনা iovec* iovec_memdup(ধ্রুব সংরচনা iovec *source, সংরচনা iovec *ret) {
        প্রতিজ্ঞা(ret);

        যদি (!iovec_is_set(source))
                *ret = (সংরচনা iovec) {};
        অন্যথা {
                শূন্য *p = memdup(source->iov_base, source->iov_len);
                যদি (!p)
                        প্রত্যাগম রিক্ত;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক iovec_done_and_memdup(সংরচনা iovec *iovec, ধ্রুব সংরচনা iovec *source) {
        প্রতিজ্ঞা(iovec);

        যদি (iovec_equal(iovec, source))
                প্রত্যাগম 0;

        সংরচনা iovec copy;
        যদি (!iovec_memdup(source, &copy))
                প্রত্যাগম -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        প্রত্যাগম 1;
}

সংরচনা iovec* iovec_append(সংরচনা iovec *iovec, ধ্রুব সংরচনা iovec *append) {
        প্রতিজ্ঞা(iovec_is_valid(iovec));

        যদি (!iovec_is_set(append))
                প্রত্যাগম iovec;

        যদি (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                প্রত্যাগম রিক্ত;

        প্রত্যাগম iovec;
}

সংরচনা iovec* iovec_consume(সংরচনা iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        যদি (!iovec)
                প্রত্যাগম রিক্ত;

        যদি (n > iovec->iov_len)
                প্রত্যাগম রিক্ত;

        যদি (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                প্রত্যাগম iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        প্রত্যাগম iovec;
}
