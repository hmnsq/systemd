/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "iovec-util.h"
#समावेश "string-util.h"

अटल स्थिर uint8_t nul_byte = 0;

स्थिर काष्ठा iovec iovec_nul_byte = {
        .iov_base = (व्योम*) &nul_byte,
        .iov_len = 1,
};

स्थिर काष्ठा iovec iovec_empty = {
        .iov_base = (व्योम*) &nul_byte,
        .iov_len = 0,
};

पूर्णांक iovec_alloc(माप_प्रकार n, काष्ठा iovec *ret) {
        निश्चित(ret);

        व्योम *buf = दो_स्मृति(n ?: 1);
        अगर (!buf)
                वापस -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        वापस 0;
}

माप_प्रकार iovec_योग_आकार(स्थिर काष्ठा iovec *iovec, माप_प्रकार n) {
        माप_प्रकार योग = 0;

        निश्चित(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                अगर (j->iov_len > SIZE_MAX - योग)
                        वापस SIZE_MAX; /* Indicate overflow. */
                योग += j->iov_len;
        }

        वापस योग;
}

bool iovec_inc_many(काष्ठा iovec *iovec, माप_प्रकार n, माप_प्रकार k) {
        निश्चित(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                अगर (j->iov_len == 0)
                        जारी;
                अगर (k == 0)
                        वापस false;

                माप_प्रकार sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_set(j);
        }

        निश्चित(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        वापस !have;
}

काष्ठा iovec* iovec_make_string(काष्ठा iovec *iovec, स्थिर अक्षर *s) {
        निश्चित(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        वापस iovec;
}

व्योम iovec_erase(काष्ठा iovec *iovec) {
        निश्चित(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_safe(iovec->iov_base, iovec->iov_len);
}

व्योम iovec_done_erase(काष्ठा iovec *iovec) {
        निश्चित(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

अक्षर* set_iovec_string_field(काष्ठा iovec *iovec, माप_प्रकार *n_iovec, स्थिर अक्षर *field, स्थिर अक्षर *मूल्य) {
        अक्षर *x;

        निश्चित(iovec);
        निश्चित(n_iovec);

        x = strjoin(field, मूल्य);
        अगर (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        वापस x;
}

अक्षर* set_iovec_string_field_free(काष्ठा iovec *iovec, माप_प्रकार *n_iovec, स्थिर अक्षर *field, अक्षर *मूल्य) {
        अक्षर *x;

        निश्चित(iovec);
        निश्चित(n_iovec);

        x = set_iovec_string_field(iovec, n_iovec, field, मूल्य);
        मुक्त(मूल्य);
        वापस x;
}

व्योम iovec_array_free(काष्ठा iovec *iovec, माप_प्रकार n_iovec) {
        निश्चित(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                मुक्त(i->iov_base);

        मुक्त(iovec);
}

पूर्णांक iovec_memcmp(स्थिर काष्ठा iovec *a, स्थिर काष्ठा iovec *b) {

        अगर (a == b)
                वापस 0;

        वापस memcmp_nn(a ? a->iov_base : शून्य,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : शून्य,
                         b ? b->iov_len : 0);
}

काष्ठा iovec* iovec_memdup(स्थिर काष्ठा iovec *source, काष्ठा iovec *ret) {
        निश्चित(ret);

        अगर (!iovec_is_set(source))
                *ret = (काष्ठा iovec) {};
        अन्यथा {
                व्योम *p = memdup(source->iov_base, source->iov_len);
                अगर (!p)
                        वापस शून्य;

                *ret = IOVEC_MAKE(p, source->iov_len);
        }

        वापस ret;
}

पूर्णांक iovec_done_and_memdup(काष्ठा iovec *iovec, स्थिर काष्ठा iovec *source) {
        निश्चित(iovec);

        अगर (iovec_equal(iovec, source))
                वापस 0;

        काष्ठा iovec copy;
        अगर (!iovec_memdup(source, &copy))
                वापस -ENOMEM;

        iovec_done(iovec);
        *iovec = copy;
        वापस 1;
}

काष्ठा iovec* iovec_append(काष्ठा iovec *iovec, स्थिर काष्ठा iovec *append) {
        निश्चित(iovec_is_valid(iovec));

        अगर (!iovec_is_set(append))
                वापस iovec;

        अगर (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                वापस शून्य;

        वापस iovec;
}

काष्ठा iovec* iovec_consume(काष्ठा iovec *iovec, माप_प्रकार n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        अगर (!iovec)
                वापस शून्य;

        अगर (n > iovec->iov_len)
                वापस शून्य;

        अगर (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                वापस iovec;

        iovec->iov_len -= n;

        स_हटाओ(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        वापस iovec;
}
