/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "iovec-util.h"
#ضمن "iovec-wrapper.h"
#ضمن "string-util.h"
#ضمن "unaligned.h"

فراغ iovw_done(بنية iovec_wrapper *iovw) {
        أكد(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->تعداد_2 = 0;
}

فراغ iovw_done_free(بنية iovec_wrapper *iovw) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->تعداد_2)
                iovec_done(i);

        iovw_done(iovw);
}

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw) {
        إذا (!iovw)
                إرجاع عدم;

        iovw_done(iovw);
        إرجاع mfree(iovw);
}

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw) {
        إذا (!iovw)
                إرجاع عدم;

        iovw_done_free(iovw);
        إرجاع mfree(iovw);
}

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        صحيح r;

        إذا (a == b)
                إرجاع 0;

        إذا (!a || !b)
                إرجاع CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        لكل (size_t i = 0, n = MIN(a->تعداد_2, b->تعداد_2); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                إذا (r != 0)
                        إرجاع r;
        }

        إرجاع CMP(a->تعداد_2, b->تعداد_2);
}

صحيح iovw_put_full(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *data, size_t len) {
        أكد(iovw);
        أكد(data || len == 0);

        إذا (len == 0 && !accept_zero)
                إرجاع 0;

        إذا (iovw->تعداد_2 >= IOV_MAX)
                إرجاع -E2BIG;

        إذا (!GREEDY_REALLOC(iovw->iovec, iovw->تعداد_2 + 1))
                إرجاع -ENOMEM;

        iovw->iovec[iovw->تعداد_2++] = IOVEC_MAKE(data, len);
        إرجاع 1;
}

صحيح iovw_put_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        إذا (!iov)
                إرجاع 0;

        إرجاع iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_put_iovw_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *source) {
        صحيح r;

        أكد(iovw);

        إذا (iovw_isempty(source))
                إرجاع 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        إذا (iovw == source)
                إرجاع -EINVAL;

        إذا (iovw->تعداد_2 > SIZE_MAX - source->تعداد_2)
                إرجاع -E2BIG;
        إذا (iovw->تعداد_2 + source->تعداد_2 > IOV_MAX)
                إرجاع -E2BIG;

        إذا (accept_zero) {
                إذا (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->تعداد_2, source->iovec, source->تعداد_2))
                        إرجاع -ENOMEM;

                إرجاع 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_تعداد = iovw->تعداد_2;

        FOREACH_ARRAY(iovec, source->iovec, source->تعداد_2) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                إذا (r < 0)
                        اذهب rollback;
        }

        إرجاع 0;

rollback:
        iovw->تعداد_2 = original_تعداد;
        إرجاع r;
}

صحيح iovw_consume_full(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *data, size_t len) {
        /* Move data into iovw or free on error */
        صحيح r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        إذا (r <= 0)
                حرر(data);

        إرجاع r;
}

صحيح iovw_consume_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov) {
        صحيح r;

        أكد(iovw);

        إذا (!iov)
                إرجاع 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        إذا (r <= 0)
                iovec_done(iov);
        وإلا
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (بنية iovec) {};

        إرجاع r;
}

صحيح iovw_extend_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *data, size_t len) {
        أكد(iovw);
        أكد(data || len == 0);

        إذا (len == 0)
                إرجاع iovw_put_full(iovw, accept_zero, /* data= */ عدم, /* len= */ 0);

        فراغ *c = memdup(data, len);
        إذا (!c)
                إرجاع -ENOMEM;

        إرجاع iovw_consume_full(iovw, accept_zero, c, len);
}

صحيح iovw_extend_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        إذا (!iov)
                إرجاع 0;

        إرجاع iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_extend_iovw_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *source) {
        صحيح r;

        أكد(iovw);

        /* This duplicates the source and merges it into the iovw. */

        إذا (iovw_isempty(source))
                إرجاع 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        إذا (iovw == source)
                إرجاع -EINVAL;

        إذا (iovw->تعداد_2 > SIZE_MAX - source->تعداد_2)
                إرجاع -E2BIG;
        إذا (iovw->تعداد_2 + source->تعداد_2 > IOV_MAX)
                إرجاع -E2BIG;

        size_t original_تعداد = iovw->تعداد_2;

        FOREACH_ARRAY(iovec, source->iovec, source->تعداد_2) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                إذا (r < 0)
                        اذهب rollback;
        }

        إرجاع 0;

rollback:
        لكل (size_t i = original_تعداد; i < iovw->تعداد_2; i++)
                iovec_done(iovw->iovec + i);

        iovw->تعداد_2 = original_تعداد;
        إرجاع r;
}

