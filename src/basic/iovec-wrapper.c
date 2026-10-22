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
        nếu (!iovw)
                trả_về NULL;

        iovw_done(iovw);
        trả_về mfree(iovw);
}

struct iovec_wrapper* iovw_free_free(struct iovec_wrapper *iovw) {
        nếu (!iovw)
                trả_về NULL;

        iovw_done_free(iovw);
        trả_về mfree(iovw);
}

số_nguyên iovw_compare(const struct iovec_wrapper *a, const struct iovec_wrapper *b) {
        số_nguyên r;

        nếu (a == b)
                trả_về 0;

        nếu (!a || !b)
                trả_về CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        cho (size_t i = 0, n = MIN(a->count, b->count); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                nếu (r != 0)
                        trả_về r;
        }

        trả_về CMP(a->count, b->count);
}

số_nguyên iovw_put_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        nếu (len == 0 && !accept_zero)
                trả_về 0;

        nếu (iovw->count >= IOV_MAX)
                trả_về -E2BIG;

        nếu (!GREEDY_REALLOC(iovw->iovec, iovw->count + 1))
                trả_về -ENOMEM;

        iovw->iovec[iovw->count++] = IOVEC_MAKE(data, len);
        trả_về 1;
}

số_nguyên iovw_put_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov) {
        assert(iovw);

        nếu (!iov)
                trả_về 0;

        trả_về iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

số_nguyên iovw_put_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source) {
        số_nguyên r;

        assert(iovw);

        nếu (iovw_isempty(source))
                trả_về 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        nếu (iovw == source)
                trả_về -EINVAL;

        nếu (iovw->count > SIZE_MAX - source->count)
                trả_về -E2BIG;
        nếu (iovw->count + source->count > IOV_MAX)
                trả_về -E2BIG;

        nếu (accept_zero) {
                nếu (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->count, source->iovec, source->count))
                        trả_về -ENOMEM;

                trả_về 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                nếu (r < 0)
                        goto rollback;
        }

        trả_về 0;

rollback:
        iovw->count = original_count;
        trả_về r;
}

số_nguyên iovw_consume_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        /* Move data into iovw or free on error */
        số_nguyên r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        nếu (r <= 0)
                free(data);

        trả_về r;
}

số_nguyên iovw_consume_iov_full(struct iovec_wrapper *iovw, bool accept_zero, struct iovec *iov) {
        số_nguyên r;

        assert(iovw);

        nếu (!iov)
                trả_về 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        nếu (r <= 0)
                iovec_done(iov);
        không_thì
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (struct iovec) {};

        trả_về r;
}

số_nguyên iovw_extend_full(struct iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        nếu (len == 0)
                trả_về iovw_put_full(iovw, accept_zero, /* data= */ NULL, /* len= */ 0);

        void *c = memdup(data, len);
        nếu (!c)
                trả_về -ENOMEM;

        trả_về iovw_consume_full(iovw, accept_zero, c, len);
}

số_nguyên iovw_extend_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov) {
        assert(iovw);

        nếu (!iov)
                trả_về 0;

        trả_về iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

số_nguyên iovw_extend_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source) {
        số_nguyên r;

        assert(iovw);

        /* This duplicates the source and merges it into the iovw. */

        nếu (iovw_isempty(source))
                trả_về 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        nếu (iovw == source)
                trả_về -EINVAL;

        nếu (iovw->count > SIZE_MAX - source->count)
                trả_về -E2BIG;
        nếu (iovw->count + source->count > IOV_MAX)
                trả_về -E2BIG;

        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                nếu (r < 0)
                        goto rollback;
        }

        trả_về 0;

rollback:
        cho (size_t i = original_count; i < iovw->count; i++)
                iovec_done(iovw->iovec + i);

        iovw->count = original_count;
        trả_về r;
}

số_nguyên iovw_put_string_field_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *value) {
        _cleanup_free_ char *x = NULL;
        số_nguyên r;

        assert(iovw);

        x = strjoin(field, value);
        nếu (!x)
                trả_về -ENOMEM;

        nếu (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->count)
                        nếu (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                trả_về free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        nếu (r >= 0)
                TAKE_PTR(x);

        trả_về r;
}

số_nguyên iovw_put_string_fieldf_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *format, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        số_nguyên r;

        assert(format);

        va_start(ap, format);
        r = vasprintf(&value, format, ap);
        va_end(ap);
        nếu (r < 0)
                trả_về -ENOMEM;

        trả_về iovw_put_string_field_full(iovw, replace, field, value);
}

số_nguyên iovw_put_string_field_free(struct iovec_wrapper *iovw, const char *field, char *value) {
        _cleanup_free_ _unused_ char *free_ptr = value;

        trả_về iovw_put_string_field(iovw, field, value);
}

void iovw_rebase(struct iovec_wrapper *iovw, void *old, void *new) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count) {
                assert(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(const struct iovec_wrapper *iovw) {
        nếu (iovw_isempty(iovw))
                trả_về 0;

        trả_về iovec_total_size(iovw->iovec, iovw->count);
}

số_nguyên iovw_concat(const struct iovec_wrapper *iovw, struct iovec *ret) {
        assert(iovw);
        assert(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        nếu (len == SIZE_MAX)
                trả_về -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        nếu (!buf)
                trả_về -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        trả_về 0;
}

char* iovw_to_cstring(const struct iovec_wrapper *iovw) {
        assert(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) struct iovec iov = {};
        nếu (iovw_concat(iovw, &iov) < 0)
                trả_về NULL;

        assert(!memchr(iov.iov_base, 0, iov.iov_len));
        trả_về TAKE_PTR(iov.iov_base);
}

số_nguyên iovec_split(const struct iovec *iov, size_t length_size, struct iovec_wrapper *ret) {
        số_nguyên r;

        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        nếu (!iovec_is_set(iov)) {
                *ret = (struct iovec_wrapper) {};
                trả_về 0;
        }

        _cleanup_(iovw_done_free) struct iovec_wrapper iovw = {};
        cho (struct iovec i = *iov; iovec_is_set(&i); ) {
                nếu (i.iov_len < length_size)
                        trả_về -EBADMSG;

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

                nếu (len == 0)
                        continue;

                nếu (i.iov_len < len)
                        trả_về -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                nếu (r < 0)
                        trả_về r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        trả_về 0;
}

số_nguyên iovw_merge(const struct iovec_wrapper *iovw, size_t length_size, struct iovec *ret) {
        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        nếu (sz == 0) {
                *ret = (struct iovec) {};
                trả_về 0;
        }
        nếu (sz == SIZE_MAX)
                trả_về -E2BIG;

        nếu (size_multiply_overflow(length_size, iovw->count))
                trả_về -E2BIG;

        sz = size_add(sz, iovw->count * length_size);
        nếu (sz == SIZE_MAX)
                trả_về -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        nếu (!buf)
                trả_về -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->count) {
                nếu (iov->iov_len == 0)
                        continue;

                switch (length_size) {
                case 1:
                        nếu (iov->iov_len > UINT8_MAX)
                                trả_về -ERANGE;

                        *p = iov->iov_len;
                        break;
                case 2:
                        nếu (iov->iov_len > UINT16_MAX)
                                trả_về -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        break;
                case 4:
                        nếu (iov->iov_len > UINT32_MAX)
                                trả_về -ERANGE;

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
        trả_về 0;
}
