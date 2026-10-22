/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "iovec-util.h"
#شامل "iovec-wrapper.h"
#شامل "string-util.h"
#شامل "unaligned.h"

تهی iovw_done(ساختار iovec_wrapper *iovw) {
        أكد(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->شمار = 0;
}

تهی iovw_done_free(ساختار iovec_wrapper *iovw) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->شمار)
                iovec_done(i);

        iovw_done(iovw);
}

ساختار iovec_wrapper* iovw_free(ساختار iovec_wrapper *iovw) {
        اگر (!iovw)
                بازگشت پوچ;

        iovw_done(iovw);
        بازگشت mfree(iovw);
}

ساختار iovec_wrapper* iovw_free_free(ساختار iovec_wrapper *iovw) {
        اگر (!iovw)
                بازگشت پوچ;

        iovw_done_free(iovw);
        بازگشت mfree(iovw);
}

صحیح iovw_compare(ثابت ساختار iovec_wrapper *a, ثابت ساختار iovec_wrapper *b) {
        صحیح r;

        اگر (a == b)
                بازگشت 0;

        اگر (!a || !b)
                بازگشت CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        برای (size_t i = 0, n = MIN(a->شمار, b->شمار); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                اگر (r != 0)
                        بازگشت r;
        }

        بازگشت CMP(a->شمار, b->شمار);
}

صحیح iovw_put_full(ساختار iovec_wrapper *iovw, bool accept_zero, تهی *data, size_t len) {
        أكد(iovw);
        أكد(data || len == 0);

        اگر (len == 0 && !accept_zero)
                بازگشت 0;

        اگر (iovw->شمار >= IOV_MAX)
                بازگشت -E2BIG;

        اگر (!GREEDY_REALLOC(iovw->iovec, iovw->شمار + 1))
                بازگشت -ENOMEM;

        iovw->iovec[iovw->شمار++] = IOVEC_MAKE(data, len);
        بازگشت 1;
}

صحیح iovw_put_iov_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec *iov) {
        أكد(iovw);

        اگر (!iov)
                بازگشت 0;

        بازگشت iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحیح iovw_put_iovw_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec_wrapper *source) {
        صحیح r;

        أكد(iovw);

        اگر (iovw_isempty(source))
                بازگشت 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        اگر (iovw == source)
                بازگشت -EINVAL;

        اگر (iovw->شمار > SIZE_MAX - source->شمار)
                بازگشت -E2BIG;
        اگر (iovw->شمار + source->شمار > IOV_MAX)
                بازگشت -E2BIG;

        اگر (accept_zero) {
                اگر (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->شمار, source->iovec, source->شمار))
                        بازگشت -ENOMEM;

                بازگشت 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_شمار = iovw->شمار;

        FOREACH_ARRAY(iovec, source->iovec, source->شمار) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                اگر (r < 0)
                        برو rollback;
        }

        بازگشت 0;

rollback:
        iovw->شمار = original_شمار;
        بازگشت r;
}

صحیح iovw_consume_full(ساختار iovec_wrapper *iovw, bool accept_zero, تهی *data, size_t len) {
        /* Move data into iovw or free on error */
        صحیح r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        اگر (r <= 0)
                آزاد(data);

        بازگشت r;
}

صحیح iovw_consume_iov_full(ساختار iovec_wrapper *iovw, bool accept_zero, ساختار iovec *iov) {
        صحیح r;

        أكد(iovw);

        اگر (!iov)
                بازگشت 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        اگر (r <= 0)
                iovec_done(iov);
        وگرنه
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (ساختار iovec) {};

        بازگشت r;
}

صحیح iovw_extend_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت تهی *data, size_t len) {
        أكد(iovw);
        أكد(data || len == 0);

        اگر (len == 0)
                بازگشت iovw_put_full(iovw, accept_zero, /* data= */ پوچ, /* len= */ 0);

        تهی *c = memdup(data, len);
        اگر (!c)
                بازگشت -ENOMEM;

        بازگشت iovw_consume_full(iovw, accept_zero, c, len);
}

صحیح iovw_extend_iov_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec *iov) {
        أكد(iovw);

        اگر (!iov)
                بازگشت 0;

        بازگشت iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحیح iovw_extend_iovw_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec_wrapper *source) {
        صحیح r;

        أكد(iovw);

        /* This duplicates the source and merges it into the iovw. */

        اگر (iovw_isempty(source))
                بازگشت 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        اگر (iovw == source)
                بازگشت -EINVAL;

        اگر (iovw->شمار > SIZE_MAX - source->شمار)
                بازگشت -E2BIG;
        اگر (iovw->شمار + source->شمار > IOV_MAX)
                بازگشت -E2BIG;

        size_t original_شمار = iovw->شمار;

        FOREACH_ARRAY(iovec, source->iovec, source->شمار) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                اگر (r < 0)
                        برو rollback;
        }

        بازگشت 0;

