/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "alloc-util.h"
#include "iovec-util.h"
#include "iovec-wrapper.h"
#include "string-util.h"
#include "unaligned.h"

void iovw_done(struct iovec_wrapper *iovw) {
        assert(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->count = 0;
}

void iovw_done_free(struct iovec_wrapper *iovw) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                iovec_done(i);

        iovw_done(iovw);
}

struct iovec_wrapper* iovw_free(struct iovec_wrapper *iovw) {
        如果 (!iovw)
                返回 NULL;

        iovw_done(iovw);
        返回 mfree(iovw);
}

struct iovec_wrapper* iovw_free_free(struct iovec_wrapper *iovw) {
        如果 (!iovw)
                返回 NULL;

        iovw_done_free(iovw);
        返回 mfree(iovw);
}

整数 iovw_compare(const struct iovec_wrapper *a, const struct iovec_wrapper *b) {
        整数 r;

        如果 (a == b)
                返回 0;

        如果 (!a || !b)
                返回 CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        为 (size_t i = 0, n = MIN(a->count, b->count); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                如果 (r != 0)
                        返回 r;
        }

        返回 CMP(a->count, b->count);
}

整数 iovw_put_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        如果 (len == 0 && !accept_zero)
                返回 0;

        如果 (iovw->count >= IOV_MAX)
                返回 -E2BIG;

        如果 (!GREEDY_REALLOC(iovw->iovec, iovw->count + 1))
                返回 -ENOMEM;

        iovw->iovec[iovw->count++] = IOVEC_MAKE(data, len);
        返回 1;
}

整数 iovw_put_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov) {
        assert(iovw);

        如果 (!iov)
                返回 0;

        返回 iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

整数 iovw_put_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source) {
        整数 r;

        assert(iovw);

        如果 (iovw_isempty(source))
                返回 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        如果 (iovw == source)
                返回 -EINVAL;

        如果 (iovw->count > SIZE_MAX - source->count)
                返回 -E2BIG;
        如果 (iovw->count + source->count > IOV_MAX)
                返回 -E2BIG;

        如果 (accept_zero) {
                如果 (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->count, source->iovec, source->count))
                        返回 -ENOMEM;

                返回 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                如果 (r < 0)
                        goto rollback;
        }

        返回 0;

rollback:
        iovw->count = original_count;
        返回 r;
}

整数 iovw_consume_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        /* Move data into iovw or free on error */
        整数 r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        如果 (r <= 0)
                free(data);

        返回 r;
}

整数 iovw_consume_iov_full(struct iovec_wrapper *iovw, bool accept_zero, struct iovec *iov) {
        整数 r;

        assert(iovw);

        如果 (!iov)
                返回 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        如果 (r <= 0)
                iovec_done(iov);
        否则
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (struct iovec) {};

        返回 r;
}

整数 iovw_extend_full(struct iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        如果 (len == 0)
                返回 iovw_put_full(iovw, accept_zero, /* data= */ NULL, /* len= */ 0);

        void *c = memdup(data, len);
        如果 (!c)
                返回 -ENOMEM;

        返回 iovw_consume_full(iovw, accept_zero, c, len);
}

整数 iovw_extend_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov) {
        assert(iovw);

        如果 (!iov)
                返回 0;

        返回 iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

整数 iovw_extend_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source) {
        整数 r;

        assert(iovw);

        /* This duplicates the source and merges it into the iovw. */

        如果 (iovw_isempty(source))
                返回 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        如果 (iovw == source)
                返回 -EINVAL;

        如果 (iovw->count > SIZE_MAX - source->count)
                返回 -E2BIG;
        如果 (iovw->count + source->count > IOV_MAX)
                返回 -E2BIG;

        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                如果 (r < 0)
                        goto rollback;
        }

        返回 0;

rollback:
        为 (size_t i = original_count; i < iovw->count; i++)
                iovec_done(iovw->iovec + i);

        iovw->count = original_count;
        返回 r;
}

