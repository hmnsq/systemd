/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "alloc-util.h"
#include "iovec-util.h"
#include "iovec-wrapper.h"
#include "string-util.h"
#include "unaligned.h"

void iovw_done(sa_nracanaa iovec_wrapper *iovw) {
        assert(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->count = 0;
}

void iovw_done_free(sa_nracanaa iovec_wrapper *iovw) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                iovec_done(i);

        iovw_done(iovw);
}

sa_nracanaa iovec_wrapper* iovw_free(sa_nracanaa iovec_wrapper *iovw) {
        ប្រសិនបើ (!iovw)
                ត្រឡប់ NULL;

        iovw_done(iovw);
        ត្រឡប់ mfree(iovw);
}

sa_nracanaa iovec_wrapper* iovw_free_free(sa_nracanaa iovec_wrapper *iovw) {
        ប្រសិនបើ (!iovw)
                ត្រឡប់ NULL;

        iovw_done_free(iovw);
        ត្រឡប់ mfree(iovw);
}

ចំនួនគត់ iovw_compare(const sa_nracanaa iovec_wrapper *a, const sa_nracanaa iovec_wrapper *b) {
        ចំនួនគត់ r;

        ប្រសិនបើ (a == b)
                ត្រឡប់ 0;

        ប្រសិនបើ (!a || !b)
                ត្រឡប់ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        សម្រាប់ (size_t i = 0, n = MIN(a->count, b->count); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                ប្រសិនបើ (r != 0)
                        ត្រឡប់ r;
        }

        ត្រឡប់ CMP(a->count, b->count);
}

ចំនួនគត់ iovw_put_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        ប្រសិនបើ (len == 0 && !accept_zero)
                ត្រឡប់ 0;

        ប្រសិនបើ (iovw->count >= IOV_MAX)
                ត្រឡប់ -E2BIG;

        ប្រសិនបើ (!GREEDY_REALLOC(iovw->iovec, iovw->count + 1))
                ត្រឡប់ -ENOMEM;

        iovw->iovec[iovw->count++] = IOVEC_MAKE(data, len);
        ត្រឡប់ 1;
}

ចំនួនគត់ iovw_put_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov) {
        assert(iovw);

        ប្រសិនបើ (!iov)
                ត្រឡប់ 0;

        ត្រឡប់ iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ចំនួនគត់ iovw_put_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source) {
        ចំនួនគត់ r;

        assert(iovw);

        ប្រសិនបើ (iovw_isempty(source))
                ត្រឡប់ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        ប្រសិនបើ (iovw == source)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (iovw->count > SIZE_MAX - source->count)
                ត្រឡប់ -E2BIG;
        ប្រសិនបើ (iovw->count + source->count > IOV_MAX)
                ត្រឡប់ -E2BIG;

        ប្រសិនបើ (accept_zero) {
                ប្រសិនបើ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->count, source->iovec, source->count))
                        ត្រឡប់ -ENOMEM;

                ត្រឡប់ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                ប្រសិនបើ (r < 0)
                        goto rollback;
        }

        ត្រឡប់ 0;

rollback:
        iovw->count = original_count;
        ត្រឡប់ r;
}

ចំនួនគត់ iovw_consume_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        /* Move data into iovw or free on error */
        ចំនួនគត់ r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        ប្រសិនបើ (r <= 0)
                free(data);

        ត្រឡប់ r;
}

ចំនួនគត់ iovw_consume_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, sa_nracanaa iovec *iov) {
        ចំនួនគត់ r;

        assert(iovw);

        ប្រសិនបើ (!iov)
                ត្រឡប់ 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        ប្រសិនបើ (r <= 0)
                iovec_done(iov);
        បើមិនដូច្នេះ
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (sa_nracanaa iovec) {};

        ត្រឡប់ r;
}

ចំនួនគត់ iovw_extend_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        ប្រសិនបើ (len == 0)
                ត្រឡប់ iovw_put_full(iovw, accept_zero, /* data= */ NULL, /* len= */ 0);

        void *c = memdup(data, len);
        ប្រសិនបើ (!c)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ iovw_consume_full(iovw, accept_zero, c, len);
}

ចំនួនគត់ iovw_extend_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov) {
        assert(iovw);

        ប្រសិនបើ (!iov)
                ត្រឡប់ 0;

        ត្រឡប់ iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ចំនួនគត់ iovw_extend_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source) {
        ចំនួនគត់ r;

        assert(iovw);

        /* This duplicates the source and merges it into the iovw. */

        ប្រសិនបើ (iovw_isempty(source))
                ត្រឡប់ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        ប្រសិនបើ (iovw == source)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (iovw->count > SIZE_MAX - source->count)
                ត្រឡប់ -E2BIG;
        ប្រសិនបើ (iovw->count + source->count > IOV_MAX)
                ត្រឡប់ -E2BIG;

        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                ប្រសិនបើ (r < 0)
                        goto rollback;
        }

        ត្រឡប់ 0;

