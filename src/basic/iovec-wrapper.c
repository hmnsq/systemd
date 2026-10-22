/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>

#समावेश "alloc-util.h"
#समावेश "iovec-util.h"
#समावेश "iovec-wrapper.h"
#समावेश "string-util.h"
#समावेश "unaligned.h"

व्योम iovw_done(काष्ठा iovec_wrapper *iovw) {
        निश्चित(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->गिनती = 0;
}

व्योम iovw_done_free(काष्ठा iovec_wrapper *iovw) {
        निश्चित(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गिनती)
                iovec_done(i);

        iovw_done(iovw);
}

काष्ठा iovec_wrapper* iovw_free(काष्ठा iovec_wrapper *iovw) {
        अगर (!iovw)
                वापस शून्य;

        iovw_done(iovw);
        वापस mfree(iovw);
}

काष्ठा iovec_wrapper* iovw_free_free(काष्ठा iovec_wrapper *iovw) {
        अगर (!iovw)
                वापस शून्य;

        iovw_done_free(iovw);
        वापस mfree(iovw);
}

पूर्णांक iovw_compare(स्थिर काष्ठा iovec_wrapper *a, स्थिर काष्ठा iovec_wrapper *b) {
        पूर्णांक r;

        अगर (a == b)
                वापस 0;

        अगर (!a || !b)
                वापस CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        क्रम (माप_प्रकार i = 0, n = MIN(a->गिनती, b->गिनती); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                अगर (r != 0)
                        वापस r;
        }

        वापस CMP(a->गिनती, b->गिनती);
}

पूर्णांक iovw_put_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, व्योम *तथ्य, माप_प्रकार len) {
        निश्चित(iovw);
        निश्चित(तथ्य || len == 0);

        अगर (len == 0 && !accept_zero)
                वापस 0;

        अगर (iovw->गिनती >= IOV_MAX)
                वापस -E2BIG;

        अगर (!GREEDY_REALLOC(iovw->iovec, iovw->गिनती + 1))
                वापस -ENOMEM;

        iovw->iovec[iovw->गिनती++] = IOVEC_MAKE(तथ्य, len);
        वापस 1;
}

पूर्णांक iovw_put_iov_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec *iov) {
        निश्चित(iovw);

        अगर (!iov)
                वापस 0;

        वापस iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णांक iovw_put_iovw_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec_wrapper *source) {
        पूर्णांक r;

        निश्चित(iovw);

        अगर (iovw_isempty(source))
                वापस 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        अगर (iovw == source)
                वापस -EINVAL;

        अगर (iovw->गिनती > SIZE_MAX - source->गिनती)
                वापस -E2BIG;
        अगर (iovw->गिनती + source->गिनती > IOV_MAX)
                वापस -E2BIG;

        अगर (accept_zero) {
                अगर (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->गिनती, source->iovec, source->गिनती))
                        वापस -ENOMEM;

                वापस 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        माप_प्रकार original_गिनती = iovw->गिनती;

        FOREACH_ARRAY(iovec, source->iovec, source->गिनती) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                अगर (r < 0)
                        जाओ rollback;
        }

        वापस 0;

rollback:
        iovw->गिनती = original_गिनती;
        वापस r;
}

पूर्णांक iovw_consume_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, व्योम *तथ्य, माप_प्रकार len) {
        /* Move data into iovw or free on error */
        पूर्णांक r;

        r = iovw_put_full(iovw, accept_zero, तथ्य, len);
        अगर (r <= 0)
                मुक्त(तथ्य);

        वापस r;
}

पूर्णांक iovw_consume_iov_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, काष्ठा iovec *iov) {
        पूर्णांक r;

        निश्चित(iovw);

        अगर (!iov)
                वापस 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        अगर (r <= 0)
                iovec_done(iov);
        अन्यथा
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (काष्ठा iovec) {};

        वापस r;
}

पूर्णांक iovw_extend_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर व्योम *तथ्य, माप_प्रकार len) {
        निश्चित(iovw);
        निश्चित(तथ्य || len == 0);

        अगर (len == 0)
                वापस iovw_put_full(iovw, accept_zero, /* data= */ शून्य, /* len= */ 0);

        व्योम *c = memdup(तथ्य, len);
        अगर (!c)
                वापस -ENOMEM;

        वापस iovw_consume_full(iovw, accept_zero, c, len);
}

पूर्णांक iovw_extend_iov_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec *iov) {
        निश्चित(iovw);

        अगर (!iov)
                वापस 0;

        वापस iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

पूर्णांक iovw_extend_iovw_full(काष्ठा iovec_wrapper *iovw, bool accept_zero, स्थिर काष्ठा iovec_wrapper *source) {
        पूर्णांक r;

        निश्चित(iovw);

        /* This duplicates the source and merges it into the iovw. */

        अगर (iovw_isempty(source))
                वापस 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        अगर (iovw == source)
                वापस -EINVAL;

        अगर (iovw->गिनती > SIZE_MAX - source->गिनती)
                वापस -E2BIG;
        अगर (iovw->गिनती + source->गिनती > IOV_MAX)
                वापस -E2BIG;

        माप_प्रकार original_गिनती = iovw->गिनती;

        FOREACH_ARRAY(iovec, source->iovec, source->गिनती) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                अगर (r < 0)
                        जाओ rollback;
        }

        वापस 0;

rollback:
        क्रम (माप_प्रकार i = original_गिनती; i < iovw->गिनती; i++)
                iovec_done(iovw->iovec + i);

        iovw->गिनती = original_गिनती;
        वापस r;
}