صحيح iovw_put_string_field_full(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *field, ثابت حرف *قيمة) {
        _cleanup_free_ حرف *x = عدم;
        صحيح r;

        أكد(iovw);

        x = strjoin(field, قيمة);
        إذا (!x)
                إرجاع -ENOMEM;

        إذا (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->تعداد_2)
                        إذا (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                إرجاع free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        إذا (r >= 0)
                TAKE_PTR(x);

        إرجاع r;
}

صحيح iovw_put_string_fieldf_full(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *field, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *قيمة = عدم;
        va_list ap;
        صحيح r;

        أكد(format);

        va_start(ap, format);
        r = vasprintf(&قيمة, format, ap);
        va_end(ap);
        إذا (r < 0)
                إرجاع -ENOMEM;

        إرجاع iovw_put_string_field_full(iovw, replace, field, قيمة);
}

صحيح iovw_put_string_field_free(بنية iovec_wrapper *iovw, ثابت حرف *field, حرف *قيمة) {
        _cleanup_free_ _unused_ حرف *free_ptr = قيمة;

        إرجاع iovw_put_string_field(iovw, field, قيمة);
}

فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->تعداد_2) {
                أكد(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ثابت بنية iovec_wrapper *iovw) {
        إذا (iovw_isempty(iovw))
                إرجاع 0;

        إرجاع iovec_total_size(iovw->iovec, iovw->تعداد_2);
}

صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret) {
        أكد(iovw);
        أكد(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        إذا (len == SIZE_MAX)
                إرجاع -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        إذا (!buf)
                إرجاع -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->تعداد_2)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        إرجاع 0;
}

حرف* iovw_to_cstring(ثابت بنية iovec_wrapper *iovw) {
        أكد(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) بنية iovec iov = {};
        إذا (iovw_concat(iovw, &iov) < 0)
                إرجاع عدم;

        أكد(!memchr(iov.iov_base, 0, iov.iov_len));
        إرجاع TAKE_PTR(iov.iov_base);
}

صحيح iovec_split(ثابت بنية iovec *iov, size_t length_size, بنية iovec_wrapper *ret) {
        صحيح r;

        أكد(IN_SET(length_size, 1, 2, 4));
        أكد(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        إذا (!iovec_is_set(iov)) {
                *ret = (بنية iovec_wrapper) {};
                إرجاع 0;
        }

        _cleanup_(iovw_done_free) بنية iovec_wrapper iovw = {};
        لكل (بنية iovec i = *iov; iovec_is_set(&i); ) {
                إذا (i.iov_len < length_size)
                        إرجاع -EBADMSG;

                size_t len;
                بدل (length_size) {
                حالة 1:
                        len = *(uint8_t*) i.iov_base;
                        اكسر;
                حالة 2:
                        len = unaligned_read_be16(i.iov_base);
                        اكسر;
                حالة 4:
                        len = unaligned_read_be32(i.iov_base);
                        اكسر;
                افتراضي:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                إذا (len == 0)
                        استمر;

                إذا (i.iov_len < len)
                        إرجاع -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                إذا (r < 0)
                        إرجاع r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        إرجاع 0;
}

صحيح iovw_merge(ثابت بنية iovec_wrapper *iovw, size_t length_size, بنية iovec *ret) {
        أكد(IN_SET(length_size, 1, 2, 4));
        أكد(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        إذا (sz == 0) {
                *ret = (بنية iovec) {};
                إرجاع 0;
        }
        إذا (sz == SIZE_MAX)
                إرجاع -E2BIG;

        إذا (size_multiply_overflow(length_size, iovw->تعداد_2))
                إرجاع -E2BIG;

        sz = size_add(sz, iovw->تعداد_2 * length_size);
        إذا (sz == SIZE_MAX)
                إرجاع -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        إذا (!buf)
                إرجاع -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->تعداد_2) {
                إذا (iov->iov_len == 0)
                        استمر;

                بدل (length_size) {
                حالة 1:
                        إذا (iov->iov_len > UINT8_MAX)
                                إرجاع -ERANGE;

                        *p = iov->iov_len;
                        اكسر;
                حالة 2:
                        إذا (iov->iov_len > UINT16_MAX)
                                إرجاع -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        اكسر;
                حالة 4:
                        إذا (iov->iov_len > UINT32_MAX)
                                إرجاع -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        اكسر;
                افتراضي:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        أكد(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        إرجاع 0;
}
