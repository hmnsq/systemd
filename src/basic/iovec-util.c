/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "string-util.h"

स्थिर ध्रुव uint8_t nul_byte = 0;

ध्रुव संरचना iovec iovec_nul_byte = {
        .iov_base = (शून्य*) &nul_byte,
        .iov_len = 1,
};

ध्रुव संरचना iovec iovec_empty = {
        .iov_base = (शून्य*) &nul_byte,
        .iov_len = 0,
};

पूर्णाङ्क iovec_alloc(size_t n, संरचना iovec *ret) {
        प्रतिज्ञा(ret);

        शून्य *buf = malloc(n ?: 1);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        प्रत्यागम 0;
}

size_t iovec_total_size(ध्रुव संरचना iovec *iovec, size_t n) {
        size_t sum = 0;

        प्रतिज्ञा(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                यदि (j->iov_len > SIZE_MAX - sum)
                        प्रत्यागम SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        प्रत्यागम sum;
}

bool iovec_inc_many(संरचना iovec *iovec, size_t n, size_t k) {
        प्रतिज्ञा(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                यदि (j->iov_len == 0)
                        अनुवर्त;
                यदि (k == 0)
                        प्रत्यागम false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        प्रतिज्ञा(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        प्रत्यागम !have;
}

संरचना iovec* iovec_make_string(संरचना iovec *iovec, ध्रुव वर्ण *s) {
        प्रतिज्ञा(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        प्रत्यागम iovec;
}

शून्य iovec_erase(संरचना iovec *iovec) {
        प्रतिज्ञा(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

शून्य iovec_done_erase(संरचना iovec *iovec) {
        प्रतिज्ञा(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

वर्ण* set_iovec_string_field(संरचना iovec *iovec, size_t *n_iovec, ध्रुव वर्ण *field, ध्रुव वर्ण *मूल्य) {
        वर्ण *x;

        प्रतिज्ञा(iovec);
        प्रतिज्ञा(n_iovec);

        x = strjoin(field, मूल्य);
        यदि (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        प्रत्यागम x;
}

वर्ण* set_iovec_string_field_free(संरचना iovec *iovec, size_t *n_iovec, ध्रुव वर्ण *field, वर्ण *मूल्य) {
        वर्ण *x;

        प्रतिज्ञा(iovec);
        प्रतिज्ञा(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, मूल्य);
        मुक्त(मूल्य);
        प्रत्यागम x;
}

शून्य iovec_array_free(संरचना iovec *iovec, size_t n_iovec) {
        प्रतिज्ञा(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                मुक्त(i->iov_base);

        मुक्त(iovec);
}

पूर्णाङ्क iovec_memcmp(ध्रुव संरचना iovec *a, ध्रुव संरचना iovec *b) {

        यदि (a == b)
                प्रत्यागम 0;

        प्रत्यागम memcmp_nn(a ? a->iov_base : रिक्त,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : रिक्त,
                         b ? b->iov_len : 0);
}

संरचना iovec* iovec_memdup(ध्रुव संरचना iovec *source, संरचना iovec *ret) {
        प्रतिज्ञा(ret);

        यदि (!iovec_is_set(source))
                *ret = (संरचना iovec) {};
        अन्यथा {
                शून्य *p = memdup(source->iov_base, source->iov_len);
                यदि (!p)
                        प्रत्यागम रिक्त;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        प्रत्यागम ret;
}

पूर्णाङ्क iovec_done_and_memdup(संरचना iovec *iovec, ध्रुव संरचना iovec *source) {
        प्रतिज्ञा(iovec);

        यदि (iovec_equal(iovec, source))
                प्रत्यागम 0;

        संरचना iovec copy;
        यदि (!iovec_memdup(source, &copy))
                प्रत्यागम -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        प्रत्यागम 1;
}

संरचना iovec* iovec_append(संरचना iovec *iovec, ध्रुव संरचना iovec *append) {
        प्रतिज्ञा(iovec_is_valid(iovec));

        यदि (!iovec_is_set(append))
                प्रत्यागम iovec;

        यदि (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                प्रत्यागम रिक्त;

        प्रत्यागम iovec;
}

संरचना iovec* iovec_consume(संरचना iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        यदि (!iovec)
                प्रत्यागम रिक्त;

        यदि (n > iovec->iov_len)
                प्रत्यागम रिक्त;

        यदि (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                प्रत्यागम iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        प्रत्यागम iovec;
}