整数 iovw_put_string_field_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *value) {
        _cleanup_free_ char *x = NULL;
        整数 r;

        assert(iovw);

        x = strjoin(field, value);
        如果 (!x)
                返回 -ENOMEM;

        如果 (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->count)
                        如果 (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                返回 free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        如果 (r >= 0)
                TAKE_PTR(x);

        返回 r;
}

整数 iovw_put_string_fieldf_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *format, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        整数 r;

        assert(format);

        va_start(ap, format);
        r = vasprintf(&value, format, ap);
        va_end(ap);
        如果 (r < 0)
                返回 -ENOMEM;

        返回 iovw_put_string_field_full(iovw, replace, field, value);
}

整数 iovw_put_string_field_free(struct iovec_wrapper *iovw, const char *field, char *value) {
        _cleanup_free_ _unused_ char *free_ptr = value;

        返回 iovw_put_string_field(iovw, field, value);
}

void iovw_rebase(struct iovec_wrapper *iovw, void *old, void *new) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count) {
                assert(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(const struct iovec_wrapper *iovw) {
        如果 (iovw_isempty(iovw))
                返回 0;

        返回 iovec_total_size(iovw->iovec, iovw->count);
}

整数 iovw_concat(const struct iovec_wrapper *iovw, struct iovec *ret) {
        assert(iovw);
        assert(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        如果 (len == SIZE_MAX)
                返回 -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        如果 (!buf)
                返回 -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        返回 0;
}

char* iovw_to_cstring(const struct iovec_wrapper *iovw) {
        assert(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) struct iovec iov = {};
        如果 (iovw_concat(iovw, &iov) < 0)
                返回 NULL;

        assert(!memchr(iov.iov_base, 0, iov.iov_len));
        返回 TAKE_PTR(iov.iov_base);
}

整数 iovec_split(const struct iovec *iov, size_t length_size, struct iovec_wrapper *ret) {
        整数 r;

        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        如果 (!iovec_is_set(iov)) {
                *ret = (struct iovec_wrapper) {};
                返回 0;
        }

        _cleanup_(iovw_done_free) struct iovec_wrapper iovw = {};
        为 (struct iovec i = *iov; iovec_is_set(&i); ) {
                如果 (i.iov_len < length_size)
                        返回 -EBADMSG;

                size_t len;
                switch (length_size) {
                case 1:
                        len = *(uint8_t*) i.iov_base;
                        break;
                case 2:
                        len = unaligned_read_be16(i.iov_base);
                        break;
                case 4:
                        len = unaligned_read_be32(i.iov_base);
                        break;
                default:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                如果 (len == 0)
                        continue;

                如果 (i.iov_len < len)
                        返回 -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                如果 (r < 0)
                        返回 r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        返回 0;
}

整数 iovw_merge(const struct iovec_wrapper *iovw, size_t length_size, struct iovec *ret) {
        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        如果 (sz == 0) {
                *ret = (struct iovec) {};
                返回 0;
        }
        如果 (sz == SIZE_MAX)
                返回 -E2BIG;

        如果 (size_multiply_overflow(length_size, iovw->count))
                返回 -E2BIG;

        sz = size_add(sz, iovw->count * length_size);
        如果 (sz == SIZE_MAX)
                返回 -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        如果 (!buf)
                返回 -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->count) {
                如果 (iov->iov_len == 0)
                        continue;

                switch (length_size) {
                case 1:
                        如果 (iov->iov_len > UINT8_MAX)
                                返回 -ERANGE;

                        *p = iov->iov_len;
                        break;
                case 2:
                        如果 (iov->iov_len > UINT16_MAX)
                                返回 -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        break;
                case 4:
                        如果 (iov->iov_len > UINT32_MAX)
                                返回 -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        break;
                default:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        assert(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        返回 0;
}
