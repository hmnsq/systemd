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
        အကယ်၍ (!iovw)
                ပြန်ပေး NULL;

        iovw_done(iovw);
        ပြန်ပေး mfree(iovw);
}

sa_nracanaa iovec_wrapper* iovw_free_free(sa_nracanaa iovec_wrapper *iovw) {
        အကယ်၍ (!iovw)
                ပြန်ပေး NULL;

        iovw_done_free(iovw);
        ပြန်ပေး mfree(iovw);
}

ကိန်းပြည့် iovw_compare(const sa_nracanaa iovec_wrapper *a, const sa_nracanaa iovec_wrapper *b) {
        ကိန်းပြည့် r;

        အကယ်၍ (a == b)
                ပြန်ပေး 0;

        အကယ်၍ (!a || !b)
                ပြန်ပေး CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        အတွက် (size_t i = 0, n = MIN(a->count, b->count); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                အကယ်၍ (r != 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး CMP(a->count, b->count);
}

ကိန်းပြည့် iovw_put_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        အကယ်၍ (len == 0 && !accept_zero)
                ပြန်ပေး 0;

        အကယ်၍ (iovw->count >= IOV_MAX)
                ပြန်ပေး -E2BIG;

        အကယ်၍ (!GREEDY_REALLOC(iovw->iovec, iovw->count + 1))
                ပြန်ပေး -ENOMEM;

        iovw->iovec[iovw->count++] = IOVEC_MAKE(data, len);
        ပြန်ပေး 1;
}

ကိန်းပြည့် iovw_put_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov) {
        assert(iovw);

        အကယ်၍ (!iov)
                ပြန်ပေး 0;

        ပြန်ပေး iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ကိန်းပြည့် iovw_put_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source) {
        ကိန်းပြည့် r;

        assert(iovw);

        အကယ်၍ (iovw_isempty(source))
                ပြန်ပေး 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        အကယ်၍ (iovw == source)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (iovw->count > SIZE_MAX - source->count)
                ပြန်ပေး -E2BIG;
        အကယ်၍ (iovw->count + source->count > IOV_MAX)
                ပြန်ပေး -E2BIG;

        အကယ်၍ (accept_zero) {
                အကယ်၍ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->count, source->iovec, source->count))
                        ပြန်ပေး -ENOMEM;

                ပြန်ပေး 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                အကယ်၍ (r < 0)
                        goto rollback;
        }

        ပြန်ပေး 0;

rollback:
        iovw->count = original_count;
        ပြန်ပေး r;
}

ကိန်းပြည့် iovw_consume_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len) {
        /* Move data into iovw or free on error */
        ကိန်းပြည့် r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        အကယ်၍ (r <= 0)
                free(data);

        ပြန်ပေး r;
}

ကိန်းပြည့် iovw_consume_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, sa_nracanaa iovec *iov) {
        ကိန်းပြည့် r;

        assert(iovw);

        အကယ်၍ (!iov)
                ပြန်ပေး 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        အကယ်၍ (r <= 0)
                iovec_done(iov);
        မဟုတ်ပါက
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (sa_nracanaa iovec) {};

        ပြန်ပေး r;
}

ကိန်းပြည့် iovw_extend_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len) {
        assert(iovw);
        assert(data || len == 0);

        အကယ်၍ (len == 0)
                ပြန်ပေး iovw_put_full(iovw, accept_zero, /* data= */ NULL, /* len= */ 0);

        void *c = memdup(data, len);
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး iovw_consume_full(iovw, accept_zero, c, len);
}

ကိန်းပြည့် iovw_extend_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov) {
        assert(iovw);

        အကယ်၍ (!iov)
                ပြန်ပေး 0;

        ပြန်ပေး iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ကိန်းပြည့် iovw_extend_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source) {
        ကိန်းပြည့် r;

        assert(iovw);

        /* This duplicates the source and merges it into the iovw. */

        အကယ်၍ (iovw_isempty(source))
                ပြန်ပေး 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        အကယ်၍ (iovw == source)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (iovw->count > SIZE_MAX - source->count)
                ပြန်ပေး -E2BIG;
        အကယ်၍ (iovw->count + source->count > IOV_MAX)
                ပြန်ပေး -E2BIG;

        size_t original_count = iovw->count;

        FOREACH_ARRAY(iovec, source->iovec, source->count) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                အကယ်၍ (r < 0)
                        goto rollback;
        }

        ပြန်ပေး 0;

rollback:
        အတွက် (size_t i = original_count; i < iovw->count; i++)
                iovec_done(iovw->iovec + i);

        iovw->count = original_count;
        ပြန်ပေး r;
}

