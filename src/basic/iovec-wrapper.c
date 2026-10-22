/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "iovec-wrapper.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "unaligned.h"

শূন্য iovw_done(কাঠামো iovec_wrapper *iovw) {
        প্রতিজ্ঞা(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->গুনতি = 0;
}

শূন্য iovw_done_free(কাঠামো iovec_wrapper *iovw) {
        প্রতিজ্ঞা(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->গুনতি)
                iovec_done(i);

        iovw_done(iovw);
}

কাঠামো iovec_wrapper* iovw_free(কাঠামো iovec_wrapper *iovw) {
        যদি (!iovw)
                ফেরত ফাঁকা;

        iovw_done(iovw);
        ফেরত mfree(iovw);
}

কাঠামো iovec_wrapper* iovw_free_free(কাঠামো iovec_wrapper *iovw) {
        যদি (!iovw)
                ফেরত ফাঁকা;

        iovw_done_free(iovw);
        ফেরত mfree(iovw);
}

পূর্ণসংখ্যা iovw_compare(ধ্রুবক কাঠামো iovec_wrapper *a, ধ্রুবক কাঠামো iovec_wrapper *b) {
        পূর্ণসংখ্যা r;

        যদি (a == b)
                ফেরত 0;

        যদি (!a || !b)
                ফেরত CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        জন্য (size_t i = 0, n = MIN(a->গুনতি, b->গুনতি); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                যদি (r != 0)
                        ফেরত r;
        }

        ফেরত CMP(a->গুনতি, b->গুনতি);
}

পূর্ণসংখ্যা iovw_put_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, শূন্য *তথ্য, size_t len) {
        প্রতিজ্ঞা(iovw);
        প্রতিজ্ঞা(তথ্য || len == 0);

        যদি (len == 0 && !accept_zero)
                ফেরত 0;

        যদি (iovw->গুনতি >= IOV_MAX)
                ফেরত -E2BIG;

        যদি (!GREEDY_REALLOC(iovw->iovec, iovw->গুনতি + 1))
                ফেরত -ENOMEM;

        iovw->iovec[iovw->গুনতি++] = IOVEC_MAKE(তথ্য, len);
        ফেরত 1;
}

পূর্ণসংখ্যা iovw_put_iov_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec *iov) {
        প্রতিজ্ঞা(iovw);

        যদি (!iov)
                ফেরত 0;

        ফেরত iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

পূর্ণসংখ্যা iovw_put_iovw_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec_wrapper *source) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(iovw);

        যদি (iovw_isempty(source))
                ফেরত 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        যদি (iovw == source)
                ফেরত -EINVAL;

        যদি (iovw->গুনতি > SIZE_MAX - source->গুনতি)
                ফেরত -E2BIG;
        যদি (iovw->গুনতি + source->গুনতি > IOV_MAX)
                ফেরত -E2BIG;

        যদি (accept_zero) {
                যদি (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->গুনতি, source->iovec, source->গুনতি))
                        ফেরত -ENOMEM;

                ফেরত 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_গুনতি = iovw->গুনতি;

        FOREACH_ARRAY(iovec, source->iovec, source->গুনতি) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                যদি (r < 0)
                        যাও rollback;
        }

        ফেরত 0;

rollback:
        iovw->গুনতি = original_গুনতি;
        ফেরত r;
}

পূর্ণসংখ্যা iovw_consume_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, শূন্য *তথ্য, size_t len) {
        /* Move data into iovw or free on error */
        পূর্ণসংখ্যা r;

        r = iovw_put_full(iovw, accept_zero, তথ্য, len);
        যদি (r <= 0)
                মুক্ত(তথ্য);

        ফেরত r;
}

পূর্ণসংখ্যা iovw_consume_iov_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, কাঠামো iovec *iov) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(iovw);

        যদি (!iov)
                ফেরত 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        যদি (r <= 0)
                iovec_done(iov);
        অন্যথা
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (কাঠামো iovec) {};

        ফেরত r;
}

পূর্ণসংখ্যা iovw_extend_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক শূন্য *তথ্য, size_t len) {
        প্রতিজ্ঞা(iovw);
        প্রতিজ্ঞা(তথ্য || len == 0);

        যদি (len == 0)
                ফেরত iovw_put_full(iovw, accept_zero, /* data= */ ফাঁকা, /* len= */ 0);

        শূন্য *c = memdup(তথ্য, len);
        যদি (!c)
                ফেরত -ENOMEM;

        ফেরত iovw_consume_full(iovw, accept_zero, c, len);
}

পূর্ণসংখ্যা iovw_extend_iov_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec *iov) {
        প্রতিজ্ঞা(iovw);

        যদি (!iov)
                ফেরত 0;

        ফেরত iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

পূর্ণসংখ্যা iovw_extend_iovw_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec_wrapper *source) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(iovw);

        /* This duplicates the source and merges it into the iovw. */

        যদি (iovw_isempty(source))
                ফেরত 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        যদি (iovw == source)
                ফেরত -EINVAL;

        যদি (iovw->গুনতি > SIZE_MAX - source->গুনতি)
                ফেরত -E2BIG;
        যদি (iovw->গুনতি + source->গুনতি > IOV_MAX)
                ফেরত -E2BIG;

        size_t original_গুনতি = iovw->গুনতি;

        FOREACH_ARRAY(iovec, source->iovec, source->গুনতি) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                যদি (r < 0)
                        যাও rollback;
        }

        ফেরত 0;

rollback:
        জন্য (size_t i = original_গুনতি; i < iovw->গুনতি; i++)
                iovec_done(iovw->iovec + i);

        iovw->গুনতি = original_গুনতি;
        ফেরত r;
}

