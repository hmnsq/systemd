/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "iovec-wrapper.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "unaligned.h"

શૂન્ય iovw_done(સંરચના iovec_wrapper *iovw) {
        પ્રતિજ્ઞા(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->ગણતરી = 0;
}

શૂન્ય iovw_done_free(સંરચના iovec_wrapper *iovw) {
        પ્રતિજ્ઞા(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ગણતરી)
                iovec_done(i);

        iovw_done(iovw);
}

સંરચના iovec_wrapper* iovw_free(સંરચના iovec_wrapper *iovw) {
        જો (!iovw)
                પાછા ખાલી;

        iovw_done(iovw);
        પાછા mfree(iovw);
}

સંરચના iovec_wrapper* iovw_free_free(સંરચના iovec_wrapper *iovw) {
        જો (!iovw)
                પાછા ખાલી;

        iovw_done_free(iovw);
        પાછા mfree(iovw);
}

પૂર્ણાંક iovw_compare(અચલ સંરચના iovec_wrapper *a, અચલ સંરચના iovec_wrapper *b) {
        પૂર્ણાંક r;

        જો (a == b)
                પાછા 0;

        જો (!a || !b)
                પાછા CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        માટે (size_t i = 0, n = MIN(a->ગણતરી, b->ગણતરી); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                જો (r != 0)
                        પાછા r;
        }

        પાછા CMP(a->ગણતરી, b->ગણતરી);
}

પૂર્ણાંક iovw_put_full(સંરચના iovec_wrapper *iovw, bool accept_zero, શૂન્ય *data, size_t len) {
        પ્રતિજ્ઞા(iovw);
        પ્રતિજ્ઞા(data || len == 0);

        જો (len == 0 && !accept_zero)
                પાછા 0;

        જો (iovw->ગણતરી >= IOV_MAX)
                પાછા -E2BIG;

        જો (!GREEDY_REALLOC(iovw->iovec, iovw->ગણતરી + 1))
                પાછા -ENOMEM;

        iovw->iovec[iovw->ગણતરી++] = IOVEC_MAKE(data, len);
        પાછા 1;
}

પૂર્ણાંક iovw_put_iov_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec *iov) {
        પ્રતિજ્ઞા(iovw);

        જો (!iov)
                પાછા 0;

        પાછા iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

પૂર્ણાંક iovw_put_iovw_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec_wrapper *source) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(iovw);

        જો (iovw_isempty(source))
                પાછા 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        જો (iovw == source)
                પાછા -EINVAL;

        જો (iovw->ગણતરી > SIZE_MAX - source->ગણતરી)
                પાછા -E2BIG;
        જો (iovw->ગણતરી + source->ગણતરી > IOV_MAX)
                પાછા -E2BIG;

        જો (accept_zero) {
                જો (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->ગણતરી, source->iovec, source->ગણતરી))
                        પાછા -ENOMEM;

                પાછા 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_ગણતરી = iovw->ગણતરી;

        FOREACH_ARRAY(iovec, source->iovec, source->ગણતરી) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                જો (r < 0)
                        જાઓ rollback;
        }

        પાછા 0;

rollback:
        iovw->ગણતરી = original_ગણતરી;
        પાછા r;
}

પૂર્ણાંક iovw_consume_full(સંરચના iovec_wrapper *iovw, bool accept_zero, શૂન્ય *data, size_t len) {
        /* Move data into iovw or free on error */
        પૂર્ણાંક r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        જો (r <= 0)
                મુક્ત(data);

        પાછા r;
}

પૂર્ણાંક iovw_consume_iov_full(સંરચના iovec_wrapper *iovw, bool accept_zero, સંરચના iovec *iov) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(iovw);

        જો (!iov)
                પાછા 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        જો (r <= 0)
                iovec_done(iov);
        નહીંતર
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (સંરચના iovec) {};

        પાછા r;
}

પૂર્ણાંક iovw_extend_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ શૂન્ય *data, size_t len) {
        પ્રતિજ્ઞા(iovw);
        પ્રતિજ્ઞા(data || len == 0);

        જો (len == 0)
                પાછા iovw_put_full(iovw, accept_zero, /* data= */ ખાલી, /* len= */ 0);

        શૂન્ય *c = memdup(data, len);
        જો (!c)
                પાછા -ENOMEM;

        પાછા iovw_consume_full(iovw, accept_zero, c, len);
}

પૂર્ણાંક iovw_extend_iov_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec *iov) {
        પ્રતિજ્ઞા(iovw);

        જો (!iov)
                પાછા 0;

        પાછા iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

પૂર્ણાંક iovw_extend_iovw_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec_wrapper *source) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(iovw);

        /* This duplicates the source and merges it into the iovw. */

        જો (iovw_isempty(source))
                પાછા 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        જો (iovw == source)
                પાછા -EINVAL;

        જો (iovw->ગણતરી > SIZE_MAX - source->ગણતરી)
                પાછા -E2BIG;
        જો (iovw->ગણતરી + source->ગણતરી > IOV_MAX)
                પાછા -E2BIG;

        size_t original_ગણતરી = iovw->ગણતરી;

        FOREACH_ARRAY(iovec, source->iovec, source->ગણતરી) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                જો (r < 0)
                        જાઓ rollback;
        }

        પાછા 0;

