/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "string-util.h"

நிலையான மாறிலி uint8_t nul_byte = 0;

மாறிலி கட்டமைப்பு iovec iovec_nul_byte = {
        .iov_base = (வெறுமை*) &nul_byte,
        .iov_len = 1,
};

மாறிலி கட்டமைப்பு iovec iovec_empty = {
        .iov_base = (வெறுமை*) &nul_byte,
        .iov_len = 0,
};

முழு iovec_alloc(size_t n, கட்டமைப்பு iovec *ret) {
        ப்ரதிஜ்ஞா(ret);

        வெறுமை *buf = malloc(n ?: 1);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        திரும்பு 0;
}

size_t iovec_total_size(மாறிலி கட்டமைப்பு iovec *iovec, size_t n) {
        size_t sum = 0;

        ப்ரதிஜ்ஞா(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                எனில் (j->iov_len > SIZE_MAX - sum)
                        திரும்பு SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        திரும்பு sum;
}

bool iovec_inc_many(கட்டமைப்பு iovec *iovec, size_t n, size_t k) {
        ப்ரதிஜ்ஞா(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                எனில் (j->iov_len == 0)
                        தொடர்;
                எனில் (k == 0)
                        திரும்பு false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        ப்ரதிஜ்ஞா(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        திரும்பு !have;
}

கட்டமைப்பு iovec* iovec_make_string(கட்டமைப்பு iovec *iovec, மாறிலி எழுத்து *s) {
        ப்ரதிஜ்ஞா(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        திரும்பு iovec;
}

வெறுமை iovec_erase(கட்டமைப்பு iovec *iovec) {
        ப்ரதிஜ்ஞா(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

வெறுமை iovec_done_erase(கட்டமைப்பு iovec *iovec) {
        ப்ரதிஜ்ஞா(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

எழுத்து* set_iovec_string_field(கட்டமைப்பு iovec *iovec, size_t *n_iovec, மாறிலி எழுத்து *field, மாறிலி எழுத்து *மதிப்பு) {
        எழுத்து *x;

        ப்ரதிஜ்ஞா(iovec);
        ப்ரதிஜ்ஞா(n_iovec);

        x = strjoin(field, மதிப்பு);
        எனில் (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        திரும்பு x;
}

எழுத்து* set_iovec_string_field_free(கட்டமைப்பு iovec *iovec, size_t *n_iovec, மாறிலி எழுத்து *field, எழுத்து *மதிப்பு) {
        எழுத்து *x;

        ப்ரதிஜ்ஞா(iovec);
        ப்ரதிஜ்ஞா(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, மதிப்பு);
        விடுவி(மதிப்பு);
        திரும்பு x;
}

வெறுமை iovec_array_free(கட்டமைப்பு iovec *iovec, size_t n_iovec) {
        ப்ரதிஜ்ஞா(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                விடுவி(i->iov_base);

        விடுவி(iovec);
}

முழு iovec_memcmp(மாறிலி கட்டமைப்பு iovec *a, மாறிலி கட்டமைப்பு iovec *b) {

        எனில் (a == b)
                திரும்பு 0;

        திரும்பு memcmp_nn(a ? a->iov_base : வெற்று,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : வெற்று,
                         b ? b->iov_len : 0);
}

கட்டமைப்பு iovec* iovec_memdup(மாறிலி கட்டமைப்பு iovec *source, கட்டமைப்பு iovec *ret) {
        ப்ரதிஜ்ஞா(ret);

        எனில் (!iovec_is_set(source))
                *ret = (கட்டமைப்பு iovec) {};
        இல்லையெனில் {
                வெறுமை *p = memdup(source->iov_base, source->iov_len);
                எனில் (!p)
                        திரும்பு வெற்று;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        திரும்பு ret;
}

முழு iovec_done_and_memdup(கட்டமைப்பு iovec *iovec, மாறிலி கட்டமைப்பு iovec *source) {
        ப்ரதிஜ்ஞா(iovec);

        எனில் (iovec_equal(iovec, source))
                திரும்பு 0;

        கட்டமைப்பு iovec copy;
        எனில் (!iovec_memdup(source, &copy))
                திரும்பு -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        திரும்பு 1;
}

கட்டமைப்பு iovec* iovec_append(கட்டமைப்பு iovec *iovec, மாறிலி கட்டமைப்பு iovec *append) {
        ப்ரதிஜ்ஞா(iovec_is_valid(iovec));

        எனில் (!iovec_is_set(append))
                திரும்பு iovec;

        எனில் (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                திரும்பு வெற்று;

        திரும்பு iovec;
}

கட்டமைப்பு iovec* iovec_consume(கட்டமைப்பு iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        எனில் (!iovec)
                திரும்பு வெற்று;

        எனில் (n > iovec->iov_len)
                திரும்பு வெற்று;

        எனில் (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                திரும்பு iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        திரும்பு iovec;
}
