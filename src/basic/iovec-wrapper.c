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
        якщо (!iovw)
                повернути NULL;

        iovw_done(iovw);
        повернути mfree(iovw);
}

struct iovec_wrapper* iovw_free_free(struct iovec_wrapper *iovw) {
        якщо (!iovw)
                повернути NULL;

        iovw_done_free(iovw);
        повернути mfree(iovw);
}

ціле iovw_compare(const struct iovec_wrapper *a, const struct iovec_wrapper *b) {
        ціле r;

        якщо (a == b)
                повернути 0;

        якщо (!a || !b)
                повернути CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        для (size_t i = 0, n = MIN(a->count, b->count); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                якщо (r != 0)
                        повернути r;
        }

        повернути CMP(a->count, b->count);
}

ціле iovw_put_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        якщо (len == 0 && !accept_zero)
                повернути 0;

        якщо (iovw->count >= IOV_MAX)
                повернути -E2BIG;

        якщо (!GREEDY_REALLOC(iovw->iovec, iovw->count + 1))
                повернути -ENOMEM;

        iovw->iovec[iovw->count++] = IOVEC_MAKE(data, len);
        повернути 1;
}

ціле iovw_put_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov) {
        assert(iovw);

        якщо (!iov)
                повернути 0;

        повернути iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ціле iovw_put_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source) {
        ціле r;

        assert(iovw);

        якщо (iovw_isempty(source))
                повернути 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        якщо (iovw == source)
                повернути -EINVAL;

        якщо (iovw->count > SIZE_MAX - source->count)
                повернути -E2BIG;
        якщо (iovw->count + source->count > IOV_MAX)
                повернути -E2BIG;

        якщо (accept_zero) {
                якщо (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->count, source->iovec, source->count))
                        повернути -ENOMEM;

                повернути 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                якщо (r < 0)
                        goto rollback;
        }

        повернути 0;

rollback:
        iovw->count = original_count;
        повернути r;
}

ціле iovw_consume_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        /* Move data into iovw or free on error */
        ціле r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        якщо (r <= 0)
                free(data);

        повернути r;
}

ціле iovw_consume_iov_full(struct iovec_wrapper *iovw, bool accept_zero, struct iovec *iov) {
        ціле r;

        assert(iovw);

        якщо (!iov)
                повернути 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        якщо (r <= 0)
                iovec_done(iov);
        інакше
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (struct iovec) {};

        повернути r;
}

ціле iovw_extend_full(struct iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        якщо (len == 0)
                повернути iovw_put_full(iovw, accept_zero, /* data= */ NULL, /* len= */ 0);

        void *c = memdup(data, len);
        якщо (!c)
                повернути -ENOMEM;

        повернути iovw_consume_full(iovw, accept_zero, c, len);
}

ціле iovw_extend_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov) {
        assert(iovw);

        якщо (!iov)
                повернути 0;

        повернути iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ціле iovw_extend_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source) {
        ціле r;

        assert(iovw);

        /* This duplicates the source and merges it into the iovw. */

        якщо (iovw_isempty(source))
                повернути 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        якщо (iovw == source)
                повернути -EINVAL;

        якщо (iovw->count > SIZE_MAX - source->count)
                повернути -E2BIG;
        якщо (iovw->count + source->count > IOV_MAX)
                повернути -E2BIG;

        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                якщо (r < 0)
                        goto rollback;
        }

        повернути 0;

rollback:
        для (size_t i = original_count; i < iovw->count; i++)
                iovec_done(iovw->iovec + i);

        iovw->count = original_count;
        повернути r;
}

ціле iovw_put_string_field_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *value) {
        _cleanup_free_ char *x = NULL;
        ціле r;

        assert(iovw);

        x = strjoin(field, value);
        якщо (!x)
                повернути -ENOMEM;

        якщо (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->count)
                        якщо (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                повернути free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        якщо (r >= 0)
                TAKE_PTR(x);

        повернути r;
}

ціле iovw_put_string_fieldf_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *format, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        ціле r;

        assert(format);

        va_start(ap, format);
        r = vasprintf(&value, format, ap);
        va_end(ap);
        якщо (r < 0)
                повернути -ENOMEM;

        повернути iovw_put_string_field_full(iovw, replace, field, value);
}

ціле iovw_put_string_field_free(struct iovec_wrapper *iovw, const char *field, char *value) {
        _cleanup_free_ _unused_ char *free_ptr = value;

        повернути iovw_put_string_field(iovw, field, value);
}

void iovw_rebase(struct iovec_wrapper *iovw, void *old, void *new) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count) {
                assert(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(const struct iovec_wrapper *iovw) {
        якщо (iovw_isempty(iovw))
                повернути 0;

        повернути iovec_total_size(iovw->iovec, iovw->count);
}

ціле iovw_concat(const struct iovec_wrapper *iovw, struct iovec *ret) {
        assert(iovw);
        assert(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        якщо (len == SIZE_MAX)
                повернути -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        якщо (!buf)
                повернути -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        повернути 0;
}

char* iovw_to_cstring(const struct iovec_wrapper *iovw) {
        assert(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) struct iovec iov = {};
        якщо (iovw_concat(iovw, &iov) < 0)
                повернути NULL;

        assert(!memchr(iov.iov_base, 0, iov.iov_len));
        повернути TAKE_PTR(iov.iov_base);
}

ціле iovec_split(const struct iovec *iov, size_t length_size, struct iovec_wrapper *ret) {
        ціле r;

        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        якщо (!iovec_is_set(iov)) {
                *ret = (struct iovec_wrapper) {};
                повернути 0;
        }

        _cleanup_(iovw_done_free) struct iovec_wrapper iovw = {};
        для (struct iovec i = *iov; iovec_is_set(&i); ) {
                якщо (i.iov_len < length_size)
                        повернути -EBADMSG;

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

                якщо (len == 0)
                        continue;

                якщо (i.iov_len < len)
                        повернути -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                якщо (r < 0)
                        повернути r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        повернути 0;
}

ціле iovw_merge(const struct iovec_wrapper *iovw, size_t length_size, struct iovec *ret) {
        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        якщо (sz == 0) {
                *ret = (struct iovec) {};
                повернути 0;
        }
        якщо (sz == SIZE_MAX)
                повернути -E2BIG;

        якщо (size_multiply_overflow(length_size, iovw->count))
                повернути -E2BIG;

        sz = size_add(sz, iovw->count * length_size);
        якщо (sz == SIZE_MAX)
                повернути -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        якщо (!buf)
                повернути -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->count) {
                якщо (iov->iov_len == 0)
                        continue;

                switch (length_size) {
                case 1:
                        якщо (iov->iov_len > UINT8_MAX)
                                повернути -ERANGE;

                        *p = iov->iov_len;
                        break;
                case 2:
                        якщо (iov->iov_len > UINT16_MAX)
                                повернути -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        break;
                case 4:
                        якщо (iov->iov_len > UINT32_MAX)
                                повернути -ERANGE;

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
        повернути 0;
}
