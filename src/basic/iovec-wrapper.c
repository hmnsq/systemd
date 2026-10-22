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
        iovw->गन्ती = 0;
}

शून्य iovw_done_free(संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गन्ती)
                iovec_done(i);

        iovw_done(iovw);
}

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw) {
        यदि (!iovw)
                फर्काऊ रिक्त;

        iovw_done(iovw);
        फर्काऊ mfree(iovw);
}

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw) {
        यदि (!iovw)
                फर्काऊ रिक्त;

        iovw_done_free(iovw);
        फर्काऊ mfree(iovw);
}

पूर्णाङ्क iovw_compare(अचर संरचना iovec_wrapper *a, अचर संरचना iovec_wrapper *b) {
        पूर्णाङ्क r;

        यदि (a == b)
                फर्काऊ 0;

        यदि (!a || !b)
                फर्काऊ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        लागि (माप_प्रकार i = 0, n = MIN(a->गन्ती, b->गन्ती); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                यदि (r != 0)
                        फर्काऊ r;
        }

        फर्काऊ CMP(a->गन्ती, b->गन्ती);
}

पूर्णाङ्क iovw_put_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        यदि (len == 0 && !accept_zero)
                फर्काऊ 0;

        यदि (iovw->गन्ती >= IOV_MAX)
                फर्काऊ -E2BIG;

        यदि (!GREEDY_REALLOC(iovw->iovec, iovw->गन्ती + 1))
                फर्काऊ -ENOMEM;

        iovw->iovec[iovw->गन्ती++] = IOVEC_MAKE(data, len);
        फर्काऊ 1;
}

पूर्णाङ्क iovw_put_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        यदि (!iov)
                फर्काऊ 0;

        फर्काऊ iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णाङ्क iovw_put_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec_wrapper *source) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        यदि (iovw_isempty(source))
                फर्काऊ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        यदि (iovw == source)
                फर्काऊ -EINVAL;

        यदि (iovw->गन्ती > SIZE_MAX - source->गन्ती)
                फर्काऊ -E2BIG;
        यदि (iovw->गन्ती + source->गन्ती > IOV_MAX)
                फर्काऊ -E2BIG;

        यदि (accept_zero) {
                यदि (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->गन्ती, source->iovec, source->गन्ती))
                        फर्काऊ -ENOMEM;

                फर्काऊ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        माप_प्रकार original_गन्ती = iovw->गन्ती;

        FOREACH_ARRAY(iovec, source->iovec, source->गन्ती) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                यदि (r < 0)
                        गच्छ rollback;
        }

        फर्काऊ 0;

rollback:
        iovw->गन्ती = original_गन्ती;
        फर्काऊ r;
}

पूर्णाङ्क iovw_consume_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len) {
        /* Move data into iovw or free on error */
        पूर्णाङ्क r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        यदि (r <= 0)
                मुक्त(data);

        फर्काऊ r;
}

पूर्णाङ्क iovw_consume_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        यदि (!iov)
                फर्काऊ 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        यदि (r <= 0)
                iovec_done(iov);
        अन्यथा
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (संरचना iovec) {};

        फर्काऊ r;
}

पूर्णाङ्क iovw_extend_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर शून्य *data, माप_प्रकार len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        यदि (len == 0)
                फर्काऊ iovw_put_full(iovw, accept_zero, /* data= */ रिक्त, /* len= */ 0);

        शून्य *c = memdup(data, len);
        यदि (!c)
                फर्काऊ -ENOMEM;

        फर्काऊ iovw_consume_full(iovw, accept_zero, c, len);
}

पूर्णाङ्क iovw_extend_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        यदि (!iov)
                फर्काऊ 0;

        फर्काऊ iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णाङ्क iovw_extend_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec_wrapper *source) {
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        /* This duplicates the source and merges it into the iovw. */

        यदि (iovw_isempty(source))
                फर्काऊ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        यदि (iovw == source)
                फर्काऊ -EINVAL;

        यदि (iovw->गन्ती > SIZE_MAX - source->गन्ती)
                फर्काऊ -E2BIG;
        यदि (iovw->गन्ती + source->गन्ती > IOV_MAX)
                फर्काऊ -E2BIG;

        माप_प्रकार original_गन्ती = iovw->गन्ती;

        FOREACH_ARRAY(iovec, source->iovec, source->गन्ती) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                यदि (r < 0)
                        गच्छ rollback;
        }

        फर्काऊ 0;

rollback:
        लागि (माप_प्रकार i = original_गन्ती; i < iovw->गन्ती; i++)
                iovec_done(iovw->iovec + i);

        iovw->गन्ती = original_गन्ती;
        फर्काऊ r;
}

पूर्णाङ्क iovw_put_string_field_full(संरचना iovec_wrapper *iovw, bool replace, अचर वर्ण *field, अचर वर्ण *मान) {
        _cleanup_free_ वर्ण *x = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(iovw);

        x = strjoin(field, मान);
        यदि (!x)
                फर्काऊ -ENOMEM;

        यदि (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->गन्ती)
                        यदि (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = म_माप(x);
                                फर्काऊ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, म_माप(x));
        यदि (r >= 0)
                TAKE_PTR(x);

        फर्काऊ r;
}

