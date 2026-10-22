/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ସ୍ଥିର ଅଚଳ uint8_t nul_byte = 0;

ଅଚଳ ସଂରଚନା iovec iovec_nul_byte = {
        .iov_base = (ଶୂନ୍ୟ*) &nul_byte,
        .iov_len = 1,
};

ଅଚଳ ସଂରଚନା iovec iovec_empty = {
        .iov_base = (ଶୂନ୍ୟ*) &nul_byte,
        .iov_len = 0,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_alloc(size_t n, ସଂରଚନା iovec *ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଶୂନ୍ୟ *buf = malloc(n ?: 1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        ଫେରନ୍ତୁ 0;
}

size_t iovec_total_size(ଅଚଳ ସଂରଚନା iovec *iovec, size_t n) {
        size_t sum = 0;

        ପ୍ରତିଜ୍ଞା(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                ଯଦି (j->iov_len > SIZE_MAX - sum)
                        ଫେରନ୍ତୁ SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        ଫେରନ୍ତୁ sum;
}

bool iovec_inc_many(ସଂରଚନା iovec *iovec, size_t n, size_t k) {
        ପ୍ରତିଜ୍ଞା(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                ଯଦି (j->iov_len == 0)
                        ଜାରି;
                ଯଦି (k == 0)
                        ଫେରନ୍ତୁ false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        ପ୍ରତିଜ୍ଞା(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        ଫେରନ୍ତୁ !have;
}

ସଂରଚନା iovec* iovec_make_string(ସଂରଚନା iovec *iovec, ଅଚଳ ଅକ୍ଷର *s) {
        ପ୍ରତିଜ୍ଞା(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        ଫେରନ୍ତୁ iovec;
}

ଶୂନ୍ୟ iovec_erase(ସଂରଚନା iovec *iovec) {
        ପ୍ରତିଜ୍ଞା(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

ଶୂନ୍ୟ iovec_done_erase(ସଂରଚନା iovec *iovec) {
        ପ୍ରତିଜ୍ଞା(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

ଅକ୍ଷର* set_iovec_string_field(ସଂରଚନା iovec *iovec, size_t *n_iovec, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଅକ୍ଷର *x;

        ପ୍ରତିଜ୍ଞା(iovec);
        ପ୍ରତିଜ୍ଞା(n_iovec);

        x = strjoin(field, ମୂଲ୍ୟ);
        ଯଦି (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        ଫେରନ୍ତୁ x;
}

ଅକ୍ଷର* set_iovec_string_field_free(ସଂରଚନା iovec *iovec, size_t *n_iovec, ଅଚଳ ଅକ୍ଷର *field, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଅକ୍ଷର *x;

        ପ୍ରତିଜ୍ଞା(iovec);
        ପ୍ରତିଜ୍ଞା(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, ମୂଲ୍ୟ);
        ମୁକ୍ତ(ମୂଲ୍ୟ);
        ଫେରନ୍ତୁ x;
}

ଶୂନ୍ୟ iovec_array_free(ସଂରଚନା iovec *iovec, size_t n_iovec) {
        ପ୍ରତିଜ୍ଞା(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                ମୁକ୍ତ(i->iov_base);

        ମୁକ୍ତ(iovec);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_memcmp(ଅଚଳ ସଂରଚନା iovec *a, ଅଚଳ ସଂରଚନା iovec *b) {

        ଯଦି (a == b)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ memcmp_nn(a ? a->iov_base : ଖାଲି,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ଖାଲି,
                         b ? b->iov_len : 0);
}

ସଂରଚନା iovec* iovec_memdup(ଅଚଳ ସଂରଚନା iovec *source, ସଂରଚନା iovec *ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!iovec_is_set(source))
                *ret = (ସଂରଚନା iovec) {};
        ଅନ୍ୟଥା {
                ଶୂନ୍ୟ *p = memdup(source->iov_base, source->iov_len);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ ଖାଲି;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_done_and_memdup(ସଂରଚନା iovec *iovec, ଅଚଳ ସଂରଚନା iovec *source) {
        ପ୍ରତିଜ୍ଞା(iovec);

        ଯଦି (iovec_equal(iovec, source))
                ଫେରନ୍ତୁ 0;

        ସଂରଚନା iovec copy;
        ଯଦି (!iovec_memdup(source, &copy))
                ଫେରନ୍ତୁ -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        ଫେରନ୍ତୁ 1;
}

ସଂରଚନା iovec* iovec_append(ସଂରଚନା iovec *iovec, ଅଚଳ ସଂରଚନା iovec *append) {
        ପ୍ରତିଜ୍ଞା(iovec_is_valid(iovec));

        ଯଦି (!iovec_is_set(append))
                ଫେରନ୍ତୁ iovec;

        ଯଦି (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ iovec;
}

ସଂରଚନା iovec* iovec_consume(ସଂରଚନା iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        ଯଦି (!iovec)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (n > iovec->iov_len)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                ଫେରନ୍ତୁ iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        ଫେରନ୍ତୁ iovec;
}
