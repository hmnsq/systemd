/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "iovec-util.h"
#অন্তর্ভাৱ "iovec-wrapper.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "unaligned.h"

শূন্য iovw_done(সংরচনা iovec_wrapper *iovw) {
        প্রতিজ্ঞা(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->গুনতি = 0;
}

শূন্য iovw_done_free(সংরচনা iovec_wrapper *iovw) {
        প্রতিজ্ঞা(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->গুনতি)
                iovec_done(i);

        iovw_done(iovw);
}

সংরচনা iovec_wrapper* iovw_free(সংরচনা iovec_wrapper *iovw) {
        যদি (!iovw)
                ঘূৰাই রিক্ত;

        iovw_done(iovw);
        ঘূৰাই mfree(iovw);
}

সংরচনা iovec_wrapper* iovw_free_free(সংরচনা iovec_wrapper *iovw) {
        যদি (!iovw)
                ঘূৰাই রিক্ত;

        iovw_done_free(iovw);
        ঘূৰাই mfree(iovw);
}

পূর্ণাঙ্ক iovw_compare(ধ্রুৱ সংরচনা iovec_wrapper *a, ধ্রুৱ সংরচনা iovec_wrapper *b) {
        পূর্ণাঙ্ক r;

        যদি (a == b)
                ঘূৰাই 0;

        যদি (!a || !b)
                ঘূৰাই CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        বাবে (size_t i = 0, n = MIN(a->গুনতি, b->গুনতি); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                যদি (r != 0)
                        ঘূৰাই r;
        }

        ঘূৰাই CMP(a->গুনতি, b->গুনতি);
}

পূর্ণাঙ্ক iovw_put_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, শূন্য *data, size_t len) {
        প্রতিজ্ঞা(iovw);
        প্রতিজ্ঞা(data || len == 0);

        যদি (len == 0 && !accept_zero)
                ঘূৰাই 0;

        যদি (iovw->গুনতি >= IOV_MAX)
                ঘূৰাই -E2BIG;

        যদি (!GREEDY_REALLOC(iovw->iovec, iovw->গুনতি + 1))
                ঘূৰাই -ENOMEM;

        iovw->iovec[iovw->গুনতি++] = IOVEC_MAKE(data, len);
        ঘূৰাই 1;
}

পূর্ণাঙ্ক iovw_put_iov_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুৱ সংরচনা iovec *iov) {
        প্রতিজ্ঞা(iovw);

        যদি (!iov)
                ঘূৰাই 0;

        ঘূৰাই iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

পূর্ণাঙ্ক iovw_put_iovw_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুৱ সংরচনা iovec_wrapper *source) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(iovw);

        যদি (iovw_isempty(source))
                ঘূৰাই 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        যদি (iovw == source)
                ঘূৰাই -EINVAL;

        যদি (iovw->গুনতি > SIZE_MAX - source->গুনতি)
                ঘূৰাই -E2BIG;
        যদি (iovw->গুনতি + source->গুনতি > IOV_MAX)
                ঘূৰাই -E2BIG;

        যদি (accept_zero) {
                যদি (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->গুনতি, source->iovec, source->গুনতি))
                        ঘূৰাই -ENOMEM;

                ঘূৰাই 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_গুনতি = iovw->গুনতি;

        FOREACH_ARRAY(iovec, source->iovec, source->গুনতি) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                যদি (r < 0)
                        গচ্ছ rollback;
        }

        ঘূৰাই 0;

rollback:
        iovw->গুনতি = original_গুনতি;
        ঘূৰাই r;
}

পূর্ণাঙ্ক iovw_consume_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, শূন্য *data, size_t len) {
        /* Move data into iovw or free on error */
        পূর্ণাঙ্ক r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        যদি (r <= 0)
                মুক্ত(data);

        ঘূৰাই r;
}