पूर्णाङ्क iovw_put_string_fieldf_full(संरचना iovec_wrapper *iovw, bool replace, अचर वर्ण *field, अचर वर्ण *format, ...) {
        _cleanup_free_ वर्ण *मान = रिक्त;
        बहु_सूची ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(format);

        बहु_शुरू(ap, format);
        r = vasprintf(&मान, format, ap);
        बहु_पूर्ण(ap);
        यदि (r < 0)
                फर्काऊ -ENOMEM;

        फर्काऊ iovw_put_string_field_full(iovw, replace, field, मान);
}

पूर्णाङ्क iovw_put_string_field_free(संरचना iovec_wrapper *iovw, अचर वर्ण *field, वर्ण *मान) {
        _cleanup_free_ _unused_ वर्ण *free_ptr = मान;

        फर्काऊ iovw_put_string_field(iovw, field, मान);
}

शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गन्ती) {
                प्रतिज्ञा(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

माप_प्रकार iovw_size(अचर संरचना iovec_wrapper *iovw) {
        यदि (iovw_isempty(iovw))
                फर्काऊ 0;

        फर्काऊ iovec_total_size(iovw->iovec, iovw->गन्ती);
}

पूर्णाङ्क iovw_concat(अचर संरचना iovec_wrapper *iovw, संरचना iovec *ret) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(ret);

        /* Squish a series of iovecs into a single iovec. */

        माप_प्रकार len = iovw_size(iovw);
        यदि (len == SIZE_MAX)
                फर्काऊ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = दो_स्मृति(len + 1);
        यदि (!buf)
                फर्काऊ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->गन्ती)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        फर्काऊ 0;
}

वर्ण* iovw_to_cstring(अचर संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) संरचना iovec iov = {};
        यदि (iovw_concat(iovw, &iov) < 0)
                फर्काऊ रिक्त;

        प्रतिज्ञा(!स_प्रथम(iov.iov_base, 0, iov.iov_len));
        फर्काऊ TAKE_PTR(iov.iov_base);
}

पूर्णाङ्क iovec_split(अचर संरचना iovec *iov, माप_प्रकार length_size, संरचना iovec_wrapper *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        यदि (!iovec_is_set(iov)) {
                *ret = (संरचना iovec_wrapper) {};
                फर्काऊ 0;
        }

        _cleanup_(iovw_done_free) संरचना iovec_wrapper iovw = {};
        लागि (संरचना iovec i = *iov; iovec_is_set(&i); ) {
                यदि (i.iov_len < length_size)
                        फर्काऊ -EBADMSG;

                माप_प्रकार len;
                विकल्प (length_size) {
                पक्ष 1:
                        len = *(uint8_t*) i.iov_base;
                        रोक;
                पक्ष 2:
                        len = unaligned_read_be16(i.iov_base);
                        रोक;
                पक्ष 4:
                        len = unaligned_read_be32(i.iov_base);
                        रोक;
                स्वभाव:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                यदि (len == 0)
                        जारी;

                यदि (i.iov_len < len)
                        फर्काऊ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                यदि (r < 0)
                        फर्काऊ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        फर्काऊ 0;
}

पूर्णाङ्क iovw_merge(अचर संरचना iovec_wrapper *iovw, माप_प्रकार length_size, संरचना iovec *ret) {
        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        माप_प्रकार sz = iovw_size(iovw);
        यदि (sz == 0) {
                *ret = (संरचना iovec) {};
                फर्काऊ 0;
        }
        यदि (sz == SIZE_MAX)
                फर्काऊ -E2BIG;

        यदि (size_multiply_overflow(length_size, iovw->गन्ती))
                फर्काऊ -E2BIG;

        sz = size_add(sz, iovw->गन्ती * length_size);
        यदि (sz == SIZE_MAX)
                फर्काऊ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        यदि (!buf)
                फर्काऊ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->गन्ती) {
                यदि (iov->iov_len == 0)
                        जारी;

                विकल्प (length_size) {
                पक्ष 1:
                        यदि (iov->iov_len > UINT8_MAX)
                                फर्काऊ -दुस्फल;

                        *p = iov->iov_len;
                        रोक;
                पक्ष 2:
                        यदि (iov->iov_len > UINT16_MAX)
                                फर्काऊ -दुस्फल;

                        unaligned_write_be16(p, iov->iov_len);
                        रोक;
                पक्ष 4:
                        यदि (iov->iov_len > UINT32_MAX)
                                फर्काऊ -दुस्फल;

                        unaligned_write_be32(p, iov->iov_len);
                        रोक;
                स्वभाव:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        प्रतिज्ञा(sz >= (माप_प्रकार) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        फर्काऊ 0;
}