rollback:
        માટે (size_t i = original_ગણતરી; i < iovw->ગણતરી; i++)
                iovec_done(iovw->iovec + i);

        iovw->ગણતરી = original_ગણતરી;
        પાછા r;
}

પૂર્ણાંક iovw_put_string_field_full(સંરચના iovec_wrapper *iovw, bool replace, અચલ અક્ષર *field, અચલ અક્ષર *કિંમત) {
        _cleanup_free_ અક્ષર *x = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(iovw);

        x = strjoin(field, કિંમત);
        જો (!x)
                પાછા -ENOMEM;

        જો (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->ગણતરી)
                        જો (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                પાછા free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        જો (r >= 0)
                TAKE_PTR(x);

        પાછા r;
}

પૂર્ણાંક iovw_put_string_fieldf_full(સંરચના iovec_wrapper *iovw, bool replace, અચલ અક્ષર *field, અચલ અક્ષર *format, ...) {
        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        va_list ap;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(format);

        va_start(ap, format);
        r = vasprintf(&કિંમત, format, ap);
        va_end(ap);
        જો (r < 0)
                પાછા -ENOMEM;

        પાછા iovw_put_string_field_full(iovw, replace, field, કિંમત);
}

પૂર્ણાંક iovw_put_string_field_free(સંરચના iovec_wrapper *iovw, અચલ અક્ષર *field, અક્ષર *કિંમત) {
        _cleanup_free_ _unused_ અક્ષર *free_ptr = કિંમત;

        પાછા iovw_put_string_field(iovw, field, કિંમત);
}

શૂન્ય iovw_rebase(સંરચના iovec_wrapper *iovw, શૂન્ય *old, શૂન્ય *new) {
        પ્રતિજ્ઞા(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ગણતરી) {
                પ્રતિજ્ઞા(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(અચલ સંરચના iovec_wrapper *iovw) {
        જો (iovw_isempty(iovw))
                પાછા 0;

        પાછા iovec_total_size(iovw->iovec, iovw->ગણતરી);
}

પૂર્ણાંક iovw_concat(અચલ સંરચના iovec_wrapper *iovw, સંરચના iovec *ret) {
        પ્રતિજ્ઞા(iovw);
        પ્રતિજ્ઞા(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        જો (len == SIZE_MAX)
                પાછા -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        જો (!buf)
                પાછા -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->ગણતરી)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        પાછા 0;
}

અક્ષર* iovw_to_cstring(અચલ સંરચના iovec_wrapper *iovw) {
        પ્રતિજ્ઞા(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) સંરચના iovec iov = {};
        જો (iovw_concat(iovw, &iov) < 0)
                પાછા ખાલી;

        પ્રતિજ્ઞા(!memchr(iov.iov_base, 0, iov.iov_len));
        પાછા TAKE_PTR(iov.iov_base);
}

પૂર્ણાંક iovec_split(અચલ સંરચના iovec *iov, size_t length_size, સંરચના iovec_wrapper *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(IN_SET(length_size, 1, 2, 4));
        પ્રતિજ્ઞા(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        જો (!iovec_is_set(iov)) {
                *ret = (સંરચના iovec_wrapper) {};
                પાછા 0;
        }

        _cleanup_(iovw_done_free) સંરચના iovec_wrapper iovw = {};
        માટે (સંરચના iovec i = *iov; iovec_is_set(&i); ) {
                જો (i.iov_len < length_size)
                        પાછા -EBADMSG;

                size_t len;
                પસંદગી (length_size) {
                સ્થિતિ 1:
                        len = *(uint8_t*) i.iov_base;
                        તોડો;
                સ્થિતિ 2:
                        len = unaligned_read_be16(i.iov_base);
                        તોડો;
                સ્થિતિ 4:
                        len = unaligned_read_be32(i.iov_base);
                        તોડો;
                મૂળભૂત:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                જો (len == 0)
                        ચાલુ;

                જો (i.iov_len < len)
                        પાછા -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                જો (r < 0)
                        પાછા r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        પાછા 0;
}

પૂર્ણાંક iovw_merge(અચલ સંરચના iovec_wrapper *iovw, size_t length_size, સંરચના iovec *ret) {
        પ્રતિજ્ઞા(IN_SET(length_size, 1, 2, 4));
        પ્રતિજ્ઞા(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        જો (sz == 0) {
                *ret = (સંરચના iovec) {};
                પાછા 0;
        }
        જો (sz == SIZE_MAX)
                પાછા -E2BIG;

        જો (size_multiply_overflow(length_size, iovw->ગણતરી))
                પાછા -E2BIG;

        sz = size_add(sz, iovw->ગણતરી * length_size);
        જો (sz == SIZE_MAX)
                પાછા -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        જો (!buf)
                પાછા -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->ગણતરી) {
                જો (iov->iov_len == 0)
                        ચાલુ;

                પસંદગી (length_size) {
                સ્થિતિ 1:
                        જો (iov->iov_len > UINT8_MAX)
                                પાછા -ERANGE;

                        *p = iov->iov_len;
                        તોડો;
                સ્થિતિ 2:
                        જો (iov->iov_len > UINT16_MAX)
                                પાછા -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        તોડો;
                સ્થિતિ 4:
                        જો (iov->iov_len > UINT32_MAX)
                                પાછા -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        તોડો;
                મૂળભૂત:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        પ્રતિજ્ઞા(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        પાછા 0;
}