ကိန်းပြည့် iovw_put_string_field_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *value) {
        _cleanup_free_ var_na *x = NULL;
        ကိန်းပြည့် r;

        assert(iovw);

        x = strjoin(field, value);
        အကယ်၍ (!x)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->count)
                        အကယ်၍ (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                ပြန်ပေး free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        အကယ်၍ (r >= 0)
                TAKE_PTR(x);

        ပြန်ပေး r;
}

ကိန်းပြည့် iovw_put_string_fieldf_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *format, ...) {
        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        ကိန်းပြည့် r;

        assert(format);

        va_start(ap, format);
        r = vasprintf(&value, format, ap);
        va_end(ap);
        အကယ်၍ (r < 0)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး iovw_put_string_field_full(iovw, replace, field, value);
}

ကိန်းပြည့် iovw_put_string_field_free(sa_nracanaa iovec_wrapper *iovw, const var_na *field, var_na *value) {
        _cleanup_free_ _unused_ var_na *free_ptr = value;

        ပြန်ပေး iovw_put_string_field(iovw, field, value);
}

void iovw_rebase(sa_nracanaa iovec_wrapper *iovw, void *old, void *new) {
        assert(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->count) {
                assert(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(const sa_nracanaa iovec_wrapper *iovw) {
        အကယ်၍ (iovw_isempty(iovw))
                ပြန်ပေး 0;

        ပြန်ပေး iovec_total_size(iovw->iovec, iovw->count);
}

ကိန်းပြည့် iovw_concat(const sa_nracanaa iovec_wrapper *iovw, sa_nracanaa iovec *ret) {
        assert(iovw);
        assert(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        အကယ်၍ (len == SIZE_MAX)
                ပြန်ပေး -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->count)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ပြန်ပေး 0;
}

var_na* iovw_to_cstring(const sa_nracanaa iovec_wrapper *iovw) {
        assert(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) sa_nracanaa iovec iov = {};
        အကယ်၍ (iovw_concat(iovw, &iov) < 0)
                ပြန်ပေး NULL;

        assert(!memchr(iov.iov_base, 0, iov.iov_len));
        ပြန်ပေး TAKE_PTR(iov.iov_base);
}

ကိန်းပြည့် iovec_split(const sa_nracanaa iovec *iov, size_t length_size, sa_nracanaa iovec_wrapper *ret) {
        ကိန်းပြည့် r;

        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        အကယ်၍ (!iovec_is_set(iov)) {
                *ret = (sa_nracanaa iovec_wrapper) {};
                ပြန်ပေး 0;
        }

        _cleanup_(iovw_done_free) sa_nracanaa iovec_wrapper iovw = {};
        အတွက် (sa_nracanaa iovec i = *iov; iovec_is_set(&i); ) {
                အကယ်၍ (i.iov_len < length_size)
                        ပြန်ပေး -EBADMSG;

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

                အကယ်၍ (len == 0)
                        continue;

                အကယ်၍ (i.iov_len < len)
                        ပြန်ပေး -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ပြန်ပေး 0;
}

ကိန်းပြည့် iovw_merge(const sa_nracanaa iovec_wrapper *iovw, size_t length_size, sa_nracanaa iovec *ret) {
        assert(IN_SET(length_size, 1, 2, 4));
        assert(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        အကယ်၍ (sz == 0) {
                *ret = (sa_nracanaa iovec) {};
                ပြန်ပေး 0;
        }
        အကယ်၍ (sz == SIZE_MAX)
                ပြန်ပေး -E2BIG;

        အကယ်၍ (size_multiply_overflow(length_size, iovw->count))
                ပြန်ပေး -E2BIG;

        sz = size_add(sz, iovw->count * length_size);
        အကယ်၍ (sz == SIZE_MAX)
                ပြန်ပေး -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->count) {
                အကယ်၍ (iov->iov_len == 0)
                        continue;

                switch (length_size) {
                pak_sa 1:
                        အကယ်၍ (iov->iov_len > UINT8_MAX)
                                ပြန်ပေး -ERANGE;

                        *p = iov->iov_len;
                        bha_nga;
                pak_sa 2:
                        အကယ်၍ (iov->iov_len > UINT16_MAX)
                                ပြန်ပေး -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        bha_nga;
                pak_sa 4:
                        အကယ်၍ (iov->iov_len > UINT32_MAX)
                                ပြန်ပေး -ERANGE;

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
        ပြန်ပေး 0;
}