পূর্ণাঙ্ক iovw_consume_iov_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, সংরচনা iovec *iov) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(iovw);

        যদি (!iov)
                ঘূৰাই 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        যদি (r <= 0)
                iovec_done(iov);
        অন্যথা
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (সংরচনা iovec) {};

        ঘূৰাই r;
}

পূর্ণাঙ্ক iovw_extend_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুৱ শূন্য *data, size_t len) {
        প্রতিজ্ঞা(iovw);
        প্রতিজ্ঞা(data || len == 0);

        যদি (len == 0)
                ঘূৰাই iovw_put_full(iovw, accept_zero, /* data= */ রিক্ত, /* len= */ 0);

        শূন্য *c = memdup(data, len);
        যদি (!c)
                ঘূৰাই -ENOMEM;

        ঘূৰাই iovw_consume_full(iovw, accept_zero, c, len);
}

পূর্ণাঙ্ক iovw_extend_iov_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুৱ সংরচনা iovec *iov) {
        প্রতিজ্ঞা(iovw);

        যদি (!iov)
                ঘূৰাই 0;

        ঘূৰাই iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

পূর্ণাঙ্ক iovw_extend_iovw_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুৱ সংরচনা iovec_wrapper *source) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(iovw);

        /* This duplicates the source and merges it into the iovw. */

        যদি (iovw_isempty(source))
                ঘূৰাই 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        যদি (iovw == source)
                ঘূৰাই -EINVAL;

        যদি (iovw->গুনতি > SIZE_MAX - source->গুনতি)
                ঘূৰাই -E2BIG;
        যদি (iovw->গুনতি + source->গুনতি > IOV_MAX)
                ঘূৰাই -E2BIG;

        size_t original_গুনতি = iovw->গুনতি;

        FOREACH_ARRAY(iovec, source->iovec, source->গুনতি) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                যদি (r < 0)
                        গচ্ছ rollback;
        }

        ঘূৰাই 0;

rollback:
        বাবে (size_t i = original_গুনতি; i < iovw->গুনতি; i++)
                iovec_done(iovw->iovec + i);

        iovw->গুনতি = original_গুনতি;
        ঘূৰাই r;
}

