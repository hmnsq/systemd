/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "iovec-wrapper.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "unaligned.h"

வெறுமை iovw_done(கட்டமைப்பு iovec_wrapper *iovw) {
        ப்ரதிஜ்ஞா(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->மொத்தம் = 0;
}

வெறுமை iovw_done_free(கட்டமைப்பு iovec_wrapper *iovw) {
        ப்ரதிஜ்ஞா(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->மொத்தம்)
                iovec_done(i);

        iovw_done(iovw);
}

கட்டமைப்பு iovec_wrapper* iovw_free(கட்டமைப்பு iovec_wrapper *iovw) {
        எனில் (!iovw)
                திரும்பு வெற்று;

        iovw_done(iovw);
        திரும்பு mfree(iovw);
}

கட்டமைப்பு iovec_wrapper* iovw_free_free(கட்டமைப்பு iovec_wrapper *iovw) {
        எனில் (!iovw)
                திரும்பு வெற்று;

        iovw_done_free(iovw);
        திரும்பு mfree(iovw);
}

முழு iovw_compare(மாறிலி கட்டமைப்பு iovec_wrapper *a, மாறிலி கட்டமைப்பு iovec_wrapper *b) {
        முழு r;

        எனில் (a == b)
                திரும்பு 0;

        எனில் (!a || !b)
                திரும்பு CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        க்கு (size_t i = 0, n = MIN(a->மொத்தம், b->மொத்தம்); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                எனில் (r != 0)
                        திரும்பு r;
        }

        திரும்பு CMP(a->மொத்தம், b->மொத்தம்);
}

முழு iovw_put_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, வெறுமை *data, size_t len) {
        ப்ரதிஜ்ஞா(iovw);
        ப்ரதிஜ்ஞா(data || len == 0);

        எனில் (len == 0 && !accept_zero)
                திரும்பு 0;

        எனில் (iovw->மொத்தம் >= IOV_MAX)
                திரும்பு -E2BIG;

        எனில் (!GREEDY_REALLOC(iovw->iovec, iovw->மொத்தம் + 1))
                திரும்பு -ENOMEM;

        iovw->iovec[iovw->மொத்தம்++] = IOVEC_MAKE(data, len);
        திரும்பு 1;
}

முழு iovw_put_iov_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec *iov) {
        ப்ரதிஜ்ஞா(iovw);

        எனில் (!iov)
                திரும்பு 0;

        திரும்பு iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

முழு iovw_put_iovw_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec_wrapper *source) {
        முழு r;

        ப்ரதிஜ்ஞா(iovw);

        எனில் (iovw_isempty(source))
                திரும்பு 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        எனில் (iovw == source)
                திரும்பு -EINVAL;

        எனில் (iovw->மொத்தம் > SIZE_MAX - source->மொத்தம்)
                திரும்பு -E2BIG;
        எனில் (iovw->மொத்தம் + source->மொத்தம் > IOV_MAX)
                திரும்பு -E2BIG;

        எனில் (accept_zero) {
                எனில் (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->மொத்தம், source->iovec, source->மொத்தம்))
                        திரும்பு -ENOMEM;

                திரும்பு 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_மொத்தம் = iovw->மொத்தம்;

        FOREACH_ARRAY(iovec, source->iovec, source->மொத்தம்) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                எனில் (r < 0)
                        செல் rollback;
        }

        திரும்பு 0;

rollback:
        iovw->மொத்தம் = original_மொத்தம்;
        திரும்பு r;
}

முழு iovw_consume_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, வெறுமை *data, size_t len) {
        /* Move data into iovw or free on error */
        முழு r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        எனில் (r <= 0)
                விடுவி(data);

        திரும்பு r;
}

முழு iovw_consume_iov_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, கட்டமைப்பு iovec *iov) {
        முழு r;

        ப்ரதிஜ்ஞா(iovw);

        எனில் (!iov)
                திரும்பு 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        எனில் (r <= 0)
                iovec_done(iov);
        இல்லையெனில்
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (கட்டமைப்பு iovec) {};

        திரும்பு r;
}

முழு iovw_extend_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி வெறுமை *data, size_t len) {
        ப்ரதிஜ்ஞா(iovw);
        ப்ரதிஜ்ஞா(data || len == 0);

        எனில் (len == 0)
                திரும்பு iovw_put_full(iovw, accept_zero, /* data= */ வெற்று, /* len= */ 0);

        வெறுமை *c = memdup(data, len);
        எனில் (!c)
                திரும்பு -ENOMEM;

        திரும்பு iovw_consume_full(iovw, accept_zero, c, len);
}

முழு iovw_extend_iov_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec *iov) {
        ப்ரதிஜ்ஞா(iovw);

        எனில் (!iov)
                திரும்பு 0;

        திரும்பு iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

முழு iovw_extend_iovw_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec_wrapper *source) {
        முழு r;

        ப்ரதிஜ்ஞா(iovw);

        /* This duplicates the source and merges it into the iovw. */

        எனில் (iovw_isempty(source))
                திரும்பு 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        எனில் (iovw == source)
                திரும்பு -EINVAL;

        எனில் (iovw->மொத்தம் > SIZE_MAX - source->மொத்தம்)
                திரும்பு -E2BIG;
        எனில் (iovw->மொத்தம் + source->மொத்தம் > IOV_MAX)
                திரும்பு -E2BIG;

        size_t original_மொத்தம் = iovw->மொத்தம்;

        FOREACH_ARRAY(iovec, source->iovec, source->மொத்தம்) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                எனில் (r < 0)
                        செல் rollback;
        }

        திரும்பு 0;

