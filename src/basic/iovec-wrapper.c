/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-wrapper.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

ಶೂನ್ಯ iovw_done(ಸಂರಚನಾ iovec_wrapper *iovw) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->ಎಣಿಕೆ = 0;
}

ಶೂನ್ಯ iovw_done_free(ಸಂರಚನಾ iovec_wrapper *iovw) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ಎಣಿಕೆ)
                iovec_done(i);

        iovw_done(iovw);
}

ಸಂರಚನಾ iovec_wrapper* iovw_free(ಸಂರಚನಾ iovec_wrapper *iovw) {
        ಯದಿ (!iovw)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        iovw_done(iovw);
        ಪ್ರತ್ಯಾಗಮ mfree(iovw);
}

ಸಂರಚನಾ iovec_wrapper* iovw_free_free(ಸಂರಚನಾ iovec_wrapper *iovw) {
        ಯದಿ (!iovw)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        iovw_done_free(iovw);
        ಪ್ರತ್ಯಾಗಮ mfree(iovw);
}

ಪೂರ್ಣಾಙ್ಕ iovw_compare(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *a, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (a == b)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!a || !b)
                ಪ್ರತ್ಯಾಗಮ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        ಪ್ರತಿ (size_t i = 0, n = MIN(a->ಎಣಿಕೆ, b->ಎಣಿಕೆ); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                ಯದಿ (r != 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ CMP(a->ಎಣಿಕೆ, b->ಎಣಿಕೆ);
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *data, size_t len) {
        ಪ್ರತಿಜ್ಞಾ(iovw);
        ಪ್ರತಿಜ್ಞಾ(data || len == 0);

        ಯದಿ (len == 0 && !accept_zero)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (iovw->ಎಣಿಕೆ >= IOV_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        ಯದಿ (!GREEDY_REALLOC(iovw->iovec, iovw->ಎಣಿಕೆ + 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        iovw->iovec[iovw->ಎಣಿಕೆ++] = IOVEC_MAKE(data, len);
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_iov_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec *iov) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಯದಿ (!iov)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_iovw_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *source) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಯದಿ (iovw_isempty(source))
                ಪ್ರತ್ಯಾಗಮ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        ಯದಿ (iovw == source)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (iovw->ಎಣಿಕೆ > SIZE_MAX - source->ಎಣಿಕೆ)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;
        ಯದಿ (iovw->ಎಣಿಕೆ + source->ಎಣಿಕೆ > IOV_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        ಯದಿ (accept_zero) {
                ಯದಿ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->ಎಣಿಕೆ, source->iovec, source->ಎಣಿಕೆ))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_ಎಣಿಕೆ = iovw->ಎಣಿಕೆ;

        FOREACH_ARRAY(iovec, source->iovec, source->ಎಣಿಕೆ) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ rollback;
        }

        ಪ್ರತ್ಯಾಗಮ 0;

rollback:
        iovw->ಎಣಿಕೆ = original_ಎಣಿಕೆ;
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ iovw_consume_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *data, size_t len) {
        /* Move data into iovw or free on error */
        ಪೂರ್ಣಾಙ್ಕ r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        ಯದಿ (r <= 0)
                ಮುಕ್ತ(data);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ iovw_consume_iov_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಸಂರಚನಾ iovec *iov) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಯದಿ (!iov)
                ಪ್ರತ್ಯಾಗಮ 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        ಯದಿ (r <= 0)
                iovec_done(iov);
        ಅನ್ಯಥಾ
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (ಸಂರಚನಾ iovec) {};

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ iovw_extend_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಶೂನ್ಯ *data, size_t len) {
        ಪ್ರತಿಜ್ಞಾ(iovw);
        ಪ್ರತಿಜ್ಞಾ(data || len == 0);

        ಯದಿ (len == 0)
                ಪ್ರತ್ಯಾಗಮ iovw_put_full(iovw, accept_zero, /* data= */ ರಿಕ್ತ, /* len= */ 0);

        ಶೂನ್ಯ *c = memdup(data, len);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ iovw_consume_full(iovw, accept_zero, c, len);
}

ಪೂರ್ಣಾಙ್ಕ iovw_extend_iov_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec *iov) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಯದಿ (!iov)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ಪೂರ್ಣಾಙ್ಕ iovw_extend_iovw_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *source) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        /* This duplicates the source and merges it into the iovw. */

        ಯದಿ (iovw_isempty(source))
                ಪ್ರತ್ಯಾಗಮ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        ಯದಿ (iovw == source)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (iovw->ಎಣಿಕೆ > SIZE_MAX - source->ಎಣಿಕೆ)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;
        ಯದಿ (iovw->ಎಣಿಕೆ + source->ಎಣಿಕೆ > IOV_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        size_t original_ಎಣಿಕೆ = iovw->ಎಣಿಕೆ;

        FOREACH_ARRAY(iovec, source->iovec, source->ಎಣಿಕೆ) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ rollback;
        }

        ಪ್ರತ್ಯಾಗಮ 0;

