/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "iovec-wrapper.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unaligned.h"

शून्य iovw_done(संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->गणनम् = 0;
}

शून्य iovw_done_free(संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गणनम्)
                iovec_done(i);

        iovw_done(iovw);
}

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw) {
        सचे (!iovw)
                पटिनिवत्त रिक्त;

        iovw_done(iovw);
        पटिनिवत्त mfree(iovw);
}

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw) {
        सचे (!iovw)
                पटिनिवत्त रिक्त;

        iovw_done_free(iovw);
        पटिनिवत्त mfree(iovw);
}

पूर्णाङ्क iovw_compare(ध्रुव संरचना iovec_wrapper *a, ध्रुव संरचना iovec_wrapper *b) {
        पूर्णाङ्क r;

        सचे (a == b)
                पटिनिवत्त 0;

        सचे (!a || !b)
                पटिनिवत्त CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        अत्थाय (size_t i = 0, n = MIN(a->गणनम्, b->गणनम्); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                सचे (r != 0)
                        पटिनिवत्त r;
        }

        पटिनिवत्त CMP(a->गणनम्, b->गणनम्);
}

पूर्णाङ्क iovw_put_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, size_t len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        सचे (len == 0 && !accept_zero)
                पटिनिवत्त 0;

        सचे (iovw->गणनम् >= IOV_MAX)
                पटिनिवत्त -E2BIG;

        सचे (!GREEDY_REALLOC(iovw->iovec, iovw->गणनम् + 1))
                पटिनिवत्त -ENOMEM;

        iovw->iovec[iovw->गणनम्++] = IOVEC_MAKE(data, len);
        पटिनिवत्त 1;
}

पूर्णाङ्क iovw_put_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        सचे (!iov)
                पटिनिवत्त 0;

        पटिनिवत्त iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णाङ्क iovw_put_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        सचे (iovw_isempty(source))
                पटिनिवत्त 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        सचे (iovw == source)
                पटिनिवत्त -EINVAL;

        सचे (iovw->गणनम् > SIZE_MAX - source->गणनम्)
                पटिनिवत्त -E2BIG;
        सचे (iovw->गणनम् + source->गणनम् > IOV_MAX)
                पटिनिवत्त -E2BIG;

        सचे (accept_zero) {
                सचे (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->गणनम्, source->iovec, source->गणनम्))
                        पटिनिवत्त -ENOMEM;

                पटिनिवत्त 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_गणनम् = iovw->गणनम्;

        FOREACH_ARRAY(iovec, source->iovec, source->गणनम्) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                सचे (r < 0)
                        गच्छ rollback;
        }

        पटिनिवत्त 0;

rollback:
        iovw->गणनम् = original_गणनम्;
        पटिनिवत्त r;
}

पूर्णाङ्क iovw_consume_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, size_t len) {
        /* Move data into iovw or free on error */
        पूर्णाङ्क r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        सचे (r <= 0)
                मुक्त(data);

        पटिनिवत्त r;
}

पूर्णाङ्क iovw_consume_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        सचे (!iov)
                पटिनिवत्त 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        सचे (r <= 0)
                iovec_done(iov);
        अञ्ञथा
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (संरचना iovec) {};

        पटिनिवत्त r;
}

पूर्णाङ्क iovw_extend_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव शून्य *data, size_t len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        सचे (len == 0)
                पटिनिवत्त iovw_put_full(iovw, accept_zero, /* data= */ रिक्त, /* len= */ 0);

        शून्य *c = memdup(data, len);
        सचे (!c)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त iovw_consume_full(iovw, accept_zero, c, len);
}

पूर्णाङ्क iovw_extend_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        सचे (!iov)
                पटिनिवत्त 0;

        पटिनिवत्त iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णाङ्क iovw_extend_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        /* This duplicates the source and merges it into the iovw. */

        सचे (iovw_isempty(source))
                पटिनिवत्त 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        सचे (iovw == source)
                पटिनिवत्त -EINVAL;

        सचे (iovw->गणनम् > SIZE_MAX - source->गणनम्)
                पटिनिवत्त -E2BIG;
        सचे (iovw->गणनम् + source->गणनम् > IOV_MAX)
                पटिनिवत्त -E2BIG;

        size_t original_गणनम् = iovw->गणनम्;

        FOREACH_ARRAY(iovec, source->iovec, source->गणनम्) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                सचे (r < 0)
                        गच्छ rollback;
        }

        पटिनिवत्त 0;

rollback:
        अत्थाय (size_t i = original_गणनम्; i < iovw->गणनम्; i++)
                iovec_done(iovw->iovec + i);

        iovw->गणनम् = original_गणनम्;
        पटिनिवत्त r;
}