पूर्णांक iovw_put_string_field_full(काष्ठा iovec_wrapper *iovw, bool replace, स्थिर अक्षर *field, स्थिर अक्षर *मूल्य) {
        _cleanup_free_ अक्षर *x = शून्य;
        पूर्णांक r;

        निश्चित(iovw);

        x = strjoin(field, मूल्य);
        अगर (!x)
                वापस -ENOMEM;

        अगर (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->गिनती)
                        अगर (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = म_माप(x);
                                वापस free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, म_माप(x));
        अगर (r >= 0)
                TAKE_PTR(x);

        वापस r;
}

पूर्णांक iovw_put_string_fieldf_full(काष्ठा iovec_wrapper *iovw, bool replace, स्थिर अक्षर *field, स्थिर अक्षर *format, ...) {
        _cleanup_free_ अक्षर *मूल्य = शून्य;
        बहु_सूची ap;
        पूर्णांक r;

        निश्चित(format);

        बहु_शुरू(ap, format);
        r = vasprintf(&मूल्य, format, ap);
        बहु_पूर्ण(ap);
        अगर (r < 0)
                वापस -ENOMEM;

        वापस iovw_put_string_field_full(iovw, replace, field, मूल्य);
}

पूर्णांक iovw_put_string_field_free(काष्ठा iovec_wrapper *iovw, स्थिर अक्षर *field, अक्षर *मूल्य) {
        _cleanup_free_ _unused_ अक्षर *free_ptr = मूल्य;

        वापस iovw_put_string_field(iovw, field, मूल्य);
}

व्योम iovw_rebase(काष्ठा iovec_wrapper *iovw, व्योम *old, व्योम *new) {
        निश्चित(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->गिनती) {
                निश्चित(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

माप_प्रकार iovw_आकार(स्थिर काष्ठा iovec_wrapper *iovw) {
        अगर (iovw_isempty(iovw))
                वापस 0;

        वापस iovec_योग_आकार(iovw->iovec, iovw->गिनती);
}

पूर्णांक iovw_concat(स्थिर काष्ठा iovec_wrapper *iovw, काष्ठा iovec *ret) {
        निश्चित(iovw);
        निश्चित(ret);

        /* Squish a series of iovecs into a single iovec. */

        माप_प्रकार len = iovw_आकार(iovw);
        अगर (len == SIZE_MAX)
                वापस -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = दो_स्मृति(len + 1);
        अगर (!buf)
                वापस -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->गिनती)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        वापस 0;
}

अक्षर* iovw_to_cstring(स्थिर काष्ठा iovec_wrapper *iovw) {
        निश्चित(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) काष्ठा iovec iov = {};
        अगर (iovw_concat(iovw, &iov) < 0)
                वापस शून्य;

        निश्चित(!स_प्रथम(iov.iov_base, 0, iov.iov_len));
        वापस TAKE_PTR(iov.iov_base);
}

पूर्णांक iovec_split(स्थिर काष्ठा iovec *iov, माप_प्रकार लंबाई_आकार, काष्ठा iovec_wrapper *ret) {
        पूर्णांक r;

        निश्चित(IN_SET(लंबाई_आकार, 1, 2, 4));
        निश्चित(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        अगर (!iovec_is_set(iov)) {
                *ret = (काष्ठा iovec_wrapper) {};
                वापस 0;
        }

        _cleanup_(iovw_done_free) काष्ठा iovec_wrapper iovw = {};
        क्रम (काष्ठा iovec i = *iov; iovec_is_set(&i); ) {
                अगर (i.iov_len < लंबाई_आकार)
                        वापस -EBADMSG;

                माप_प्रकार len;
                चयन (लंबाई_आकार) {
                हाल 1:
                        len = *(uint8_t*) i.iov_base;
                        अवरोध;
                हाल 2:
                        len = unaligned_read_be16(i.iov_base);
                        अवरोध;
                हाल 4:
                        len = unaligned_read_be32(i.iov_base);
                        अवरोध;
                शेष:
                        assert_not_reached();
                }

                iovec_inc(&i, लंबाई_आकार);

                अगर (len == 0)
                        जारी;

                अगर (i.iov_len < len)
                        वापस -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                अगर (r < 0)
                        वापस r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        वापस 0;
}

पूर्णांक iovw_merge(स्थिर काष्ठा iovec_wrapper *iovw, माप_प्रकार लंबाई_आकार, काष्ठा iovec *ret) {
        निश्चित(IN_SET(लंबाई_आकार, 1, 2, 4));
        निश्चित(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        माप_प्रकार sz = iovw_आकार(iovw);
        अगर (sz == 0) {
                *ret = (काष्ठा iovec) {};
                वापस 0;
        }
        अगर (sz == SIZE_MAX)
                वापस -E2BIG;

        अगर (आकार_multiply_overflow(लंबाई_आकार, iovw->गिनती))
                वापस -E2BIG;

        sz = आकार_add(sz, iovw->गिनती * लंबाई_आकार);
        अगर (sz == SIZE_MAX)
                वापस -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        अगर (!buf)
                वापस -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->गिनती) {
                अगर (iov->iov_len == 0)
                        जारी;

                चयन (लंबाई_आकार) {
                हाल 1:
                        अगर (iov->iov_len > UINT8_MAX)
                                वापस -दुस्फल;

                        *p = iov->iov_len;
                        अवरोध;
                हाल 2:
                        अगर (iov->iov_len > UINT16_MAX)
                                वापस -दुस्फल;

                        unaligned_write_be16(p, iov->iov_len);
                        अवरोध;
                हाल 4:
                        अगर (iov->iov_len > UINT32_MAX)
                                वापस -दुस्फल;

                        unaligned_write_be32(p, iov->iov_len);
                        अवरोध;
                शेष:
                        assert_not_reached();
                }
                p += लंबाई_आकार;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        निश्चित(sz >= (माप_प्रकार) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        वापस 0;
}