পূর্ণসংখ্যা iovw_put_string_field_full(কাঠামো iovec_wrapper *iovw, bool replace, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *মান) {
        _cleanup_free_ অক্ষর *x = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(iovw);

        x = strjoin(field, মান);
        যদি (!x)
                ফেরত -ENOMEM;

        যদি (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->গুনতি)
                        যদি (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                ফেরত free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        যদি (r >= 0)
                TAKE_PTR(x);

        ফেরত r;
}

পূর্ণসংখ্যা iovw_put_string_fieldf_full(কাঠামো iovec_wrapper *iovw, bool replace, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *format, ...) {
        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        va_list ap;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(format);

        va_start(ap, format);
        r = vasprintf(&মান, format, ap);
        va_end(ap);
        যদি (r < 0)
                ফেরত -ENOMEM;

        ফেরত iovw_put_string_field_full(iovw, replace, field, মান);
}

পূর্ণসংখ্যা iovw_put_string_field_free(কাঠামো iovec_wrapper *iovw, ধ্রুবক অক্ষর *field, অক্ষর *মান) {
        _cleanup_free_ _unused_ অক্ষর *free_ptr = মান;

        ফেরত iovw_put_string_field(iovw, field, মান);
}

শূন্য iovw_rebase(কাঠামো iovec_wrapper *iovw, শূন্য *old, শূন্য *new) {
        প্রতিজ্ঞা(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->গুনতি) {
                প্রতিজ্ঞা(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_আকার(ধ্রুবক কাঠামো iovec_wrapper *iovw) {
        যদি (iovw_isempty(iovw))
                ফেরত 0;

        ফেরত iovec_যোগ_আকার(iovw->iovec, iovw->গুনতি);
}

পূর্ণসংখ্যা iovw_concat(ধ্রুবক কাঠামো iovec_wrapper *iovw, কাঠামো iovec *ret) {
        প্রতিজ্ঞা(iovw);
        প্রতিজ্ঞা(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_আকার(iovw);
        যদি (len == SIZE_MAX)
                ফেরত -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        যদি (!buf)
                ফেরত -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->গুনতি)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ফেরত 0;
}

অক্ষর* iovw_to_cstring(ধ্রুবক কাঠামো iovec_wrapper *iovw) {
        প্রতিজ্ঞা(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) কাঠামো iovec iov = {};
        যদি (iovw_concat(iovw, &iov) < 0)
                ফেরত ফাঁকা;

        প্রতিজ্ঞা(!memchr(iov.iov_base, 0, iov.iov_len));
        ফেরত TAKE_PTR(iov.iov_base);
}

পূর্ণসংখ্যা iovec_split(ধ্রুবক কাঠামো iovec *iov, size_t দৈর্ঘ্য_আকার, কাঠামো iovec_wrapper *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(IN_SET(দৈর্ঘ্য_আকার, 1, 2, 4));
        প্রতিজ্ঞা(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        যদি (!iovec_is_set(iov)) {
                *ret = (কাঠামো iovec_wrapper) {};
                ফেরত 0;
        }

        _cleanup_(iovw_done_free) কাঠামো iovec_wrapper iovw = {};
        জন্য (কাঠামো iovec i = *iov; iovec_is_set(&i); ) {
                যদি (i.iov_len < দৈর্ঘ্য_আকার)
                        ফেরত -EBADMSG;

                size_t len;
                নির্বাচন (দৈর্ঘ্য_আকার) {
                ক্ষেত্র 1:
                        len = *(uint8_t*) i.iov_base;
                        বিরতি;
                ক্ষেত্র 2:
                        len = unaligned_read_be16(i.iov_base);
                        বিরতি;
                ক্ষেত্র 4:
                        len = unaligned_read_be32(i.iov_base);
                        বিরতি;
                পূর্বনির্ধারিত:
                        assert_not_reached();
                }

                iovec_inc(&i, দৈর্ঘ্য_আকার);

                যদি (len == 0)
                        চালু;

                যদি (i.iov_len < len)
                        ফেরত -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                যদি (r < 0)
                        ফেরত r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ফেরত 0;
}

পূর্ণসংখ্যা iovw_merge(ধ্রুবক কাঠামো iovec_wrapper *iovw, size_t দৈর্ঘ্য_আকার, কাঠামো iovec *ret) {
        প্রতিজ্ঞা(IN_SET(দৈর্ঘ্য_আকার, 1, 2, 4));
        প্রতিজ্ঞা(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_আকার(iovw);
        যদি (sz == 0) {
                *ret = (কাঠামো iovec) {};
                ফেরত 0;
        }
        যদি (sz == SIZE_MAX)
                ফেরত -E2BIG;

        যদি (আকার_multiply_overflow(দৈর্ঘ্য_আকার, iovw->গুনতি))
                ফেরত -E2BIG;

        sz = আকার_add(sz, iovw->গুনতি * দৈর্ঘ্য_আকার);
        যদি (sz == SIZE_MAX)
                ফেরত -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        যদি (!buf)
                ফেরত -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->গুনতি) {
                যদি (iov->iov_len == 0)
                        চালু;

                নির্বাচন (দৈর্ঘ্য_আকার) {
                ক্ষেত্র 1:
                        যদি (iov->iov_len > UINT8_MAX)
                                ফেরত -ERANGE;

                        *p = iov->iov_len;
                        বিরতি;
                ক্ষেত্র 2:
                        যদি (iov->iov_len > UINT16_MAX)
                                ফেরত -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        বিরতি;
                ক্ষেত্র 4:
                        যদি (iov->iov_len > UINT32_MAX)
                                ফেরত -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        বিরতি;
                পূর্বনির্ধারিত:
                        assert_not_reached();
                }
                p += দৈর্ঘ্য_আকার;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        প্রতিজ্ঞা(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        ফেরত 0;
}