पूर्णाङ्क iovw_put_string_field_full(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *field, ध्रुव वर्ण *मानम्) {
        _cleanup_free_ वर्ण *x = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        x = strjoin(field, मानम्);
        सचे (!x)
                पटिनिवत्त -ENOMEM;

        सचे (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->गणनम्)
                        सचे (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                पटिनिवत्त free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        सचे (r >= 0)
                TAKE_PTR(x);

        पटिनिवत्त r;
}

पूर्णाङ्क iovw_put_string_fieldf_full(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *field, ध्रुव वर्ण *format, ...) {
        _cleanup_free_ वर्ण *मानम् = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(format);

        va_start(ap, format);
        r = vasprintf(&मानम्, format, ap);
        va_end(ap);
        सचे (r < 0)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त iovw_put_string_field_full(iovw, replace, field, मानम्);
}

पूर्णाङ्क iovw_put_string_field_free(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *field, वर्ण *मानम्) {
        _cleanup_free_ _unused_ वर्ण *free_ptr = मानम्;

        पटिनिवत्त iovw_put_string_field(iovw, field, मानम्);
}

शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गणनम्) {
                प्रतिज्ञा(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ध्रुव संरचना iovec_wrapper *iovw) {
        सचे (iovw_isempty(iovw))
                पटिनिवत्त 0;

        पटिनिवत्त iovec_total_size(iovw->iovec, iovw->गणनम्);
}

पूर्णाङ्क iovw_concat(ध्रुव संरचना iovec_wrapper *iovw, संरचना iovec *ret) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        सचे (len == SIZE_MAX)
                पटिनिवत्त -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        सचे (!buf)
                पटिनिवत्त -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->गणनम्)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        पटिनिवत्त 0;
}

वर्ण* iovw_to_cstring(ध्रुव संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) संरचना iovec iov = {};
        सचे (iovw_concat(iovw, &iov) < 0)
                पटिनिवत्त रिक्त;

        प्रतिज्ञा(!memchr(iov.iov_base, 0, iov.iov_len));
        पटिनिवत्त TAKE_PTR(iov.iov_base);
}

पूर्णाङ्क iovec_split(ध्रुव संरचना iovec *iov, size_t length_size, संरचना iovec_wrapper *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        सचे (!iovec_is_set(iov)) {
                *ret = (संरचना iovec_wrapper) {};
                पटिनिवत्त 0;
        }

        _cleanup_(iovw_done_free) संरचना iovec_wrapper iovw = {};
        अत्थाय (संरचना iovec i = *iov; iovec_is_set(&i); ) {
                सचे (i.iov_len < length_size)
                        पटिनिवत्त -EBADMSG;

                size_t len;
                विकल्प (length_size) {
                पक्ष 1:
                        len = *(uint8_t*) i.iov_base;
                        भङ्ग;
                पक्ष 2:
                        len = unaligned_read_be16(i.iov_base);
                        भङ्ग;
                पक्ष 4:
                        len = unaligned_read_be32(i.iov_base);
                        भङ्ग;
                स्वभाव:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                सचे (len == 0)
                        अनुवर्त;

                सचे (i.iov_len < len)
                        पटिनिवत्त -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                सचे (r < 0)
                        पटिनिवत्त r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        पटिनिवत्त 0;
}

पूर्णाङ्क iovw_merge(ध्रुव संरचना iovec_wrapper *iovw, size_t length_size, संरचना iovec *ret) {
        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        सचे (sz == 0) {
                *ret = (संरचना iovec) {};
                पटिनिवत्त 0;
        }
        सचे (sz == SIZE_MAX)
                पटिनिवत्त -E2BIG;

        सचे (size_multiply_overflow(length_size, iovw->गणनम्))
                पटिनिवत्त -E2BIG;

        sz = size_add(sz, iovw->गणनम् * length_size);
        सचे (sz == SIZE_MAX)
                पटिनिवत्त -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        सचे (!buf)
                पटिनिवत्त -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->गणनम्) {
                सचे (iov->iov_len == 0)
                        अनुवर्त;

                विकल्प (length_size) {
                पक्ष 1:
                        सचे (iov->iov_len > UINT8_MAX)
                                पटिनिवत्त -ERANGE;

                        *p = iov->iov_len;
                        भङ्ग;
                पक्ष 2:
                        सचे (iov->iov_len > UINT16_MAX)
                                पटिनिवत्त -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        भङ्ग;
                पक्ष 4:
                        सचे (iov->iov_len > UINT32_MAX)
                                पटिनिवत्त -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        भङ्ग;
                स्वभाव:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        प्रतिज्ञा(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        पटिनिवत्त 0;
}
