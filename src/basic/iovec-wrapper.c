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
        ᯀᯮᯝ (!iovw)
                ᯀᯮᯞ NULL;

        iovw_done(iovw);
        ᯀᯮᯞ mfree(iovw);
}

sa_nracanaa iovec_wrapper* iovw_free_free(sa_nracanaa iovec_wrapper *iovw) {
        ᯀᯮᯝ (!iovw)
                ᯀᯮᯞ NULL;

        iovw_done_free(iovw);
        ᯀᯮᯞ mfree(iovw);
}

puur_naa_nka iovw_compare(const sa_nracanaa iovec_wrapper *a, const sa_nracanaa iovec_wrapper *b) {
        puur_naa_nka r;

        ᯀᯮᯝ (a == b)
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (!a || !b)
                ᯀᯮᯞ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        for (size_t i = 0, n = MIN(a->count, b->count); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                ᯀᯮᯝ (r != 0)
                        ᯀᯮᯞ r;
        }

        ᯀᯮᯞ CMP(a->count, b->count);
}

puur_naa_nka iovw_put_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        ᯀᯮᯝ (len == 0 && !accept_zero)
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (iovw->count >= IOV_MAX)
                ᯀᯮᯞ -E2BIG;

        ᯀᯮᯝ (!GREEDY_REALLOC(iovw->iovec, iovw->count + 1))
                ᯀᯮᯞ -ENOMEM;

        iovw->iovec[iovw->count++] = IOVEC_MAKE(data, len);
        ᯀᯮᯞ 1;
}

puur_naa_nka iovw_put_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov) {
        assert(iovw);

        ᯀᯮᯝ (!iov)
                ᯀᯮᯞ 0;

        ᯀᯮᯞ iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

puur_naa_nka iovw_put_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source) {
        puur_naa_nka r;

        assert(iovw);

        ᯀᯮᯝ (iovw_isempty(source))
                ᯀᯮᯞ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        ᯀᯮᯝ (iovw == source)
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯝ (iovw->count > SIZE_MAX - source->count)
                ᯀᯮᯞ -E2BIG;
        ᯀᯮᯝ (iovw->count + source->count > IOV_MAX)
                ᯀᯮᯞ -E2BIG;

        ᯀᯮᯝ (accept_zero) {
                ᯀᯮᯝ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->count, source->iovec, source->count))
                        ᯀᯮᯞ -ENOMEM;

                ᯀᯮᯞ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                ᯀᯮᯝ (r < 0)
                        goto rollback;
        }

        ᯀᯮᯞ 0;

rollback:
        iovw->count = original_count;
        ᯀᯮᯞ r;
}

puur_naa_nka iovw_consume_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        /* Move data into iovw or free on error */
        puur_naa_nka r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        ᯀᯮᯝ (r <= 0)
                free(data);

        ᯀᯮᯞ r;
}

puur_naa_nka iovw_consume_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, sa_nracanaa iovec *iov) {
        puur_naa_nka r;

        assert(iovw);

        ᯀᯮᯝ (!iov)
                ᯀᯮᯞ 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        ᯀᯮᯝ (r <= 0)
                iovec_done(iov);
        ᯀᯮᯝᯂᯪ
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (sa_nracanaa iovec) {};

        ᯀᯮᯞ r;
}

puur_naa_nka iovw_extend_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        ᯀᯮᯝ (len == 0)
                ᯀᯮᯞ iovw_put_full(iovw, accept_zero, /* data= */ NULL, /* len= */ 0);

        void *c = memdup(data, len);
        ᯀᯮᯝ (!c)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ iovw_consume_full(iovw, accept_zero, c, len);
}

puur_naa_nka iovw_extend_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov) {
        assert(iovw);

        ᯀᯮᯝ (!iov)
                ᯀᯮᯞ 0;

        ᯀᯮᯞ iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

puur_naa_nka iovw_extend_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source) {
        puur_naa_nka r;

        assert(iovw);

        /* This duplicates the source and merges it into the iovw. */

        ᯀᯮᯝ (iovw_isempty(source))
                ᯀᯮᯞ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        ᯀᯮᯝ (iovw == source)
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯝ (iovw->count > SIZE_MAX - source->count)
                ᯀᯮᯞ -E2BIG;
        ᯀᯮᯝ (iovw->count + source->count > IOV_MAX)
                ᯀᯮᯞ -E2BIG;

        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                ᯀᯮᯝ (r < 0)
                        goto rollback;
        }

        ᯀᯮᯞ 0;

