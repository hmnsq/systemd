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
        iovw->मोजणी = 0;
}

शून्य iovw_done_free(संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->मोजणी)
                iovec_done(i);

        iovw_done(iovw);
}

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw) {
        जर (!iovw)
                परत रिक्त;

        iovw_done(iovw);
        परत mfree(iovw);
}

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw) {
        जर (!iovw)
                परत रिक्त;

        iovw_done_free(iovw);
        परत mfree(iovw);
}

पूर्णांक iovw_compare(स्थिरांक संरचना iovec_wrapper *a, स्थिरांक संरचना iovec_wrapper *b) {
        पूर्णांक r;

        जर (a == b)
                परत 0;

        जर (!a || !b)
                परत CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        साठी (माप_प्रकार i = 0, n = MIN(a->मोजणी, b->मोजणी); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                जर (r != 0)
                        परत r;
        }

        परत CMP(a->मोजणी, b->मोजणी);
}

पूर्णांक iovw_put_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        जर (len == 0 && !accept_zero)
                परत 0;

        जर (iovw->मोजणी >= IOV_MAX)
                परत -E2BIG;

        जर (!GREEDY_REALLOC(iovw->iovec, iovw->मोजणी + 1))
                परत -ENOMEM;

        iovw->iovec[iovw->मोजणी++] = IOVEC_MAKE(data, len);
        परत 1;
}

पूर्णांक iovw_put_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        जर (!iov)
                परत 0;

        परत iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णांक iovw_put_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec_wrapper *source) {
        पूर्णांक r;

        प्रतिज्ञा(iovw);

        जर (iovw_isempty(source))
                परत 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        जर (iovw == source)
                परत -EINVAL;

        जर (iovw->मोजणी > SIZE_MAX - source->मोजणी)
                परत -E2BIG;
        जर (iovw->मोजणी + source->मोजणी > IOV_MAX)
                परत -E2BIG;

        जर (accept_zero) {
                जर (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->मोजणी, source->iovec, source->मोजणी))
                        परत -ENOMEM;

                परत 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        माप_प्रकार original_मोजणी = iovw->मोजणी;

        FOREACH_ARRAY(iovec, source->iovec, source->मोजणी) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                जर (r < 0)
                        जा rollback;
        }

        परत 0;

rollback:
        iovw->मोजणी = original_मोजणी;
        परत r;
}

पूर्णांक iovw_consume_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len) {
        /* Move data into iovw or free on error */
        पूर्णांक r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        जर (r <= 0)
                मुक्त(data);

        परत r;
}

पूर्णांक iovw_consume_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov) {
        पूर्णांक r;

        प्रतिज्ञा(iovw);

        जर (!iov)
                परत 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        जर (r <= 0)
                iovec_done(iov);
        अन्यथा
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (संरचना iovec) {};

        परत r;
}

पूर्णांक iovw_extend_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक शून्य *data, माप_प्रकार len) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(data || len == 0);

        जर (len == 0)
                परत iovw_put_full(iovw, accept_zero, /* data= */ रिक्त, /* len= */ 0);

        शून्य *c = memdup(data, len);
        जर (!c)
                परत -ENOMEM;

        परत iovw_consume_full(iovw, accept_zero, c, len);
}

पूर्णांक iovw_extend_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec *iov) {
        प्रतिज्ञा(iovw);

        जर (!iov)
                परत 0;

        परत iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णांक iovw_extend_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec_wrapper *source) {
        पूर्णांक r;

        प्रतिज्ञा(iovw);

        /* This duplicates the source and merges it into the iovw. */

        जर (iovw_isempty(source))
                परत 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        जर (iovw == source)
                परत -EINVAL;

        जर (iovw->मोजणी > SIZE_MAX - source->मोजणी)
                परत -E2BIG;
        जर (iovw->मोजणी + source->मोजणी > IOV_MAX)
                परत -E2BIG;

        माप_प्रकार original_मोजणी = iovw->मोजणी;

        FOREACH_ARRAY(iovec, source->iovec, source->मोजणी) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                जर (r < 0)
                        जा rollback;
        }

        परत 0;

rollback:
        साठी (माप_प्रकार i = original_मोजणी; i < iovw->मोजणी; i++)
                iovec_done(iovw->iovec + i);

        iovw->मोजणी = original_मोजणी;
        परत r;
}

पूर्णांक iovw_put_string_field_full(संरचना iovec_wrapper *iovw, bool replace, स्थिरांक अक्षर *field, स्थिरांक अक्षर *मूल्य) {
        _cleanup_free_ अक्षर *x = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(iovw);

        x = strjoin(field, मूल्य);
        जर (!x)
                परत -ENOMEM;

        जर (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->मोजणी)
                        जर (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = म_माप(x);
                                परत free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, म_माप(x));
        जर (r >= 0)
                TAKE_PTR(x);

        परत r;
}

