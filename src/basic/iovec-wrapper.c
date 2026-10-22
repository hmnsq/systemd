/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-wrapper.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "unaligned.h"

ଶୂନ୍ୟ iovw_done(ସଂରଚନା iovec_wrapper *iovw) {
        ପ୍ରତିଜ୍ଞା(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->ଗଣନା_2 = 0;
}

ଶୂନ୍ୟ iovw_done_free(ସଂରଚନା iovec_wrapper *iovw) {
        ପ୍ରତିଜ୍ଞା(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ଗଣନା_2)
                iovec_done(i);

        iovw_done(iovw);
}

ସଂରଚନା iovec_wrapper* iovw_free(ସଂରଚନା iovec_wrapper *iovw) {
        ଯଦି (!iovw)
                ଫେରନ୍ତୁ ଖାଲି;

        iovw_done(iovw);
        ଫେରନ୍ତୁ mfree(iovw);
}

ସଂରଚନା iovec_wrapper* iovw_free_free(ସଂରଚନା iovec_wrapper *iovw) {
        ଯଦି (!iovw)
                ଫେରନ୍ତୁ ଖାଲି;

        iovw_done_free(iovw);
        ଫେରନ୍ତୁ mfree(iovw);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_compare(ଅଚଳ ସଂରଚନା iovec_wrapper *a, ଅଚଳ ସଂରଚନା iovec_wrapper *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (a == b)
                ଫେରନ୍ତୁ 0;

        ଯଦି (!a || !b)
                ଫେରନ୍ତୁ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        ପାଇଁ (size_t i = 0, n = MIN(a->ଗଣନା_2, b->ଗଣନା_2); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                ଯଦି (r != 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ CMP(a->ଗଣନା_2, b->ଗଣନା_2);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଶୂନ୍ୟ *data, size_t len) {
        ପ୍ରତିଜ୍ଞା(iovw);
        ପ୍ରତିଜ୍ଞା(data || len == 0);

        ଯଦି (len == 0 && !accept_zero)
                ଫେରନ୍ତୁ 0;

        ଯଦି (iovw->ଗଣନା_2 >= IOV_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        ଯଦି (!GREEDY_REALLOC(iovw->iovec, iovw->ଗଣନା_2 + 1))
                ଫେରନ୍ତୁ -ENOMEM;

        iovw->iovec[iovw->ଗଣନା_2++] = IOVEC_MAKE(data, len);
        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_iov_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec *iov) {
        ପ୍ରତିଜ୍ଞା(iovw);

        ଯଦି (!iov)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_iovw_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec_wrapper *source) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(iovw);

        ଯଦି (iovw_isempty(source))
                ଫେରନ୍ତୁ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        ଯଦି (iovw == source)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (iovw->ଗଣନା_2 > SIZE_MAX - source->ଗଣନା_2)
                ଫେରନ୍ତୁ -E2BIG;
        ଯଦି (iovw->ଗଣନା_2 + source->ଗଣନା_2 > IOV_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        ଯଦି (accept_zero) {
                ଯଦି (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->ଗଣନା_2, source->iovec, source->ଗଣନା_2))
                        ଫେରନ୍ତୁ -ENOMEM;

                ଫେରନ୍ତୁ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_ଗଣନା = iovw->ଗଣନା_2;

        FOREACH_ARRAY(iovec, source->iovec, source->ଗଣନା_2) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                ଯଦି (r < 0)
                        ଯାଅ rollback;
        }

        ଫେରନ୍ତୁ 0;

rollback:
        iovw->ଗଣନା_2 = original_ଗଣନା;
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_consume_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଶୂନ୍ୟ *data, size_t len) {
        /* Move data into iovw or free on error */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        ଯଦି (r <= 0)
                ମୁକ୍ତ(data);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_consume_iov_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ସଂରଚନା iovec *iov) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(iovw);

        ଯଦି (!iov)
                ଫେରନ୍ତୁ 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        ଯଦି (r <= 0)
                iovec_done(iov);
        ଅନ୍ୟଥା
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (ସଂରଚନା iovec) {};

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ଶୂନ୍ୟ *data, size_t len) {
        ପ୍ରତିଜ୍ଞା(iovw);
        ପ୍ରତିଜ୍ଞା(data || len == 0);

        ଯଦି (len == 0)
                ଫେରନ୍ତୁ iovw_put_full(iovw, accept_zero, /* data= */ ଖାଲି, /* len= */ 0);

        ଶୂନ୍ୟ *c = memdup(data, len);
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ iovw_consume_full(iovw, accept_zero, c, len);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_iov_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec *iov) {
        ପ୍ରତିଜ୍ଞା(iovw);

        ଯଦି (!iov)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_iovw_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec_wrapper *source) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(iovw);

        /* This duplicates the source and merges it into the iovw. */

        ଯଦି (iovw_isempty(source))
                ଫେରନ୍ତୁ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        ଯଦି (iovw == source)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (iovw->ଗଣନା_2 > SIZE_MAX - source->ଗଣନା_2)
                ଫେରନ୍ତୁ -E2BIG;
        ଯଦି (iovw->ଗଣନା_2 + source->ଗଣନା_2 > IOV_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        size_t original_ଗଣନା = iovw->ଗଣନା_2;

        FOREACH_ARRAY(iovec, source->iovec, source->ଗଣନା_2) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                ଯଦି (r < 0)
                        ଯାଅ rollback;
        }

        ଫେରନ୍ତୁ 0;

rollback:
        ପାଇଁ (size_t i = original_ଗଣନା; i < iovw->ଗଣନା_2; i++)
                iovec_done(iovw->iovec + i);

        iovw->ଗଣନା_2 = original_ଗଣନା;
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_string_field_full(ସଂରଚନା iovec_wrapper *iovw, bool replace, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *x = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(iovw);

        x = strjoin(field, ମୂଲ୍ୟ);
        ଯଦି (!x)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->ଗଣନା_2)
                        ଯଦି (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                ଫେରନ୍ତୁ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        ଯଦି (r >= 0)
                TAKE_PTR(x);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_string_fieldf_full(ସଂରଚନା iovec_wrapper *iovw, bool replace, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *format, ...) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(format);

        va_start(ap, format);
        r = vasprintf(&ମୂଲ୍ୟ, format, ap);
        va_end(ap);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ iovw_put_string_field_full(iovw, replace, field, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_string_field_free(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ଅକ୍ଷର *field, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        _cleanup_free_ _unused_ ଅକ୍ଷର *free_ptr = ମୂଲ୍ୟ;

        ଫେରନ୍ତୁ iovw_put_string_field(iovw, field, ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ iovw_rebase(ସଂରଚନା iovec_wrapper *iovw, ଶୂନ୍ୟ *old, ଶୂନ୍ୟ *new) {
        ପ୍ରତିଜ୍ଞା(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ଗଣନା_2) {
                ପ୍ରତିଜ୍ଞା(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw) {
        ଯଦି (iovw_isempty(iovw))
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ iovec_total_size(iovw->iovec, iovw->ଗଣନା_2);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_concat(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw, ସଂରଚନା iovec *ret) {
        ପ୍ରତିଜ୍ଞା(iovw);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        ଯଦି (len == SIZE_MAX)
                ଫେରନ୍ତୁ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->ଗଣନା_2)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* iovw_to_cstring(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw) {
        ପ୍ରତିଜ୍ଞା(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) ସଂରଚନା iovec iov = {};
        ଯଦି (iovw_concat(iovw, &iov) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(!memchr(iov.iov_base, 0, iov.iov_len));
        ଫେରନ୍ତୁ TAKE_PTR(iov.iov_base);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_split(ଅଚଳ ସଂରଚନା iovec *iov, size_t length_size, ସଂରଚନା iovec_wrapper *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(IN_SET(length_size, 1, 2, 4));
        ପ୍ରତିଜ୍ଞା(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        ଯଦି (!iovec_is_set(iov)) {
                *ret = (ସଂରଚନା iovec_wrapper) {};
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_(iovw_done_free) ସଂରଚନା iovec_wrapper iovw = {};
        ପାଇଁ (ସଂରଚନା iovec i = *iov; iovec_is_set(&i); ) {
                ଯଦି (i.iov_len < length_size)
                        ଫେରନ୍ତୁ -EBADMSG;

                size_t len;
                ବାଛ (length_size) {
                ସ୍ଥିତି 1:
                        len = *(uint8_t*) i.iov_base;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି 2:
                        len = unaligned_read_be16(i.iov_base);
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି 4:
                        len = unaligned_read_be32(i.iov_base);
                        ଭାଙ୍ଗ;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                ଯଦି (len == 0)
                        ଜାରି;

                ଯଦି (i.iov_len < len)
                        ଫେରନ୍ତୁ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_merge(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw, size_t length_size, ସଂରଚନା iovec *ret) {
        ପ୍ରତିଜ୍ଞା(IN_SET(length_size, 1, 2, 4));
        ପ୍ରତିଜ୍ଞା(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        ଯଦି (sz == 0) {
                *ret = (ସଂରଚନା iovec) {};
                ଫେରନ୍ତୁ 0;
        }
        ଯଦି (sz == SIZE_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        ଯଦି (size_multiply_overflow(length_size, iovw->ଗଣନା_2))
                ଫେରନ୍ତୁ -E2BIG;

        sz = size_add(sz, iovw->ଗଣନା_2 * length_size);
        ଯଦି (sz == SIZE_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->ଗଣନା_2) {
                ଯଦି (iov->iov_len == 0)
                        ଜାରି;

                ବାଛ (length_size) {
                ସ୍ଥିତି 1:
                        ଯଦି (iov->iov_len > UINT8_MAX)
                                ଫେରନ୍ତୁ -ERANGE;

                        *p = iov->iov_len;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି 2:
                        ଯଦି (iov->iov_len > UINT16_MAX)
                                ଫେରନ୍ତୁ -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି 4:
                        ଯଦି (iov->iov_len > UINT32_MAX)
                                ଫେରନ୍ତୁ -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        ଭାଙ୍ଗ;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        ପ୍ରତିଜ୍ଞା(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        ଫେରନ୍ତୁ 0;
}
