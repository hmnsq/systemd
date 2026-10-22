/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-wrapper.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"

ਖਾਲੀ iovw_ਪੂਰਾ(ਬਣਤਰ iovec_wrapper *iovw) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->ਗਿਣਤੀ_2 = 0;
}

ਖਾਲੀ iovw_ਪੂਰਾ_free(ਬਣਤਰ iovec_wrapper *iovw) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ਗਿਣਤੀ_2)
                iovec_ਪੂਰਾ(i);

        iovw_ਪੂਰਾ(iovw);
}

ਬਣਤਰ iovec_wrapper* iovw_free(ਬਣਤਰ iovec_wrapper *iovw) {
        ਜੇ (!iovw)
                ਵਾਪਸ ਨਿਰਾ;

        iovw_ਪੂਰਾ(iovw);
        ਵਾਪਸ mfree(iovw);
}

ਬਣਤਰ iovec_wrapper* iovw_free_free(ਬਣਤਰ iovec_wrapper *iovw) {
        ਜੇ (!iovw)
                ਵਾਪਸ ਨਿਰਾ;

        iovw_ਪੂਰਾ_free(iovw);
        ਵਾਪਸ mfree(iovw);
}

ਪੂਰਨ iovw_compare(ਅਚਲ ਬਣਤਰ iovec_wrapper *a, ਅਚਲ ਬਣਤਰ iovec_wrapper *b) {
        ਪੂਰਨ r;

        ਜੇ (a == b)
                ਵਾਪਸ 0;

        ਜੇ (!a || !b)
                ਵਾਪਸ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        ਲਈ (size_t i = 0, n = MIN(a->ਗਿਣਤੀ_2, b->ਗਿਣਤੀ_2); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                ਜੇ (r != 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ CMP(a->ਗਿਣਤੀ_2, b->ਗਿਣਤੀ_2);
}

ਪੂਰਨ iovw_put_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਖਾਲੀ *ਅੰਕੜੇ, size_t len) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ || len == 0);

        ਜੇ (len == 0 && !accept_zero)
                ਵਾਪਸ 0;

        ਜੇ (iovw->ਗਿਣਤੀ_2 >= IOV_MAX)
                ਵਾਪਸ -E2BIG;

        ਜੇ (!GREEDY_REALLOC(iovw->iovec, iovw->ਗਿਣਤੀ_2 + 1))
                ਵਾਪਸ -ENOMEM;

        iovw->iovec[iovw->ਗਿਣਤੀ_2++] = IOVEC_MAKE(ਅੰਕੜੇ, len);
        ਵਾਪਸ 1;
}

ਪੂਰਨ iovw_put_iov_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec *iov) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        ਜੇ (!iov)
                ਵਾਪਸ 0;

        ਵਾਪਸ iovw_put_ਭਰਿਆ(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ਪੂਰਨ iovw_put_iovw_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec_wrapper *ਸਰੋਤ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        ਜੇ (iovw_isempty(ਸਰੋਤ))
                ਵਾਪਸ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        ਜੇ (iovw == ਸਰੋਤ)
                ਵਾਪਸ -EINVAL;

        ਜੇ (iovw->ਗਿਣਤੀ_2 > SIZE_MAX - ਸਰੋਤ->ਗਿਣਤੀ_2)
                ਵਾਪਸ -E2BIG;
        ਜੇ (iovw->ਗਿਣਤੀ_2 + ਸਰੋਤ->ਗਿਣਤੀ_2 > IOV_MAX)
                ਵਾਪਸ -E2BIG;

        ਜੇ (accept_zero) {
                ਜੇ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->ਗਿਣਤੀ_2, ਸਰੋਤ->iovec, ਸਰੋਤ->ਗਿਣਤੀ_2))
                        ਵਾਪਸ -ENOMEM;

                ਵਾਪਸ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_ਗਿਣਤੀ = iovw->ਗਿਣਤੀ_2;

        FOREACH_ARRAY(iovec, ਸਰੋਤ->iovec, ਸਰੋਤ->ਗਿਣਤੀ_2) {
                r = iovw_put_iov_ਭਰਿਆ(iovw, accept_zero, iovec);
                ਜੇ (r < 0)
                        ਜਾਓ rollback;
        }

        ਵਾਪਸ 0;

rollback:
        iovw->ਗਿਣਤੀ_2 = original_ਗਿਣਤੀ;
        ਵਾਪਸ r;
}

ਪੂਰਨ iovw_consume_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਖਾਲੀ *ਅੰਕੜੇ, size_t len) {
        /* Move data into iovw or free on error */
        ਪੂਰਨ r;

        r = iovw_put_ਭਰਿਆ(iovw, accept_zero, ਅੰਕੜੇ, len);
        ਜੇ (r <= 0)
                ਮੁਕਤ(ਅੰਕੜੇ);

        ਵਾਪਸ r;
}