पूर्णांक iovw_put_string_fieldf_full(संरचना iovec_wrapper *iovw, bool replace, स्थिरांक अक्षर *field, स्थिरांक अक्षर *format, ...) {
        _cleanup_free_ अक्षर *मूल्य = रिक्त;
        बहु_सूची ap;
        पूर्णांक r;

        प्रतिज्ञा(format);

        बहु_शुरू(ap, format);
        r = vasprintf(&मूल्य, format, ap);
        बहु_पूर्ण(ap);
        जर (r < 0)
                परत -ENOMEM;

        परत iovw_put_string_field_full(iovw, replace, field, मूल्य);
}

पूर्णांक iovw_put_string_field_free(संरचना iovec_wrapper *iovw, स्थिरांक अक्षर *field, अक्षर *मूल्य) {
        _cleanup_free_ _unused_ अक्षर *free_ptr = मूल्य;

        परत iovw_put_string_field(iovw, field, मूल्य);
}

शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new) {
        प्रतिज्ञा(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->मोजणी) {
                प्रतिज्ञा(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

माप_प्रकार iovw_size(स्थिरांक संरचना iovec_wrapper *iovw) {
        जर (iovw_isempty(iovw))
                परत 0;

        परत iovec_total_size(iovw->iovec, iovw->मोजणी);
}

पूर्णांक iovw_concat(स्थिरांक संरचना iovec_wrapper *iovw, संरचना iovec *ret) {
        प्रतिज्ञा(iovw);
        प्रतिज्ञा(ret);

        /* Squish a series of iovecs into a single iovec. */

        माप_प्रकार len = iovw_size(iovw);
        जर (len == SIZE_MAX)
                परत -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = दो_स्मृति(len + 1);
        जर (!buf)
                परत -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->मोजणी)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        परत 0;
}

अक्षर* iovw_to_cstring(स्थिरांक संरचना iovec_wrapper *iovw) {
        प्रतिज्ञा(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) संरचना iovec iov = {};
        जर (iovw_concat(iovw, &iov) < 0)
                परत रिक्त;

        प्रतिज्ञा(!स_प्रथम(iov.iov_base, 0, iov.iov_len));
        परत TAKE_PTR(iov.iov_base);
}

पूर्णांक iovec_split(स्थिरांक संरचना iovec *iov, माप_प्रकार length_size, संरचना iovec_wrapper *ret) {
        पूर्णांक r;

        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        जर (!iovec_is_set(iov)) {
                *ret = (संरचना iovec_wrapper) {};
                परत 0;
        }

        _cleanup_(iovw_done_free) संरचना iovec_wrapper iovw = {};
        साठी (संरचना iovec i = *iov; iovec_is_set(&i); ) {
                जर (i.iov_len < length_size)
                        परत -EBADMSG;

                माप_प्रकार len;
                निवड (length_size) {
                स्थिती 1:
                        len = *(uint8_t*) i.iov_base;
                        थांबा;
                स्थिती 2:
                        len = unaligned_read_be16(i.iov_base);
                        थांबा;
                स्थिती 4:
                        len = unaligned_read_be32(i.iov_base);
                        थांबा;
                पूर्वनिर्धारित:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                जर (len == 0)
                        सुरूठेवा;

                जर (i.iov_len < len)
                        परत -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                जर (r < 0)
                        परत r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        परत 0;
}

पूर्णांक iovw_merge(स्थिरांक संरचना iovec_wrapper *iovw, माप_प्रकार length_size, संरचना iovec *ret) {
        प्रतिज्ञा(IN_SET(length_size, 1, 2, 4));
        प्रतिज्ञा(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        माप_प्रकार sz = iovw_size(iovw);
        जर (sz == 0) {
                *ret = (संरचना iovec) {};
                परत 0;
        }
        जर (sz == SIZE_MAX)
                परत -E2BIG;

        जर (size_multiply_overflow(length_size, iovw->मोजणी))
                परत -E2BIG;

        sz = size_add(sz, iovw->मोजणी * length_size);
        जर (sz == SIZE_MAX)
                परत -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        जर (!buf)
                परत -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->मोजणी) {
                जर (iov->iov_len == 0)
                        सुरूठेवा;

                निवड (length_size) {
                स्थिती 1:
                        जर (iov->iov_len > UINT8_MAX)
                                परत -दुस्फल;

                        *p = iov->iov_len;
                        थांबा;
                स्थिती 2:
                        जर (iov->iov_len > UINT16_MAX)
                                परत -दुस्फल;

                        unaligned_write_be16(p, iov->iov_len);
                        थांबा;
                स्थिती 4:
                        जर (iov->iov_len > UINT32_MAX)
                                परत -दुस्फल;

                        unaligned_write_be32(p, iov->iov_len);
                        थांबा;
                पूर्वनिर्धारित:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        प्रतिज्ञा(sz >= (माप_प्रकार) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        परत 0;
}