rollback:
        برای (size_t i = original_شمار; i < iovw->شمار; i++)
                iovec_done(iovw->iovec + i);

        iovw->شمار = original_شمار;
        بازگشت r;
}

صحیح iovw_put_string_field_full(ساختار iovec_wrapper *iovw, bool replace, ثابت نویسه *field, ثابت نویسه *مقدار) {
        _cleanup_free_ نویسه *x = پوچ;
        صحیح r;

        أكد(iovw);

        x = strjoin(field, مقدار);
        اگر (!x)
                بازگشت -ENOMEM;

        اگر (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->شمار)
                        اگر (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                بازگشت free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        اگر (r >= 0)
                TAKE_PTR(x);

        بازگشت r;
}

صحیح iovw_put_string_fieldf_full(ساختار iovec_wrapper *iovw, bool replace, ثابت نویسه *field, ثابت نویسه *format, ...) {
        _cleanup_free_ نویسه *مقدار = پوچ;
        va_list ap;
        صحیح r;

        أكد(format);

        va_start(ap, format);
        r = vasprintf(&مقدار, format, ap);
        va_end(ap);
        اگر (r < 0)
                بازگشت -ENOMEM;

        بازگشت iovw_put_string_field_full(iovw, replace, field, مقدار);
}

صحیح iovw_put_string_field_free(ساختار iovec_wrapper *iovw, ثابت نویسه *field, نویسه *مقدار) {
        _cleanup_free_ _unused_ نویسه *free_ptr = مقدار;

        بازگشت iovw_put_string_field(iovw, field, مقدار);
}

تهی iovw_rebase(ساختار iovec_wrapper *iovw, تهی *old, تهی *new) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->شمار) {
                أكد(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ثابت ساختار iovec_wrapper *iovw) {
        اگر (iovw_isempty(iovw))
                بازگشت 0;

        بازگشت iovec_total_size(iovw->iovec, iovw->شمار);
}

صحیح iovw_concat(ثابت ساختار iovec_wrapper *iovw, ساختار iovec *ret) {
        أكد(iovw);
        أكد(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        اگر (len == SIZE_MAX)
                بازگشت -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        اگر (!buf)
                بازگشت -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->شمار)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        بازگشت 0;
}

نویسه* iovw_to_cstring(ثابت ساختار iovec_wrapper *iovw) {
        أكد(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) ساختار iovec iov = {};
        اگر (iovw_concat(iovw, &iov) < 0)
                بازگشت پوچ;

        أكد(!memchr(iov.iov_base, 0, iov.iov_len));
        بازگشت TAKE_PTR(iov.iov_base);
}

صحیح iovec_split(ثابت ساختار iovec *iov, size_t length_size, ساختار iovec_wrapper *ret) {
        صحیح r;

        أكد(IN_SET(length_size, 1, 2, 4));
        أكد(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        اگر (!iovec_is_set(iov)) {
                *ret = (ساختار iovec_wrapper) {};
                بازگشت 0;
        }

        _cleanup_(iovw_done_free) ساختار iovec_wrapper iovw = {};
        برای (ساختار iovec i = *iov; iovec_is_set(&i); ) {
                اگر (i.iov_len < length_size)
                        بازگشت -EBADMSG;

                size_t len;
                انتخاب (length_size) {
                حالت 1:
                        len = *(uint8_t*) i.iov_base;
                        بشکن;
                حالت 2:
                        len = unaligned_read_be16(i.iov_base);
                        بشکن;
                حالت 4:
                        len = unaligned_read_be32(i.iov_base);
                        بشکن;
                پیشفرض:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                اگر (len == 0)
                        ادامه;

                اگر (i.iov_len < len)
                        بازگشت -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                اگر (r < 0)
                        بازگشت r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        بازگشت 0;
}

صحیح iovw_merge(ثابت ساختار iovec_wrapper *iovw, size_t length_size, ساختار iovec *ret) {
        أكد(IN_SET(length_size, 1, 2, 4));
        أكد(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        اگر (sz == 0) {
                *ret = (ساختار iovec) {};
                بازگشت 0;
        }
        اگر (sz == SIZE_MAX)
                بازگشت -E2BIG;

        اگر (size_multiply_overflow(length_size, iovw->شمار))
                بازگشت -E2BIG;

        sz = size_add(sz, iovw->شمار * length_size);
        اگر (sz == SIZE_MAX)
                بازگشت -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        اگر (!buf)
                بازگشت -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->شمار) {
                اگر (iov->iov_len == 0)
                        ادامه;

                انتخاب (length_size) {
                حالت 1:
                        اگر (iov->iov_len > UINT8_MAX)
                                بازگشت -ERANGE;

                        *p = iov->iov_len;
                        بشکن;
                حالت 2:
                        اگر (iov->iov_len > UINT16_MAX)
                                بازگشت -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        بشکن;
                حالت 4:
                        اگر (iov->iov_len > UINT32_MAX)
                                بازگشت -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        بشکن;
                پیشفرض:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        أكد(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        بازگشت 0;
}