ਪੂਰਨ iovw_consume_iov_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਬਣਤਰ iovec *iov) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        ਜੇ (!iov)
                ਵਾਪਸ 0;

        r = iovw_put_iov_ਭਰਿਆ(iovw, accept_zero, iov);
        ਜੇ (r <= 0)
                iovec_ਪੂਰਾ(iov);
        ਨਹੀਂਤਾਂ
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (ਬਣਤਰ iovec) {};

        ਵਾਪਸ r;
}

ਪੂਰਨ iovw_extend_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t len) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ || len == 0);

        ਜੇ (len == 0)
                ਵਾਪਸ iovw_put_ਭਰਿਆ(iovw, accept_zero, /* data= */ ਨਿਰਾ, /* len= */ 0);

        ਖਾਲੀ *c = memdup(ਅੰਕੜੇ, len);
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ iovw_consume_ਭਰਿਆ(iovw, accept_zero, c, len);
}

ਪੂਰਨ iovw_extend_iov_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec *iov) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        ਜੇ (!iov)
                ਵਾਪਸ 0;

        ਵਾਪਸ iovw_extend_ਭਰਿਆ(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ਪੂਰਨ iovw_extend_iovw_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec_wrapper *ਸਰੋਤ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        /* This duplicates the source and merges it into the iovw. */

        ਜੇ (iovw_isempty(ਸਰੋਤ))
                ਵਾਪਸ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        ਜੇ (iovw == ਸਰੋਤ)
                ਵਾਪਸ -EINVAL;

        ਜੇ (iovw->ਗਿਣਤੀ_2 > SIZE_MAX - ਸਰੋਤ->ਗਿਣਤੀ_2)
                ਵਾਪਸ -E2BIG;
        ਜੇ (iovw->ਗਿਣਤੀ_2 + ਸਰੋਤ->ਗਿਣਤੀ_2 > IOV_MAX)
                ਵਾਪਸ -E2BIG;

        size_t original_ਗਿਣਤੀ = iovw->ਗਿਣਤੀ_2;

        FOREACH_ARRAY(iovec, ਸਰੋਤ->iovec, ਸਰੋਤ->ਗਿਣਤੀ_2) {
                r = iovw_extend_iov_ਭਰਿਆ(iovw, accept_zero, iovec);
                ਜੇ (r < 0)
                        ਜਾਓ rollback;
        }

        ਵਾਪਸ 0;

rollback:
        ਲਈ (size_t i = original_ਗਿਣਤੀ; i < iovw->ਗਿਣਤੀ_2; i++)
                iovec_ਪੂਰਾ(iovw->iovec + i);

        iovw->ਗਿਣਤੀ_2 = original_ਗਿਣਤੀ;
        ਵਾਪਸ r;
}

ਪੂਰਨ iovw_put_string_ਖੇਤਰ_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool replace, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        _cleanup_free_ ਅੱਖਰ *x = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        x = strjoin(ਖੇਤਰ, ਮੁੱਲ);
        ਜੇ (!x)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->ਗਿਣਤੀ_2)
                        ਜੇ (ਸਮ੍ਰਿਤੀ_startswith(iovec->iov_base, iovec->iov_len, ਖੇਤਰ)) {
                                iovec->iov_len = ਲੜੀ_ਲੰਬਾਈ(x);
                                ਵਾਪਸ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, ਲੜੀ_ਲੰਬਾਈ(x));
        ਜੇ (r >= 0)
                TAKE_PTR(x);

        ਵਾਪਸ r;
}