rollback:
        ಪ್ರತಿ (size_t i = original_ಎಣಿಕೆ; i < iovw->ಎಣಿಕೆ; i++)
                iovec_done(iovw->iovec + i);

        iovw->ಎಣಿಕೆ = original_ಎಣಿಕೆ;
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_string_field_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool replace, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        _cleanup_free_ ವರ್ಣ *x = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        x = strjoin(field, ಮೌಲ್ಯ);
        ಯದಿ (!x)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->ಎಣಿಕೆ)
                        ಯದಿ (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                ಪ್ರತ್ಯಾಗಮ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        ಯದಿ (r >= 0)
                TAKE_PTR(x);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_string_fieldf_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool replace, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *format, ...) {
        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(format);

        va_start(ap, format);
        r = vasprintf(&ಮೌಲ್ಯ, format, ap);
        va_end(ap);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ iovw_put_string_field_full(iovw, replace, field, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_string_field_free(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ವರ್ಣ *field, ವರ್ಣ *ಮೌಲ್ಯ) {
        _cleanup_free_ _unused_ ವರ್ಣ *free_ptr = ಮೌಲ್ಯ;

        ಪ್ರತ್ಯಾಗಮ iovw_put_string_field(iovw, field, ಮೌಲ್ಯ);
}

ಶೂನ್ಯ iovw_rebase(ಸಂರಚನಾ iovec_wrapper *iovw, ಶೂನ್ಯ *old, ಶೂನ್ಯ *new) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ಎಣಿಕೆ) {
                ಪ್ರತಿಜ್ಞಾ(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw) {
        ಯದಿ (iovw_isempty(iovw))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ iovec_total_size(iovw->iovec, iovw->ಎಣಿಕೆ);
}

ಪೂರ್ಣಾಙ್ಕ iovw_concat(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw, ಸಂರಚನಾ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(iovw);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        ಯದಿ (len == SIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->ಎಣಿಕೆ)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* iovw_to_cstring(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) ಸಂರಚನಾ iovec iov = {};
        ಯದಿ (iovw_concat(iovw, &iov) < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(!memchr(iov.iov_base, 0, iov.iov_len));
        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(iov.iov_base);
}

ಪೂರ್ಣಾಙ್ಕ iovec_split(ಧ್ರುವ ಸಂರಚನಾ iovec *iov, size_t length_size, ಸಂರಚನಾ iovec_wrapper *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(IN_SET(length_size, 1, 2, 4));
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        ಯದಿ (!iovec_is_set(iov)) {
                *ret = (ಸಂರಚನಾ iovec_wrapper) {};
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_(iovw_done_free) ಸಂರಚನಾ iovec_wrapper iovw = {};
        ಪ್ರತಿ (ಸಂರಚನಾ iovec i = *iov; iovec_is_set(&i); ) {
                ಯದಿ (i.iov_len < length_size)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                size_t len;
                ವಿಕಲ್ಪ (length_size) {
                ಪಕ್ಷ 1:
                        len = *(uint8_t*) i.iov_base;
                        ಭಙ್ಗ;
                ಪಕ್ಷ 2:
                        len = unaligned_read_be16(i.iov_base);
                        ಭಙ್ಗ;
                ಪಕ್ಷ 4:
                        len = unaligned_read_be32(i.iov_base);
                        ಭಙ್ಗ;
                ಸ್ವಭಾವ:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                ಯದಿ (len == 0)
                        ಅನುವರ್ತ;

                ಯದಿ (i.iov_len < len)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ iovw_merge(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw, size_t length_size, ಸಂರಚನಾ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(IN_SET(length_size, 1, 2, 4));
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        ಯದಿ (sz == 0) {
                *ret = (ಸಂರಚನಾ iovec) {};
                ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (sz == SIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        ಯದಿ (size_multiply_overflow(length_size, iovw->ಎಣಿಕೆ))
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        sz = size_add(sz, iovw->ಎಣಿಕೆ * length_size);
        ಯದಿ (sz == SIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->ಎಣಿಕೆ) {
                ಯದಿ (iov->iov_len == 0)
                        ಅನುವರ್ತ;

                ವಿಕಲ್ಪ (length_size) {
                ಪಕ್ಷ 1:
                        ಯದಿ (iov->iov_len > UINT8_MAX)
                                ಪ್ರತ್ಯಾಗಮ -ERANGE;

                        *p = iov->iov_len;
                        ಭಙ್ಗ;
                ಪಕ್ಷ 2:
                        ಯದಿ (iov->iov_len > UINT16_MAX)
                                ಪ್ರತ್ಯಾಗಮ -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        ಭಙ್ಗ;
                ಪಕ್ಷ 4:
                        ಯದಿ (iov->iov_len > UINT32_MAX)
                                ಪ್ರತ್ಯಾಗಮ -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        ಭಙ್ಗ;
                ಸ್ವಭಾವ:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        ಪ್ರತಿಜ್ಞಾ(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        ಪ್ರತ್ಯಾಗಮ 0;
}
