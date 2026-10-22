/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "string-util.h"

স্থির ধ্রুবক uint8_t nul_byte = 0;

ধ্রুবক কাঠামো iovec iovec_nul_byte = {
        .iov_base = (শূন্য*) &nul_byte,
        .iov_len = 1,
};

ধ্রুবক কাঠামো iovec iovec_empty = {
        .iov_base = (শূন্য*) &nul_byte,
        .iov_len = 0,
};

পূর্ণসংখ্যা iovec_alloc(size_t n, কাঠামো iovec *ret) {
        প্রতিজ্ঞা(ret);

        শূন্য *buf = malloc(n ?: 1);
        যদি (!buf)
                ফেরত -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ফেরত 0;
}

size_t iovec_যোগ_আকার(ধ্রুবক কাঠামো iovec *iovec, size_t n) {
        size_t যোগ = 0;

        প্রতিজ্ঞা(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                যদি (j->iov_len > SIZE_MAX - যোগ)
                        ফেরত SIZE_MAX; /* Indicate overflow. */
                যোগ += j->iov_len;
        }

        ফেরত যোগ;
}

bool iovec_inc_many(কাঠামো iovec *iovec, size_t n, size_t k) {
        প্রতিজ্ঞা(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                যদি (j->iov_len == 0)
                        চালু;
                যদি (k == 0)
                        ফেরত false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        প্রতিজ্ঞা(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ফেরত !have;
}

কাঠামো iovec* iovec_make_string(কাঠামো iovec *iovec, ধ্রুবক অক্ষর *s) {
        প্রতিজ্ঞা(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ফেরত iovec;
}

শূন্য iovec_erase(কাঠামো iovec *iovec) {
        প্রতিজ্ঞা(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

শূন্য iovec_done_erase(কাঠামো iovec *iovec) {
        প্রতিজ্ঞা(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

অক্ষর* set_iovec_string_field(কাঠামো iovec *iovec, size_t *n_iovec, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *মান) {
        অক্ষর *x;

        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n_iovec);

        x = strjoin(field, মান);
        যদি (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ফেরত x;
}

অক্ষর* set_iovec_string_field_free(কাঠামো iovec *iovec, size_t *n_iovec, ধ্রুবক অক্ষর *field, অক্ষর *মান) {
        অক্ষর *x;

        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, মান);
        মুক্ত(মান);
        ফেরত x;
}

শূন্য iovec_array_free(কাঠামো iovec *iovec, size_t n_iovec) {
        প্রতিজ্ঞা(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                মুক্ত(i->iov_base);

        মুক্ত(iovec);
}

পূর্ণসংখ্যা iovec_memcmp(ধ্রুবক কাঠামো iovec *a, ধ্রুবক কাঠামো iovec *b) {

        যদি (a == b)
                ফেরত 0;

        ফেরত memcmp_nn(a ? a->iov_base : ফাঁকা,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ফাঁকা,
                         b ? b->iov_len : 0);
}

কাঠামো iovec* iovec_memdup(ধ্রুবক কাঠামো iovec *source, কাঠামো iovec *ret) {
        প্রতিজ্ঞা(ret);

        যদি (!iovec_is_set(source))
                *ret = (কাঠামো iovec) {};
        অন্যথা {
                শূন্য *p = memdup(source->iov_base, source->iov_len);
                যদি (!p)
                        ফেরত ফাঁকা;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        ফেরত ret;
}

পূর্ণসংখ্যা iovec_done_and_memdup(কাঠামো iovec *iovec, ধ্রুবক কাঠামো iovec *source) {
        প্রতিজ্ঞা(iovec);

        যদি (iovec_equal(iovec, source))
                ফেরত 0;

        কাঠামো iovec copy;
        যদি (!iovec_memdup(source, &copy))
                ফেরত -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        ফেরত 1;
}

কাঠামো iovec* iovec_append(কাঠামো iovec *iovec, ধ্রুবক কাঠামো iovec *append) {
        প্রতিজ্ঞা(iovec_is_valid(iovec));

        যদি (!iovec_is_set(append))
                ফেরত iovec;

        যদি (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ফেরত ফাঁকা;

        ফেরত iovec;
}

কাঠামো iovec* iovec_consume(কাঠামো iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        যদি (!iovec)
                ফেরত ফাঁকা;

        যদি (n > iovec->iov_len)
                ফেরত ফাঁকা;

        যদি (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ফেরত iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ফেরত iovec;
}
