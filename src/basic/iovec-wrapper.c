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
        iovw->गिनती = 0;
}

शून्य iovw_done_free(संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गिनती)
                iovec_done(i);

        iovw_done(iovw);
}

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw) {
        यदि (!iovw)
                प्रत्यागम रिक्त;

        iovw_done(iovw);
        प्रत्यागम mfree(iovw);
}

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw) {
        यदि (!iovw)
                प्रत्यागम रिक्त;

        iovw_done_free(iovw);
        प्रत्यागम mfree(iovw);
}

पूर्णाङ्क iovw_compare(ध्रुव संरचना iovec_wrapper *a, ध्रुव संरचना iovec_wrapper *b) {
        पूर्णाङ्क r;

        यदि (a == b)
                प्रत्यागम 0;

        यदि (!a || !b)
                प्रत्यागम CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        प्रति (size_t i = 0, n = MIN(a->गिनती, b->गिनती); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                यदि (r != 0)
                        प्रत्यागम r;
        }

        प्रत्यागम CMP(a->गिनती, b->गिनती);
}

पूर्णाङ्क iovw_put_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, size_t len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        यदि (len == 0 && !accept_zero)
                प्रत्यागम 0;

        यदि (iovw->गिनती >= IOV_MAX)
                प्रत्यागम -E2BIG;

        यदि (!GREEDY_REALLOC(iovw->iovec, iovw->गिनती + 1))
                प्रत्यागम -ENOMEM;

        iovw->iovec[iovw->गिनती++] = IOVEC_MAKE(data, len);
        प्रत्यागम 1;
}

पूर्णाङ्क iovw_put_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        यदि (!iov)
                प्रत्यागम 0;

        प्रत्यागम iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णाङ्क iovw_put_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        यदि (iovw_isempty(source))
                प्रत्यागम 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        यदि (iovw == source)
                प्रत्यागम -EINVAL;

        यदि (iovw->गिनती > SIZE_MAX - source->गिनती)
                प्रत्यागम -E2BIG;
        यदि (iovw->गिनती + source->गिनती > IOV_MAX)
                प्रत्यागम -E2BIG;

        यदि (accept_zero) {
                यदि (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->गिनती, source->iovec, source->गिनती))
                        प्रत्यागम -ENOMEM;

                प्रत्यागम 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_गिनती = iovw->गिनती;

        FOREACH_ARRAY(iovec, source->iovec, source->गिनती) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                यदि (r < 0)
                        गच्छ rollback;
        }

        प्रत्यागम 0;

rollback:
        iovw->गिनती = original_गिनती;
        प्रत्यागम r;
}

पूर्णाङ्क iovw_consume_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, size_t len) {
        /* Move data into iovw or free on error */
        पूर्णाङ्क r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        यदि (r <= 0)
                मुक्त(data);

        प्रत्यागम r;
}

पूर्णाङ्क iovw_consume_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        यदि (!iov)
                प्रत्यागम 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        यदि (r <= 0)
                iovec_done(iov);
        अन्यथा
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (संरचना iovec) {};

        प्रत्यागम r;
}

पूर्णाङ्क iovw_extend_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव शून्य *data, size_t len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        यदि (len == 0)
                प्रत्यागम iovw_put_full(iovw, accept_zero, /* data= */ रिक्त, /* len= */ 0);

        शून्य *c = memdup(data, len);
        यदि (!c)
                प्रत्यागम -ENOMEM;

        प्रत्यागम iovw_consume_full(iovw, accept_zero, c, len);
}

पूर्णाङ्क iovw_extend_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        यदि (!iov)
                प्रत्यागम 0;

        प्रत्यागम iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णाङ्क iovw_extend_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        /* This duplicates the source and merges it into the iovw. */

        यदि (iovw_isempty(source))
                प्रत्यागम 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        यदि (iovw == source)
                प्रत्यागम -EINVAL;

        यदि (iovw->गिनती > SIZE_MAX - source->गिनती)
                प्रत्यागम -E2BIG;
        यदि (iovw->गिनती + source->गिनती > IOV_MAX)
                प्रत्यागम -E2BIG;

        size_t original_गिनती = iovw->गिनती;

        FOREACH_ARRAY(iovec, source->iovec, source->गिनती) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                यदि (r < 0)
                        गच्छ rollback;
        }

        प्रत्यागम 0;

