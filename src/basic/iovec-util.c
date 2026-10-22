/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "string-util.h"

స్థిర స్థిరరాశి uint8_t nul_byte = 0;

స్థిరరాశి నిర్మాణం iovec iovec_nul_byte = {
        .iov_base = (శూన్య*) &nul_byte,
        .iov_len = 1,
};

స్థిరరాశి నిర్మాణం iovec iovec_ఖాళీలు = {
        .iov_base = (శూన్య*) &nul_byte,
        .iov_len = 0,
};

పూర్ణాంకం iovec_alloc(size_t n, నిర్మాణం iovec *ret) {
        ప్రతిజ్ఞా(ret);

        శూన్య *buf = జ్ఞాపకం_ఇవ్వు(n ?: 1);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        *ret = IOVEC_MAKE(buf, n);
        తిరిగి 0;
}

size_t iovec_మొత్తం_కొలత(స్థిరరాశి నిర్మాణం iovec *iovec, size_t n) {
        size_t కూడిక = 0;

        ప్రతిజ్ఞా(iovec || n == 0);

        FOREACH_ARRAY(j, iovec, n) {
                ఒకవేళ (j->iov_len > SIZE_MAX - కూడిక)
                        తిరిగి SIZE_MAX; /* Indicate overflow. */
                కూడిక += j->iov_len;
        }

        తిరిగి కూడిక;
}

bool iovec_inc_many(నిర్మాణం iovec *iovec, size_t n, size_t k) {
        ప్రతిజ్ఞా(iovec || n == 0);

        /* Returns true if there is nothing else to send (bytes written cover all of the iovec),
         * false if there's still work to do. */

        bool have = false;
        FOREACH_ARRAY(j, iovec, n) {
                ఒకవేళ (j->iov_len == 0)
                        కొనసాగు;
                ఒకవేళ (k == 0)
                        తిరిగి false;

                size_t sub = MIN(j->iov_len, k);
                iovec_inc(j, sub);
                k -= sub;

                have = have || iovec_is_సమితి(j);
        }

        ప్రతిజ్ఞా(k == 0); /* Anything else would mean that we wrote more bytes than available,
                         * or the kernel reported writing more bytes than sent. */

        తిరిగి !have;
}

నిర్మాణం iovec* iovec_make_string(నిర్మాణం iovec *iovec, స్థిరరాశి అక్షరం *s) {
        ప్రతిజ్ఞా(iovec);

        *iovec = IOVEC_MAKE(s, strlen_ptr(s));
        తిరిగి iovec;
}

శూన్య iovec_erase(నిర్మాణం iovec *iovec) {
        ప్రతిజ్ఞా(iovec);

        /* Unlike iovec_done_erase(), which derives the buffer size with MALLOC_SIZEOF_SAFE(), this uses
         * iov_len as the buffer size. Hence, it can be used with iovec referring to a static array or a
         * buffer allocated on the stack. */
        explicit_bzero_సురక్షిత(iovec->iov_base, iovec->iov_len);
}

శూన్య iovec_పూర్తి_erase(నిర్మాణం iovec *iovec) {
        ప్రతిజ్ఞా(iovec);

        iovec->iov_base = erase_and_free(iovec->iov_base);
        iovec->iov_len = 0;
}

అక్షరం* సమితి_iovec_string_క్షేత్రం(నిర్మాణం iovec *iovec, size_t *n_iovec, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *విలువ) {
        అక్షరం *x;

        ప్రతిజ్ఞా(iovec);
        ప్రతిజ్ఞా(n_iovec);

        x = strjoin(క్షేత్రం, విలువ);
        ఒకవేళ (x)
                iovec[(*n_iovec)++] = IOVEC_MAKE_STRING(x);
        తిరిగి x;
}

అక్షరం* సమితి_iovec_string_క్షేత్రం_free(నిర్మాణం iovec *iovec, size_t *n_iovec, స్థిరరాశి అక్షరం *క్షేత్రం, అక్షరం *విలువ) {
        అక్షరం *x;

        ప్రతిజ్ఞా(iovec);
        ప్రతిజ్ఞా(n_iovec);

        x = సమితి_iovec_string_క్షేత్రం(iovec, n_iovec, క్షేత్రం, విలువ);
        విడుదల(విలువ);
        తిరిగి x;
}

శూన్య iovec_array_free(నిర్మాణం iovec *iovec, size_t n_iovec) {
        ప్రతిజ్ఞా(iovec || n_iovec == 0);

        FOREACH_ARRAY(i, iovec, n_iovec)
                విడుదల(i->iov_base);

        విడుదల(iovec);
}

పూర్ణాంకం iovec_memcmp(స్థిరరాశి నిర్మాణం iovec *a, స్థిరరాశి నిర్మాణం iovec *b) {

        ఒకవేళ (a == b)
                తిరిగి 0;

        తిరిగి memcmp_nn(a ? a->iov_base : ఖాళీ,
                         a ? a->iov_len : 0,
                         b ? b->iov_base : ఖాళీ,
                         b ? b->iov_len : 0);
}

నిర్మాణం iovec* iovec_memdup(స్థిరరాశి నిర్మాణం iovec *ఆధారం, నిర్మాణం iovec *ret) {
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!iovec_is_సమితి(ఆధారం))
                *ret = (నిర్మాణం iovec) {};
        లేకపోతే {
                శూన్య *p = memdup(ఆధారం->iov_base, ఆధారం->iov_len);
                ఒకవేళ (!p)
                        తిరిగి ఖాళీ;

                *ret = IOVEC_MAKE(p, ఆధారం->iov_len);
        }

        తిరిగి ret;
}

పూర్ణాంకం iovec_పూర్తి_and_memdup(నిర్మాణం iovec *iovec, స్థిరరాశి నిర్మాణం iovec *ఆధారం) {
        ప్రతిజ్ఞా(iovec);

        ఒకవేళ (iovec_equal(iovec, ఆధారం))
                తిరిగి 0;

        నిర్మాణం iovec copy;
        ఒకవేళ (!iovec_memdup(ఆధారం, &copy))
                తిరిగి -ENOMEM;

        iovec_పూర్తి(iovec);
        *iovec = copy;
        తిరిగి 1;
}

నిర్మాణం iovec* iovec_append(నిర్మాణం iovec *iovec, స్థిరరాశి నిర్మాణం iovec *append) {
        ప్రతిజ్ఞా(iovec_is_valid(iovec));

        ఒకవేళ (!iovec_is_సమితి(append))
                తిరిగి iovec;

        ఒకవేళ (!greedy_realloc_append(&iovec->iov_base, &iovec->iov_len, append->iov_base, append->iov_len, 1))
                తిరిగి ఖాళీ;

        తిరిగి iovec;
}

నిర్మాణం iovec* iovec_consume(నిర్మాణం iovec *iovec, size_t n) {

        /* Similar to iovec_inc(), but instead of moving the pointer ahead we copy the data via memmove() and
         * thus the pointer stays fixed, but the data moves */

        ఒకవేళ (!iovec)
                తిరిగి ఖాళీ;

        ఒకవేళ (n > iovec->iov_len)
                తిరిగి ఖాళీ;

        ఒకవేళ (n == 0) /* NOP — note that this is fine on an empty (or even unset) iovec, so that consuming an
                     * iovec we already fully drained continues to be a valid (if pointless) operation. */
                తిరిగి iovec;

        iovec->iov_len -= n;

        memmove(ASSERT_PTR(iovec->iov_base), (uint8_t*) iovec->iov_base + n, iovec->iov_len);
        తిరిగి iovec;
}
