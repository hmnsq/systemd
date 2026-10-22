/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "iovec-wrapper.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "unaligned.h"

శూన్య iovw_పూర్తి(నిర్మాణం iovec_wrapper *iovw) {
        ప్రతిజ్ఞా(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->లెక్క = 0;
}

శూన్య iovw_పూర్తి_free(నిర్మాణం iovec_wrapper *iovw) {
        ప్రతిజ్ఞా(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->లెక్క)
                iovec_పూర్తి(i);

        iovw_పూర్తి(iovw);
}

నిర్మాణం iovec_wrapper* iovw_free(నిర్మాణం iovec_wrapper *iovw) {
        ఒకవేళ (!iovw)
                తిరిగి ఖాళీ;

        iovw_పూర్తి(iovw);
        తిరిగి mfree(iovw);
}

నిర్మాణం iovec_wrapper* iovw_free_free(నిర్మాణం iovec_wrapper *iovw) {
        ఒకవేళ (!iovw)
                తిరిగి ఖాళీ;

        iovw_పూర్తి_free(iovw);
        తిరిగి mfree(iovw);
}

పూర్ణాంకం iovw_compare(స్థిరరాశి నిర్మాణం iovec_wrapper *a, స్థిరరాశి నిర్మాణం iovec_wrapper *b) {
        పూర్ణాంకం r;

        ఒకవేళ (a == b)
                తిరిగి 0;

        ఒకవేళ (!a || !b)
                తిరిగి CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        కొరకు (size_t i = 0, n = MIN(a->లెక్క, b->లెక్క); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                ఒకవేళ (r != 0)
                        తిరిగి r;
        }

        తిరిగి CMP(a->లెక్క, b->లెక్క);
}

పూర్ణాంకం iovw_put_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, శూన్య *దత్తాంశం, size_t len) {
        ప్రతిజ్ఞా(iovw);
        ప్రతిజ్ఞా(దత్తాంశం || len == 0);

        ఒకవేళ (len == 0 && !accept_zero)
                తిరిగి 0;

        ఒకవేళ (iovw->లెక్క >= IOV_MAX)
                తిరిగి -E2BIG;

        ఒకవేళ (!GREEDY_REALLOC(iovw->iovec, iovw->లెక్క + 1))
                తిరిగి -ENOMEM;

        iovw->iovec[iovw->లెక్క++] = IOVEC_MAKE(దత్తాంశం, len);
        తిరిగి 1;
}

పూర్ణాంకం iovw_put_iov_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec *iov) {
        ప్రతిజ్ఞా(iovw);

        ఒకవేళ (!iov)
                తిరిగి 0;

        తిరిగి iovw_put_నిండిన(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

పూర్ణాంకం iovw_put_iovw_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec_wrapper *ఆధారం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(iovw);

        ఒకవేళ (iovw_isempty(ఆధారం))
                తిరిగి 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        ఒకవేళ (iovw == ఆధారం)
                తిరిగి -EINVAL;

        ఒకవేళ (iovw->లెక్క > SIZE_MAX - ఆధారం->లెక్క)
                తిరిగి -E2BIG;
        ఒకవేళ (iovw->లెక్క + ఆధారం->లెక్క > IOV_MAX)
                తిరిగి -E2BIG;

        ఒకవేళ (accept_zero) {
                ఒకవేళ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->లెక్క, ఆధారం->iovec, ఆధారం->లెక్క))
                        తిరిగి -ENOMEM;

                తిరిగి 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_లెక్క = iovw->లెక్క;

        FOREACH_ARRAY(iovec, ఆధారం->iovec, ఆధారం->లెక్క) {
                r = iovw_put_iov_నిండిన(iovw, accept_zero, iovec);
                ఒకవేళ (r < 0)
                        వెళ్ళు rollback;
        }

        తిరిగి 0;

rollback:
        iovw->లెక్క = original_లెక్క;
        తిరిగి r;
}

పూర్ణాంకం iovw_consume_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, శూన్య *దత్తాంశం, size_t len) {
        /* Move data into iovw or free on error */
        పూర్ణాంకం r;

        r = iovw_put_నిండిన(iovw, accept_zero, దత్తాంశం, len);
        ఒకవేళ (r <= 0)
                విడుదల(దత్తాంశం);

        తిరిగి r;
}