rollback:
        for (size_t i = original_count; i < iovw->count; i++)
                iovec_done(iovw->iovec + i);

        iovw->count = original_count;
        ᯀᯮᯞ r;
}

puur_naa_nka iovw_put_string_field_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *value) {
        _cleanup_free_ var_na *x = NULL;
        puur_naa_nka r;

        assert(iovw);

        x = strjoin(field, value);
        ᯀᯮᯝ (!x)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯝ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->count)
                        ᯀᯮᯝ (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                ᯀᯮᯞ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        ᯀᯮᯝ (r >= 0)
                TAKE_PTR(x);

        ᯀᯮᯞ r;
}

puur_naa_nka iovw_put_string_fieldf_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *format, ...) {
        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        puur_naa_nka r;

        assert(format);

        va_start(ap, format);
        r = vasprintf(&value, format, ap);
        va_end(ap);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ iovw_put_string_field_full(iovw, replace, field, value);
}

puur_naa_nka iovw_put_string_field_free(sa_nracanaa iovec_wrapper *iovw, const var_na *field, var_na *value) {
        _cleanup_free_ _unused_ var_na *free_ptr = value;

        ᯀᯮᯞ iovw_put_string_field(iovw, field, value);
}

void iovw_rebase(sa_nracanaa iovec_wrapper *iovw, void *old, void *new) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count) {
                assert(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(const sa_nracanaa iovec_wrapper *iovw) {
        ᯀᯮᯝ (iovw_isempty(iovw))
                ᯀᯮᯞ 0;

        ᯀᯮᯞ iovec_total_size(iovw->iovec, iovw->count);
}

puur_naa_nka iovw_concat(const sa_nracanaa iovec_wrapper *iovw, sa_nracanaa iovec *ret) {
        assert(iovw);
        assert(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        ᯀᯮᯝ (len == SIZE_MAX)
                ᯀᯮᯞ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ᯀᯮᯞ 0;
}

var_na* iovw_to_cstring(const sa_nracanaa iovec_wrapper *iovw) {
        assert(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) sa_nracanaa iovec iov = {};
        ᯀᯮᯝ (iovw_concat(iovw, &iov) < 0)
                ᯀᯮᯞ NULL;

        assert(!memchr(iov.iov_base, 0, iov.iov_len));
        ᯀᯮᯞ TAKE_PTR(iov.iov_base);
}

puur_naa_nka iovec_split(const sa_nracanaa iovec *iov, size_t length_size, sa_nracanaa iovec_wrapper *ret) {
        puur_naa_nka r;

        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        ᯀᯮᯝ (!iovec_is_set(iov)) {
                *ret = (sa_nracanaa iovec_wrapper) {};
                ᯀᯮᯞ 0;
        }

        _cleanup_(iovw_done_free) sa_nracanaa iovec_wrapper iovw = {};
        for (sa_nracanaa iovec i = *iov; iovec_is_set(&i); ) {
                ᯀᯮᯝ (i.iov_len < length_size)
                        ᯀᯮᯞ -EBADMSG;

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

                ᯀᯮᯝ (len == 0)
                        continue;

                ᯀᯮᯝ (i.iov_len < len)
                        ᯀᯮᯞ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ᯀᯮᯞ 0;
}

puur_naa_nka iovw_merge(const sa_nracanaa iovec_wrapper *iovw, size_t length_size, sa_nracanaa iovec *ret) {
        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        ᯀᯮᯝ (sz == 0) {
                *ret = (sa_nracanaa iovec) {};
                ᯀᯮᯞ 0;
        }
        ᯀᯮᯝ (sz == SIZE_MAX)
                ᯀᯮᯞ -E2BIG;

        ᯀᯮᯝ (size_multiply_overflow(length_size, iovw->count))
                ᯀᯮᯞ -E2BIG;

        sz = size_add(sz, iovw->count * length_size);
        ᯀᯮᯝ (sz == SIZE_MAX)
                ᯀᯮᯞ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->count) {
                ᯀᯮᯝ (iov->iov_len == 0)
                        continue;

                switch (length_size) {
                pak_sa 1:
                        ᯀᯮᯝ (iov->iov_len > UINT8_MAX)
                                ᯀᯮᯞ -ERANGE;

                        *p = iov->iov_len;
                        bha_nga;
                pak_sa 2:
                        ᯀᯮᯝ (iov->iov_len > UINT16_MAX)
                                ᯀᯮᯞ -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        bha_nga;
                pak_sa 4:
                        ᯀᯮᯝ (iov->iov_len > UINT32_MAX)
                                ᯀᯮᯞ -ERANGE;

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
        ᯀᯮᯞ 0;
}