ਪੂਰਨ iovw_put_string_fieldf_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool replace, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *format, ...) {
        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        va_list ap;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(format);

        va_start(ap, format);
        r = vasprintf(&ਮੁੱਲ, format, ap);
        va_end(ap);
        ਜੇ (r < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ iovw_put_string_ਖੇਤਰ_ਭਰਿਆ(iovw, replace, ਖੇਤਰ, ਮੁੱਲ);
}

ਪੂਰਨ iovw_put_string_ਖੇਤਰ_free(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅੱਖਰ *ਮੁੱਲ) {
        _cleanup_free_ ਅਣਵਰਤਿਆ ਅੱਖਰ *free_ptr = ਮੁੱਲ;

        ਵਾਪਸ iovw_put_string_ਖੇਤਰ(iovw, ਖੇਤਰ, ਮੁੱਲ);
}

ਖਾਲੀ iovw_rebase(ਬਣਤਰ iovec_wrapper *iovw, ਖਾਲੀ *old, ਖਾਲੀ *new) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ਗਿਣਤੀ_2) {
                ਪ੍ਰਤਿਜ੍ਞਾ(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_ਮਾਪ(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw) {
        ਜੇ (iovw_isempty(iovw))
                ਵਾਪਸ 0;

        ਵਾਪਸ iovec_ਕੁੱਲ_ਮਾਪ(iovw->iovec, iovw->ਗਿਣਤੀ_2);
}

ਪੂਰਨ iovw_concat(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw, ਬਣਤਰ iovec *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_ਮਾਪ(iovw);
        ਜੇ (len == SIZE_MAX)
                ਵਾਪਸ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(len + 1);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->ਗਿਣਤੀ_2)
                p = mempcpy_ਸੁਰੱਖਿਅਤ(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ਵਾਪਸ 0;
}

ਅੱਖਰ* iovw_ਮੰਜ਼ਿਲ_cstring(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_ਪੂਰਾ) ਬਣਤਰ iovec iov = {};
        ਜੇ (iovw_concat(iovw, &iov) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(!memchr(iov.iov_base, 0, iov.iov_len));
        ਵਾਪਸ TAKE_PTR(iov.iov_base);
}

ਪੂਰਨ iovec_ਵੰਡੋ(ਅਚਲ ਬਣਤਰ iovec *iov, size_t ਲੰਮਾਈ_ਮਾਪ, ਬਣਤਰ iovec_wrapper *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(ਲੰਮਾਈ_ਮਾਪ, 1, 2, 4));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        ਜੇ (!iovec_is_ਸੈੱਟ(iov)) {
                *ret = (ਬਣਤਰ iovec_wrapper) {};
                ਵਾਪਸ 0;
        }

        _cleanup_(iovw_ਪੂਰਾ_free) ਬਣਤਰ iovec_wrapper iovw = {};
        ਲਈ (ਬਣਤਰ iovec i = *iov; iovec_is_ਸੈੱਟ(&i); ) {
                ਜੇ (i.iov_len < ਲੰਮਾਈ_ਮਾਪ)
                        ਵਾਪਸ -EBADMSG;

                size_t len;
                ਚੋਣ (ਲੰਮਾਈ_ਮਾਪ) {
                ਹਾਲਤ 1:
                        len = *(uint8_t*) i.iov_base;
                        ਰੋਕੋ;
                ਹਾਲਤ 2:
                        len = unaligned_read_be16(i.iov_base);
                        ਰੋਕੋ;
                ਹਾਲਤ 4:
                        len = unaligned_read_be32(i.iov_base);
                        ਰੋਕੋ;
                ਮੂਲ:
                        assert_not_reached();
                }

                iovec_inc(&i, ਲੰਮਾਈ_ਮਾਪ);

                ਜੇ (len == 0)
                        ਜਾਰੀ;

                ਜੇ (i.iov_len < len)
                        ਵਾਪਸ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ਵਾਪਸ 0;
}

ਪੂਰਨ iovw_ਮਿਲਾਓ(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw, size_t ਲੰਮਾਈ_ਮਾਪ, ਬਣਤਰ iovec *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(ਲੰਮਾਈ_ਮਾਪ, 1, 2, 4));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_ਮਾਪ(iovw);
        ਜੇ (sz == 0) {
                *ret = (ਬਣਤਰ iovec) {};
                ਵਾਪਸ 0;
        }
        ਜੇ (sz == SIZE_MAX)
                ਵਾਪਸ -E2BIG;

        ਜੇ (ਮਾਪ_multiply_ਉਛਾਲ(ਲੰਮਾਈ_ਮਾਪ, iovw->ਗਿਣਤੀ_2))
                ਵਾਪਸ -E2BIG;

        sz = ਮਾਪ_ਜੋੜੋ(sz, iovw->ਗਿਣਤੀ_2 * ਲੰਮਾਈ_ਮਾਪ);
        ਜੇ (sz == SIZE_MAX)
                ਵਾਪਸ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->ਗਿਣਤੀ_2) {
                ਜੇ (iov->iov_len == 0)
                        ਜਾਰੀ;

                ਚੋਣ (ਲੰਮਾਈ_ਮਾਪ) {
                ਹਾਲਤ 1:
                        ਜੇ (iov->iov_len > UINT8_MAX)
                                ਵਾਪਸ -ERANGE;

                        *p = iov->iov_len;
                        ਰੋਕੋ;
                ਹਾਲਤ 2:
                        ਜੇ (iov->iov_len > UINT16_MAX)
                                ਵਾਪਸ -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        ਰੋਕੋ;
                ਹਾਲਤ 4:
                        ਜੇ (iov->iov_len > UINT32_MAX)
                                ਵਾਪਸ -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        ਰੋਕੋ;
                ਮੂਲ:
                        assert_not_reached();
                }
                p += ਲੰਮਾਈ_ਮਾਪ;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        ਵਾਪਸ 0;
}