పూర్ణాంకం iovw_consume_iov_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, నిర్మాణం iovec *iov) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(iovw);

        ఒకవేళ (!iov)
                తిరిగి 0;

        r = iovw_put_iov_నిండిన(iovw, accept_zero, iov);
        ఒకవేళ (r <= 0)
                iovec_పూర్తి(iov);
        లేకపోతే
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (నిర్మాణం iovec) {};

        తిరిగి r;
}

పూర్ణాంకం iovw_extend_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి శూన్య *దత్తాంశం, size_t len) {
        ప్రతిజ్ఞా(iovw);
        ప్రతిజ్ఞా(దత్తాంశం || len == 0);

        ఒకవేళ (len == 0)
                తిరిగి iovw_put_నిండిన(iovw, accept_zero, /* data= */ ఖాళీ, /* len= */ 0);

        శూన్య *c = memdup(దత్తాంశం, len);
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        తిరిగి iovw_consume_నిండిన(iovw, accept_zero, c, len);
}

పూర్ణాంకం iovw_extend_iov_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec *iov) {
        ప్రతిజ్ఞా(iovw);

        ఒకవేళ (!iov)
                తిరిగి 0;

        తిరిగి iovw_extend_నిండిన(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

పూర్ణాంకం iovw_extend_iovw_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec_wrapper *ఆధారం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(iovw);

        /* This duplicates the source and merges it into the iovw. */

        ఒకవేళ (iovw_isempty(ఆధారం))
                తిరిగి 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        ఒకవేళ (iovw == ఆధారం)
                తిరిగి -EINVAL;

        ఒకవేళ (iovw->లెక్క > SIZE_MAX - ఆధారం->లెక్క)
                తిరిగి -E2BIG;
        ఒకవేళ (iovw->లెక్క + ఆధారం->లెక్క > IOV_MAX)
                తిరిగి -E2BIG;

        size_t original_లెక్క = iovw->లెక్క;

        FOREACH_ARRAY(iovec, ఆధారం->iovec, ఆధారం->లెక్క) {
                r = iovw_extend_iov_నిండిన(iovw, accept_zero, iovec);
                ఒకవేళ (r < 0)
                        వెళ్ళు rollback;
        }

        తిరిగి 0;

rollback:
        కొరకు (size_t i = original_లెక్క; i < iovw->లెక్క; i++)
                iovec_పూర్తి(iovw->iovec + i);

        iovw->లెక్క = original_లెక్క;
        తిరిగి r;
}

పూర్ణాంకం iovw_put_string_క్షేత్రం_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool replace, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *విలువ) {
        _cleanup_free_ అక్షరం *x = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(iovw);

        x = strjoin(క్షేత్రం, విలువ);
        ఒకవేళ (!x)
                తిరిగి -ENOMEM;

        ఒకవేళ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->లెక్క)
                        ఒకవేళ (జ్ఞాపకం_startswith(iovec->iov_base, iovec->iov_len, క్షేత్రం)) {
                                iovec->iov_len = అక్షరశ్రేణి_పొడవు(x);
                                తిరిగి free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, అక్షరశ్రేణి_పొడవు(x));
        ఒకవేళ (r >= 0)
                TAKE_PTR(x);

        తిరిగి r;
}

పూర్ణాంకం iovw_put_string_fieldf_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool replace, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *format, ...) {
        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        va_list ap;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(format);

        va_start(ap, format);
        r = vasprintf(&విలువ, format, ap);
        va_end(ap);
        ఒకవేళ (r < 0)
                తిరిగి -ENOMEM;

        తిరిగి iovw_put_string_క్షేత్రం_నిండిన(iovw, replace, క్షేత్రం, విలువ);
}

పూర్ణాంకం iovw_put_string_క్షేత్రం_free(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి అక్షరం *క్షేత్రం, అక్షరం *విలువ) {
        _cleanup_free_ వాడని అక్షరం *free_ptr = విలువ;

        తిరిగి iovw_put_string_క్షేత్రం(iovw, క్షేత్రం, విలువ);
}