rollback:
        प्रति (size_t i = original_गिनती; i < iovw->गिनती; i++)
                iovec_done(iovw->iovec + i);

        iovw->गिनती = original_गिनती;
        प्रत्यागम r;
}

पूर्णाङ्क iovw_put_string_field_full(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *field, ध्रुव वर्ण *मूल्य) {
        _cleanup_free_ वर्ण *x = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        x = strjoin(field, मूल्य);
        यदि (!x)
                प्रत्यागम -ENOMEM;

        यदि (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->गिनती)
                        यदि (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                प्रत्यागम free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        यदि (r >= 0)
                TAKE_PTR(x);

        प्रत्यागम r;
}

पूर्णाङ्क iovw_put_string_fieldf_full(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *field, ध्रुव वर्ण *format, ...) {
        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(format);

        va_start(ap, format);
        r = vasprintf(&मूल्य, format, ap);
        va_end(ap);
        यदि (r < 0)
                प्रत्यागम -ENOMEM;

        प्रत्यागम iovw_put_string_field_full(iovw, replace, field, मूल्य);
}

पूर्णाङ्क iovw_put_string_field_free(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *field, वर्ण *मूल्य) {
        _cleanup_free_ _unused_ वर्ण *free_ptr = मूल्य;

        प्रत्यागम iovw_put_string_field(iovw, field, मूल्य);
}

शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गिनती) {
                प्रतिज्ञा(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(ध्रुव संरचना iovec_wrapper *iovw) {
        यदि (iovw_isempty(iovw))
                प्रत्यागम 0;

        प्रत्यागम iovec_total_size(iovw->iovec, iovw->गिनती);
}

पूर्णाङ्क iovw_concat(ध्रुव संरचना iovec_wrapper *iovw, संरचना iovec *ret) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        यदि (len == SIZE_MAX)
                प्रत्यागम -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->गिनती)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        प्रत्यागम 0;
}

वर्ण* iovw_to_cstring(ध्रुव संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) संरचना iovec iov = {};
        यदि (iovw_concat(iovw, &iov) < 0)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(!memchr(iov.iov_base, 0, iov.iov_len));
        प्रत्यागम TAKE_PTR(iov.iov_base);
}

पूर्णाङ्क iovec_split(ध्रुव संरचना iovec *iov, size_t length_size, संरचना iovec_wrapper *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        यदि (!iovec_is_set(iov)) {
                *ret = (संरचना iovec_wrapper) {};
                प्रत्यागम 0;
        }

        _cleanup_(iovw_done_free) संरचना iovec_wrapper iovw = {};
        प्रति (संरचना iovec i = *iov; iovec_is_set(&i); ) {
                यदि (i.iov_len < length_size)
                        प्रत्यागम -EBADMSG;

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

                यदि (len == 0)
                        अनुवर्त;

                यदि (i.iov_len < len)
                        प्रत्यागम -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                यदि (r < 0)
                        प्रत्यागम r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        प्रत्यागम 0;
}

पूर्णाङ्क iovw_merge(ध्रुव संरचना iovec_wrapper *iovw, size_t length_size, संरचना iovec *ret) {
        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        यदि (sz == 0) {
                *ret = (संरचना iovec) {};
                प्रत्यागम 0;
        }
        यदि (sz == SIZE_MAX)
                प्रत्यागम -E2BIG;

        यदि (size_multiply_overflow(length_size, iovw->गिनती))
                प्रत्यागम -E2BIG;

        sz = size_add(sz, iovw->गिनती * length_size);
        यदि (sz == SIZE_MAX)
                प्रत्यागम -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->गिनती) {
                यदि (iov->iov_len == 0)
                        अनुवर्त;

                विकल्प (length_size) {
                पक्ष 1:
                        यदि (iov->iov_len > UINT8_MAX)
                                प्रत्यागम -ERANGE;

                        *p = iov->iov_len;
                        भङ्ग;
                पक्ष 2:
                        यदि (iov->iov_len > UINT16_MAX)
                                प्रत्यागम -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        भङ्ग;
                पक्ष 4:
                        यदि (iov->iov_len > UINT32_MAX)
                                प्रत्यागम -ERANGE;

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
        प्रत्यागम 0;
}
