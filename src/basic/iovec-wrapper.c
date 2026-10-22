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
        iovw->count = 0;
}

فراغ iovw_done_free(بنية iovec_wrapper *iovw) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                iovec_done(i);

        iovw_done(iovw);
}

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw) {
        אם (!iovw)
                החזר عدم;

        iovw_done(iovw);
        החזר mfree(iovw);
}

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw) {
        אם (!iovw)
                החזר عدم;

        iovw_done_free(iovw);
        החזר mfree(iovw);
}

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        صحيح r;

        אם (a == b)
                החזר 0;

        אם (!a || !b)
                החזר CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        עבור (size_t i = 0, n = MIN(a->count, b->count); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                אם (r != 0)
                        החזר r;
        }

        החזר CMP(a->count, b->count);
}

صحيح iovw_put_full(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *data, size_t len) {
        أكد(iovw);
        أكد(data || len == 0);

        אם (len == 0 && !accept_zero)
                החזר 0;

        אם (iovw->count >= IOV_MAX)
                החזר -E2BIG;

        אם (!GREEDY_REALLOC(iovw->iovec, iovw->count + 1))
                החזר -ENOMEM;

        iovw->iovec[iovw->count++] = IOVEC_MAKE(data, len);
        החזר 1;
}

صحيح iovw_put_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        אם (!iov)
                החזר 0;

        החזר iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_put_iovw_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *source) {
        صحيح r;

        أكد(iovw);

        אם (iovw_isempty(source))
                החזר 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        אם (iovw == source)
                החזר -EINVAL;

        אם (iovw->count > SIZE_MAX - source->count)
                החזר -E2BIG;
        אם (iovw->count + source->count > IOV_MAX)
                החזר -E2BIG;

        אם (accept_zero) {
                אם (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->count, source->iovec, source->count))
                        החזר -ENOMEM;

                החזר 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                אם (r < 0)
                        اذهب rollback;
        }

        החזר 0;

rollback:
        iovw->count = original_count;
        החזר r;
}

صحيح iovw_consume_full(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *data, size_t len) {
        /* Move data into iovw or free on error */
        صحيح r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        אם (r <= 0)
                حرر(data);

        החזר r;
}

صحيح iovw_consume_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov) {
        صحيح r;

        أكد(iovw);

        אם (!iov)
                החזר 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        אם (r <= 0)
                iovec_done(iov);
        אחרת
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (بنية iovec) {};

        החזר r;
}

صحيح iovw_extend_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *data, size_t len) {
        أكد(iovw);
        أكد(data || len == 0);

        אם (len == 0)
                החזר iovw_put_full(iovw, accept_zero, /* data= */ عدم, /* len= */ 0);

        فراغ *c = memdup(data, len);
        אם (!c)
                החזר -ENOMEM;

        החזר iovw_consume_full(iovw, accept_zero, c, len);
}

صحيح iovw_extend_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        אם (!iov)
                החזר 0;

        החזר iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_extend_iovw_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *source) {
        صحيح r;

        أكد(iovw);

        /* This duplicates the source and merges it into the iovw. */

        אם (iovw_isempty(source))
                החזר 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        אם (iovw == source)
                החזר -EINVAL;

        אם (iovw->count > SIZE_MAX - source->count)
                החזר -E2BIG;
        אם (iovw->count + source->count > IOV_MAX)
                החזר -E2BIG;

        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                אם (r < 0)
                        اذهب rollback;
        }

        החזר 0;

rollback:
        עבור (size_t i = original_count; i < iovw->count; i++)
                iovec_done(iovw->iovec + i);

        iovw->count = original_count;
        החזר r;
}

صحيح iovw_put_string_field_full(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *field, ثابت حرف *value) {
        _cleanup_free_ حرف *x = عدم;
        صحيح r;

        أكد(iovw);

        x = strjoin(field, value);
        אם (!x)
                החזר -ENOMEM;

        אם (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->count)
                        אם (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                החזר free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        אם (r >= 0)
                TAKE_PTR(x);

        החזר r;
}

صحيح iovw_put_string_fieldf_full(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *field, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *value = عدم;
        va_list ap;
        صحيح r;

        أكد(format);

        va_start(ap, format);
        r = vasprintf(&value, format, ap);
        va_end(ap);
        אם (r < 0)
                החזר -ENOMEM;

        החזר iovw_put_string_field_full(iovw, replace, field, value);
}

صحيح iovw_put_string_field_free(بنية iovec_wrapper *iovw, ثابت حرف *field, حرف *value) {
        _cleanup_free_ _unused_ حرف *free_ptr = value;

        החזר iovw_put_string_field(iovw, field, value);
}

فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count) {
                أكد(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ثابت بنية iovec_wrapper *iovw) {
        אם (iovw_isempty(iovw))
                החזר 0;

        החזר iovec_total_size(iovw->iovec, iovw->count);
}

صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret) {
        أكد(iovw);
        أكد(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        אם (len == SIZE_MAX)
                החזר -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        אם (!buf)
                החזר -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        החזר 0;
}

حرف* iovw_to_cstring(ثابت بنية iovec_wrapper *iovw) {
        أكد(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) بنية iovec iov = {};
        אם (iovw_concat(iovw, &iov) < 0)
                החזר عدم;

        أكد(!memchr(iov.iov_base, 0, iov.iov_len));
        החזר TAKE_PTR(iov.iov_base);
}

صحيح iovec_split(ثابت بنية iovec *iov, size_t length_size, بنية iovec_wrapper *ret) {
        صحيح r;

        أكد(IN_SET(length_size, 1, 2, 4));
        أكد(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        אם (!iovec_is_set(iov)) {
                *ret = (بنية iovec_wrapper) {};
                החזר 0;
        }

        _cleanup_(iovw_done_free) بنية iovec_wrapper iovw = {};
        עבור (بنية iovec i = *iov; iovec_is_set(&i); ) {
                אם (i.iov_len < length_size)
                        החזר -EBADMSG;

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

                אם (len == 0)
                        استمر;

                אם (i.iov_len < len)
                        החזר -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                אם (r < 0)
                        החזר r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        החזר 0;
}

صحيح iovw_merge(ثابت بنية iovec_wrapper *iovw, size_t length_size, بنية iovec *ret) {
        أكد(IN_SET(length_size, 1, 2, 4));
        أكد(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        אם (sz == 0) {
                *ret = (بنية iovec) {};
                החזר 0;
        }
        אם (sz == SIZE_MAX)
                החזר -E2BIG;

        אם (size_multiply_overflow(length_size, iovw->count))
                החזר -E2BIG;

        sz = size_add(sz, iovw->count * length_size);
        אם (sz == SIZE_MAX)
                החזר -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        אם (!buf)
                החזר -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->count) {
                אם (iov->iov_len == 0)
                        استمر;

                بدل (length_size) {
                حالة 1:
                        אם (iov->iov_len > UINT8_MAX)
                                החזר -ERANGE;

                        *p = iov->iov_len;
                        اكسر;
                حالة 2:
                        אם (iov->iov_len > UINT16_MAX)
                                החזר -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        اكسر;
                حالة 4:
                        אם (iov->iov_len > UINT32_MAX)
                                החזר -ERANGE;

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
        החזר 0;
}
