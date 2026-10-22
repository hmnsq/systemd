/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "string-util.h"

സ്ഥിതം സ്ഥിരം uint8_t nul_byte = 0;

സ്ഥിരം ഘടന iovec iovec_nul_byte = {
        .iov_base = (ശൂന്യം*) &nul_byte,
        .iov_len = 1,
};

സ്ഥിരം ഘടന iovec iovec_empty = {
        .iov_base = (ശൂന്യം*) &nul_byte,
        .iov_len = 0,
};

പൂർണ്ണസംഖ്യ iovec_alloc(size_t n, ഘടന iovec *ret) {
        പ്രതിജ്ഞാ(ret);

        ശൂന്യം *buf = malloc(n ?: 1);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        തിരികെ 0;
}

size_t iovec_total_size(സ്ഥിരം ഘടന iovec *iovec, size_t n) {
        size_t sum = 0;

        പ്രതിജ്ഞാ(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                എങ്കിൽ (j->iov_len > SIZE_MAX - sum)
                        തിരികെ SIZE_MAX; /* Indicate overflow. */
                sum += j->iov_len;
        }

        തിരികെ sum;
}

bool iovec_inc_many(ഘടന iovec *iovec, size_t n, size_t k) {
        പ്രതിജ്ഞാ(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                എങ്കിൽ (j->iov_len == 0)
                        തുടരുക;
                എങ്കിൽ (k == 0)
                        തിരികെ false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        പ്രതിജ്ഞാ(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        തിരികെ !have;
}

ഘടന iovec* iovec_make_string(ഘടന iovec *iovec, സ്ഥിരം അക്ഷരം *s) {
        പ്രതിജ്ഞാ(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        തിരികെ iovec;
}

ശൂന്യം iovec_erase(ഘടന iovec *iovec) {
        പ്രതിജ്ഞാ(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

ശൂന്യം iovec_done_erase(ഘടന iovec *iovec) {
        പ്രതിജ്ഞാ(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

അക്ഷരം* set_iovec_string_field(ഘടന iovec *iovec, size_t *n_iovec, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        അക്ഷരം *x;

        പ്രതിജ്ഞാ(iovec);
        പ്രതിജ്ഞാ(n_iovec);

        x = strjoin(field, മൂല്യം);
        എങ്കിൽ (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        തിരികെ x;
}

അക്ഷരം* set_iovec_string_field_free(ഘടന iovec *iovec, size_t *n_iovec, സ്ഥിരം അക്ഷരം *field, അക്ഷരം *മൂല്യം) {
        അക്ഷരം *x;

        പ്രതിജ്ഞാ(iovec);
        പ്രതിജ്ഞാ(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, മൂല്യം);
        സ്വതന്ത്രം(മൂല്യം);
        തിരികെ x;
}

ശൂന്യം iovec_array_free(ഘടന iovec *iovec, size_t n_iovec) {
        പ്രതിജ്ഞാ(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                സ്വതന്ത്രം(i->iov_base);

        സ്വതന്ത്രം(iovec);
}

പൂർണ്ണസംഖ്യ iovec_memcmp(സ്ഥിരം ഘടന iovec *a, സ്ഥിരം ഘടന iovec *b) {

        എങ്കിൽ (a == b)
                തിരികെ 0;

        തിരികെ memcmp_nn(a ? a->iov_base : ശൂന്യ,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ശൂന്യ,
                         b ? b->iov_len : 0);
}

ഘടന iovec* iovec_memdup(സ്ഥിരം ഘടന iovec *source, ഘടന iovec *ret) {
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!iovec_is_set(source))
                *ret = (ഘടന iovec) {};
        അല്ലെങ്കിൽ {
                ശൂന്യം *p = memdup(source->iov_base, source->iov_len);
                എങ്കിൽ (!p)
                        തിരികെ ശൂന്യ;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ iovec_done_and_memdup(ഘടന iovec *iovec, സ്ഥിരം ഘടന iovec *source) {
        പ്രതിജ്ഞാ(iovec);

        എങ്കിൽ (iovec_equal(iovec, source))
                തിരികെ 0;

        ഘടന iovec copy;
        എങ്കിൽ (!iovec_memdup(source, &copy))
                തിരികെ -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        തിരികെ 1;
}

ഘടന iovec* iovec_append(ഘടന iovec *iovec, സ്ഥിരം ഘടന iovec *append) {
        പ്രതിജ്ഞാ(iovec_is_valid(iovec));

        എങ്കിൽ (!iovec_is_set(append))
                തിരികെ iovec;

        എങ്കിൽ (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                തിരികെ ശൂന്യ;

        തിരികെ iovec;
}

ഘടന iovec* iovec_consume(ഘടന iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        എങ്കിൽ (!iovec)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (n > iovec->iov_len)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                തിരികെ iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        തിരികെ iovec;
}