శూన్య iovw_rebase(నిర్మాణం iovec_wrapper *iovw, శూన్య *old, శూన్య *new) {
        ప్రతిజ్ఞా(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->లెక్క) {
                ప్రతిజ్ఞా(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_కొలత(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw) {
        ఒకవేళ (iovw_isempty(iovw))
                తిరిగి 0;

        తిరిగి iovec_మొత్తం_కొలత(iovw->iovec, iovw->లెక్క);
}

పూర్ణాంకం iovw_concat(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw, నిర్మాణం iovec *ret) {
        ప్రతిజ్ఞా(iovw);
        ప్రతిజ్ఞా(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_కొలత(iovw);
        ఒకవేళ (len == SIZE_MAX)
                తిరిగి -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = జ్ఞాపకం_ఇవ్వు(len + 1);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->లెక్క)
                p = mempcpy_సురక్షిత(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        తిరిగి 0;
}

అక్షరం* iovw_గమ్యం_cstring(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw) {
        ప్రతిజ్ఞా(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_పూర్తి) నిర్మాణం iovec iov = {};
        ఒకవేళ (iovw_concat(iovw, &iov) < 0)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(!memchr(iov.iov_base, 0, iov.iov_len));
        తిరిగి TAKE_PTR(iov.iov_base);
}

పూర్ణాంకం iovec_విభజించు(స్థిరరాశి నిర్మాణం iovec *iov, size_t నిడివి_కొలత, నిర్మాణం iovec_wrapper *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(లోపల_సమితి(నిడివి_కొలత, 1, 2, 4));
        ప్రతిజ్ఞా(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        ఒకవేళ (!iovec_is_సమితి(iov)) {
                *ret = (నిర్మాణం iovec_wrapper) {};
                తిరిగి 0;
        }

        _cleanup_(iovw_పూర్తి_free) నిర్మాణం iovec_wrapper iovw = {};
        కొరకు (నిర్మాణం iovec i = *iov; iovec_is_సమితి(&i); ) {
                ఒకవేళ (i.iov_len < నిడివి_కొలత)
                        తిరిగి -EBADMSG;

                size_t len;
                ఎంపిక (నిడివి_కొలత) {
                స్థితి 1:
                        len = *(uint8_t*) i.iov_base;
                        ఆపు;
                స్థితి 2:
                        len = unaligned_read_be16(i.iov_base);
                        ఆపు;
                స్థితి 4:
                        len = unaligned_read_be32(i.iov_base);
                        ఆపు;
                అప్రమేయ:
                        assert_not_reached();
                }

                iovec_inc(&i, నిడివి_కొలత);

                ఒకవేళ (len == 0)
                        కొనసాగు;

                ఒకవేళ (i.iov_len < len)
                        తిరిగి -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        తిరిగి 0;
}

పూర్ణాంకం iovw_విలీనం(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw, size_t నిడివి_కొలత, నిర్మాణం iovec *ret) {
        ప్రతిజ్ఞా(లోపల_సమితి(నిడివి_కొలత, 1, 2, 4));
        ప్రతిజ్ఞా(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_కొలత(iovw);
        ఒకవేళ (sz == 0) {
                *ret = (నిర్మాణం iovec) {};
                తిరిగి 0;
        }
        ఒకవేళ (sz == SIZE_MAX)
                తిరిగి -E2BIG;

        ఒకవేళ (కొలత_multiply_అధికప్రవాహం(నిడివి_కొలత, iovw->లెక్క))
                తిరిగి -E2BIG;

        sz = కొలత_కూడు(sz, iovw->లెక్క * నిడివి_కొలత);
        ఒకవేళ (sz == SIZE_MAX)
                తిరిగి -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->లెక్క) {
                ఒకవేళ (iov->iov_len == 0)
                        కొనసాగు;

                ఎంపిక (నిడివి_కొలత) {
                స్థితి 1:
                        ఒకవేళ (iov->iov_len > UINT8_MAX)
                                తిరిగి -ERANGE;

                        *p = iov->iov_len;
                        ఆపు;
                స్థితి 2:
                        ఒకవేళ (iov->iov_len > UINT16_MAX)
                                తిరిగి -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        ఆపు;
                స్థితి 4:
                        ఒకవేళ (iov->iov_len > UINT32_MAX)
                                తిరిగి -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        ఆపు;
                అప్రమేయ:
                        assert_not_reached();
                }
                p += నిడివి_కొలత;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        ప్రతిజ్ఞా(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        తిరిగి 0;
}