rollback:
        க்கு (size_t i = original_மொத்தம்; i < iovw->மொத்தம்; i++)
                iovec_done(iovw->iovec + i);

        iovw->மொத்தம் = original_மொத்தம்;
        திரும்பு r;
}

முழு iovw_put_string_field_full(கட்டமைப்பு iovec_wrapper *iovw, bool replace, மாறிலி எழுத்து *field, மாறிலி எழுத்து *மதிப்பு) {
        _cleanup_free_ எழுத்து *x = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(iovw);

        x = strjoin(field, மதிப்பு);
        எனில் (!x)
                திரும்பு -ENOMEM;

        எனில் (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->மொத்தம்)
                        எனில் (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                திரும்பு free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        எனில் (r >= 0)
                TAKE_PTR(x);

        திரும்பு r;
}

முழு iovw_put_string_fieldf_full(கட்டமைப்பு iovec_wrapper *iovw, bool replace, மாறிலி எழுத்து *field, மாறிலி எழுத்து *format, ...) {
        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        va_list ap;
        முழு r;

        ப்ரதிஜ்ஞா(format);

        va_start(ap, format);
        r = vasprintf(&மதிப்பு, format, ap);
        va_end(ap);
        எனில் (r < 0)
                திரும்பு -ENOMEM;

        திரும்பு iovw_put_string_field_full(iovw, replace, field, மதிப்பு);
}

முழு iovw_put_string_field_free(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி எழுத்து *field, எழுத்து *மதிப்பு) {
        _cleanup_free_ _unused_ எழுத்து *free_ptr = மதிப்பு;

        திரும்பு iovw_put_string_field(iovw, field, மதிப்பு);
}

வெறுமை iovw_rebase(கட்டமைப்பு iovec_wrapper *iovw, வெறுமை *old, வெறுமை *new) {
        ப்ரதிஜ்ஞா(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->மொத்தம்) {
                ப்ரதிஜ்ஞா(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(மாறிலி கட்டமைப்பு iovec_wrapper *iovw) {
        எனில் (iovw_isempty(iovw))
                திரும்பு 0;

        திரும்பு iovec_total_size(iovw->iovec, iovw->மொத்தம்);
}

முழு iovw_concat(மாறிலி கட்டமைப்பு iovec_wrapper *iovw, கட்டமைப்பு iovec *ret) {
        ப்ரதிஜ்ஞா(iovw);
        ப்ரதிஜ்ஞா(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        எனில் (len == SIZE_MAX)
                திரும்பு -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->மொத்தம்)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        திரும்பு 0;
}

எழுத்து* iovw_to_cstring(மாறிலி கட்டமைப்பு iovec_wrapper *iovw) {
        ப்ரதிஜ்ஞா(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) கட்டமைப்பு iovec iov = {};
        எனில் (iovw_concat(iovw, &iov) < 0)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(!memchr(iov.iov_base, 0, iov.iov_len));
        திரும்பு TAKE_PTR(iov.iov_base);
}

முழு iovec_split(மாறிலி கட்டமைப்பு iovec *iov, size_t length_size, கட்டமைப்பு iovec_wrapper *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(IN_SET(length_size, 1, 2, 4));
        ப்ரதிஜ்ஞா(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        எனில் (!iovec_is_set(iov)) {
                *ret = (கட்டமைப்பு iovec_wrapper) {};
                திரும்பு 0;
        }

        _cleanup_(iovw_done_free) கட்டமைப்பு iovec_wrapper iovw = {};
        க்கு (கட்டமைப்பு iovec i = *iov; iovec_is_set(&i); ) {
                எனில் (i.iov_len < length_size)
                        திரும்பு -EBADMSG;

                size_t len;
                தேர்வு (length_size) {
                நிலை 1:
                        len = *(uint8_t*) i.iov_base;
                        நிறுத்து;
                நிலை 2:
                        len = unaligned_read_be16(i.iov_base);
                        நிறுத்து;
                நிலை 4:
                        len = unaligned_read_be32(i.iov_base);
                        நிறுத்து;
                இயல்பு:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                எனில் (len == 0)
                        தொடர்;

                எனில் (i.iov_len < len)
                        திரும்பு -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                எனில் (r < 0)
                        திரும்பு r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        திரும்பு 0;
}

முழு iovw_merge(மாறிலி கட்டமைப்பு iovec_wrapper *iovw, size_t length_size, கட்டமைப்பு iovec *ret) {
        ப்ரதிஜ்ஞா(IN_SET(length_size, 1, 2, 4));
        ப்ரதிஜ்ஞா(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        எனில் (sz == 0) {
                *ret = (கட்டமைப்பு iovec) {};
                திரும்பு 0;
        }
        எனில் (sz == SIZE_MAX)
                திரும்பு -E2BIG;

        எனில் (size_multiply_overflow(length_size, iovw->மொத்தம்))
                திரும்பு -E2BIG;

        sz = size_add(sz, iovw->மொத்தம் * length_size);
        எனில் (sz == SIZE_MAX)
                திரும்பு -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->மொத்தம்) {
                எனில் (iov->iov_len == 0)
                        தொடர்;

                தேர்வு (length_size) {
                நிலை 1:
                        எனில் (iov->iov_len > UINT8_MAX)
                                திரும்பு -ERANGE;

                        *p = iov->iov_len;
                        நிறுத்து;
                நிலை 2:
                        எனில் (iov->iov_len > UINT16_MAX)
                                திரும்பு -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        நிறுத்து;
                நிலை 4:
                        எனில் (iov->iov_len > UINT32_MAX)
                                திரும்பு -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        நிறுத்து;
                இயல்பு:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        ப்ரதிஜ்ஞா(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        திரும்பு 0;
}