পূর্ণাঙ্ক iovw_put_string_field_full(সংরচনা iovec_wrapper *iovw, bool replace, ধ্রুৱ ৱর্ণ *field, ধ্রুৱ ৱর্ণ *মান) {
        _cleanup_free_ ৱর্ণ *x = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(iovw);

        x = strjoin(field, মান);
        যদি (!x)
                ঘূৰাই -ENOMEM;

        যদি (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->গুনতি)
                        যদি (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                ঘূৰাই free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        যদি (r >= 0)
                TAKE_PTR(x);

        ঘূৰাই r;
}

পূর্ণাঙ্ক iovw_put_string_fieldf_full(সংরচনা iovec_wrapper *iovw, bool replace, ধ্রুৱ ৱর্ণ *field, ধ্রুৱ ৱর্ণ *format, ...) {
        _cleanup_free_ ৱর্ণ *মান = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(format);

        va_start(ap, format);
        r = vasprintf(&মান, format, ap);
        va_end(ap);
        যদি (r < 0)
                ঘূৰাই -ENOMEM;

        ঘূৰাই iovw_put_string_field_full(iovw, replace, field, মান);
}

পূর্ণাঙ্ক iovw_put_string_field_free(সংরচনা iovec_wrapper *iovw, ধ্রুৱ ৱর্ণ *field, ৱর্ণ *মান) {
        _cleanup_free_ _unused_ ৱর্ণ *free_ptr = মান;

        ঘূৰাই iovw_put_string_field(iovw, field, মান);
}

শূন্য iovw_rebase(সংরচনা iovec_wrapper *iovw, শূন্য *old, শূন্য *new) {
        প্রতিজ্ঞা(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->গুনতি) {
                প্রতিজ্ঞা(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ধ্রুৱ সংরচনা iovec_wrapper *iovw) {
        যদি (iovw_isempty(iovw))
                ঘূৰাই 0;

        ঘূৰাই iovec_total_size(iovw->iovec, iovw->গুনতি);
}

পূর্ণাঙ্ক iovw_concat(ধ্রুৱ সংরচনা iovec_wrapper *iovw, সংরচনা iovec *ret) {
        প্রতিজ্ঞা(iovw);
        প্রতিজ্ঞা(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        যদি (len == SIZE_MAX)
                ঘূৰাই -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        যদি (!buf)
                ঘূৰাই -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->গুনতি)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ঘূৰাই 0;
}

ৱর্ণ* iovw_to_cstring(ধ্রুৱ সংরচনা iovec_wrapper *iovw) {
        প্রতিজ্ঞা(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) সংরচনা iovec iov = {};
        যদি (iovw_concat(iovw, &iov) < 0)
                ঘূৰাই রিক্ত;

        প্রতিজ্ঞা(!memchr(iov.iov_base, 0, iov.iov_len));
        ঘূৰাই TAKE_PTR(iov.iov_base);
}

পূর্ণাঙ্ক iovec_split(ধ্রুৱ সংরচনা iovec *iov, size_t length_size, সংরচনা iovec_wrapper *ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(IN_SET(length_size, 1, 2, 4));
        প্রতিজ্ঞা(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        যদি (!iovec_is_set(iov)) {
                *ret = (সংরচনা iovec_wrapper) {};
                ঘূৰাই 0;
        }

        _cleanup_(iovw_done_free) সংরচনা iovec_wrapper iovw = {};
        বাবে (সংরচনা iovec i = *iov; iovec_is_set(&i); ) {
                যদি (i.iov_len < length_size)
                        ঘূৰাই -EBADMSG;

                size_t len;
                ৱিকল্প (length_size) {
                পক্ষ 1:
                        len = *(uint8_t*) i.iov_base;
                        ভঙ্গ;
                পক্ষ 2:
                        len = unaligned_read_be16(i.iov_base);
                        ভঙ্গ;
                পক্ষ 4:
                        len = unaligned_read_be32(i.iov_base);
                        ভঙ্গ;
                স্ৱভাৱ:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                যদি (len == 0)
                        অনুৱর্ত;

                যদি (i.iov_len < len)
                        ঘূৰাই -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                যদি (r < 0)
                        ঘূৰাই r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক iovw_merge(ধ্রুৱ সংরচনা iovec_wrapper *iovw, size_t length_size, সংরচনা iovec *ret) {
        প্রতিজ্ঞা(IN_SET(length_size, 1, 2, 4));
        প্রতিজ্ঞা(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        যদি (sz == 0) {
                *ret = (সংরচনা iovec) {};
                ঘূৰাই 0;
        }
        যদি (sz == SIZE_MAX)
                ঘূৰাই -E2BIG;

        যদি (size_multiply_overflow(length_size, iovw->গুনতি))
                ঘূৰাই -E2BIG;

        sz = size_add(sz, iovw->গুনতি * length_size);
        যদি (sz == SIZE_MAX)
                ঘূৰাই -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        যদি (!buf)
                ঘূৰাই -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->গুনতি) {
                যদি (iov->iov_len == 0)
                        অনুৱর্ত;

                ৱিকল্প (length_size) {
                পক্ষ 1:
                        যদি (iov->iov_len > UINT8_MAX)
                                ঘূৰাই -ERANGE;

                        *p = iov->iov_len;
                        ভঙ্গ;
                পক্ষ 2:
                        যদি (iov->iov_len > UINT16_MAX)
                                ঘূৰাই -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        ভঙ্গ;
                পক্ষ 4:
                        যদি (iov->iov_len > UINT32_MAX)
                                ঘূৰাই -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        ভঙ্গ;
                স্ৱভাৱ:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        প্রতিজ্ঞা(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        ঘূৰাই 0;
}