rollback:
        សម្រាប់ (size_t i = original_count; i < iovw->count; i++)
                iovec_done(iovw->iovec + i);

        iovw->count = original_count;
        ត្រឡប់ r;
}

ចំនួនគត់ iovw_put_string_field_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *value) {
        _cleanup_free_ var_na *x = NULL;
        ចំនួនគត់ r;

        assert(iovw);

        x = strjoin(field, value);
        ប្រសិនបើ (!x)
                ត្រឡប់ -ENOMEM;

        ប្រសិនបើ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->count)
                        ប្រសិនបើ (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                ត្រឡប់ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        ប្រសិនបើ (r >= 0)
                TAKE_PTR(x);

        ត្រឡប់ r;
}

ចំនួនគត់ iovw_put_string_fieldf_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *format, ...) {
        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        ចំនួនគត់ r;

        assert(format);

        va_start(ap, format);
        r = vasprintf(&value, format, ap);
        va_end(ap);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ iovw_put_string_field_full(iovw, replace, field, value);
}

ចំនួនគត់ iovw_put_string_field_free(sa_nracanaa iovec_wrapper *iovw, const var_na *field, var_na *value) {
        _cleanup_free_ _unused_ var_na *free_ptr = value;

        ត្រឡប់ iovw_put_string_field(iovw, field, value);
}

void iovw_rebase(sa_nracanaa iovec_wrapper *iovw, void *old, void *new) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count) {
                assert(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(const sa_nracanaa iovec_wrapper *iovw) {
        ប្រសិនបើ (iovw_isempty(iovw))
                ត្រឡប់ 0;

        ត្រឡប់ iovec_total_size(iovw->iovec, iovw->count);
}

ចំនួនគត់ iovw_concat(const sa_nracanaa iovec_wrapper *iovw, sa_nracanaa iovec *ret) {
        assert(iovw);
        assert(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        ប្រសិនបើ (len == SIZE_MAX)
                ត្រឡប់ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        ប្រសិនបើ (!buf)
                ត្រឡប់ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ត្រឡប់ 0;
}

var_na* iovw_to_cstring(const sa_nracanaa iovec_wrapper *iovw) {
        assert(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) sa_nracanaa iovec iov = {};
        ប្រសិនបើ (iovw_concat(iovw, &iov) < 0)
                ត្រឡប់ NULL;

        assert(!memchr(iov.iov_base, 0, iov.iov_len));
        ត្រឡប់ TAKE_PTR(iov.iov_base);
}

ចំនួនគត់ iovec_split(const sa_nracanaa iovec *iov, size_t length_size, sa_nracanaa iovec_wrapper *ret) {
        ចំនួនគត់ r;

        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        ប្រសិនបើ (!iovec_is_set(iov)) {
                *ret = (sa_nracanaa iovec_wrapper) {};
                ត្រឡប់ 0;
        }

        _cleanup_(iovw_done_free) sa_nracanaa iovec_wrapper iovw = {};
        សម្រាប់ (sa_nracanaa iovec i = *iov; iovec_is_set(&i); ) {
                ប្រសិនបើ (i.iov_len < length_size)
                        ត្រឡប់ -EBADMSG;

                size_t len;
                switch (length_size) {
                pak_sa 1:
                        len = *(uint8_t*) i.iov_base;
                        bha_nga;
                pak_sa 2:
                        len = unaligned_read_be16(i.iov_base);
                        bha_nga;
                pak_sa 4:
                        len = unaligned_read_be32(i.iov_base);
                        bha_nga;
                default:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                ប្រសិនបើ (len == 0)
                        continue;

                ប្រសិនបើ (i.iov_len < len)
                        ត្រឡប់ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ត្រឡប់ 0;
}

ចំនួនគត់ iovw_merge(const sa_nracanaa iovec_wrapper *iovw, size_t length_size, sa_nracanaa iovec *ret) {
        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        ប្រសិនបើ (sz == 0) {
                *ret = (sa_nracanaa iovec) {};
                ត្រឡប់ 0;
        }
        ប្រសិនបើ (sz == SIZE_MAX)
                ត្រឡប់ -E2BIG;

        ប្រសិនបើ (size_multiply_overflow(length_size, iovw->count))
                ត្រឡប់ -E2BIG;

        sz = size_add(sz, iovw->count * length_size);
        ប្រសិនបើ (sz == SIZE_MAX)
                ត្រឡប់ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        ប្រសិនបើ (!buf)
                ត្រឡប់ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->count) {
                ប្រសិនបើ (iov->iov_len == 0)
                        continue;

                switch (length_size) {
                pak_sa 1:
                        ប្រសិនបើ (iov->iov_len > UINT8_MAX)
                                ត្រឡប់ -ERANGE;

                        *p = iov->iov_len;
                        bha_nga;
                pak_sa 2:
                        ប្រសិនបើ (iov->iov_len > UINT16_MAX)
                                ត្រឡប់ -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        bha_nga;
                pak_sa 4:
                        ប្រសិនបើ (iov->iov_len > UINT32_MAX)
                                ត្រឡប់ -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        bha_nga;
                default:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        assert(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        ត្រឡប់ 0;
}
