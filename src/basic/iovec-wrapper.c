/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "iovec-util.h"
#شامل "iovec-wrapper.h"
#شامل "string-util.h"
#شامل "unaligned.h"

خالی iovw_مکمل(ساخت iovec_wrapper *iovw) {
        أكد(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->گنتی = 0;
}

خالی iovw_مکمل_free(ساخت iovec_wrapper *iovw) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->گنتی)
                iovec_مکمل(i);

        iovw_مکمل(iovw);
}

ساخت iovec_wrapper* iovw_free(ساخت iovec_wrapper *iovw) {
        اگر (!iovw)
                واپس تہی;

        iovw_مکمل(iovw);
        واپس mfree(iovw);
}

ساخت iovec_wrapper* iovw_free_free(ساخت iovec_wrapper *iovw) {
        اگر (!iovw)
                واپس تہی;

        iovw_مکمل_free(iovw);
        واپس mfree(iovw);
}

صحیح iovw_compare(ثابت ساخت iovec_wrapper *a, ثابت ساخت iovec_wrapper *b) {
        صحیح r;

        اگر (a == b)
                واپس 0;

        اگر (!a || !b)
                واپس CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        برائے (size_t i = 0, n = MIN(a->گنتی, b->گنتی); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                اگر (r != 0)
                        واپس r;
        }

        واپس CMP(a->گنتی, b->گنتی);
}

صحیح iovw_put_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, خالی *ڈیٹا, size_t len) {
        أكد(iovw);
        أكد(ڈیٹا || len == 0);

        اگر (len == 0 && !accept_zero)
                واپس 0;

        اگر (iovw->گنتی >= IOV_MAX)
                واپس -E2BIG;

        اگر (!GREEDY_REALLOC(iovw->iovec, iovw->گنتی + 1))
                واپس -ENOMEM;

        iovw->iovec[iovw->گنتی++] = IOVEC_MAKE(ڈیٹا, len);
        واپس 1;
}

صحیح iovw_put_iov_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec *iov) {
        أكد(iovw);

        اگر (!iov)
                واپس 0;

        واپس iovw_put_بھرا(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحیح iovw_put_iovw_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec_wrapper *ماخذ) {
        صحیح r;

        أكد(iovw);

        اگر (iovw_isempty(ماخذ))
                واپس 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        اگر (iovw == ماخذ)
                واپس -EINVAL;

        اگر (iovw->گنتی > SIZE_MAX - ماخذ->گنتی)
                واپس -E2BIG;
        اگر (iovw->گنتی + ماخذ->گنتی > IOV_MAX)
                واپس -E2BIG;

        اگر (accept_zero) {
                اگر (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->گنتی, ماخذ->iovec, ماخذ->گنتی))
                        واپس -ENOMEM;

                واپس 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_گنتی = iovw->گنتی;

        FOREACH_ARRAY(iovec, ماخذ->iovec, ماخذ->گنتی) {
                r = iovw_put_iov_بھرا(iovw, accept_zero, iovec);
                اگر (r < 0)
                        جاؤ rollback;
        }

        واپس 0;

rollback:
        iovw->گنتی = original_گنتی;
        واپس r;
}

صحیح iovw_consume_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, خالی *ڈیٹا, size_t len) {
        /* Move data into iovw or free on error */
        صحیح r;

        r = iovw_put_بھرا(iovw, accept_zero, ڈیٹا, len);
        اگر (r <= 0)
                آزاد(ڈیٹا);

        واپس r;
}

صحیح iovw_consume_iov_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ساخت iovec *iov) {
        صحیح r;

        أكد(iovw);

        اگر (!iov)
                واپس 0;

        r = iovw_put_iov_بھرا(iovw, accept_zero, iov);
        اگر (r <= 0)
                iovec_مکمل(iov);
        ورنہ
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (ساخت iovec) {};

        واپس r;
}

صحیح iovw_extend_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت خالی *ڈیٹا, size_t len) {
        أكد(iovw);
        أكد(ڈیٹا || len == 0);

        اگر (len == 0)
                واپس iovw_put_بھرا(iovw, accept_zero, /* data= */ تہی, /* len= */ 0);

        خالی *c = memdup(ڈیٹا, len);
        اگر (!c)
                واپس -ENOMEM;

        واپس iovw_consume_بھرا(iovw, accept_zero, c, len);
}

صحیح iovw_extend_iov_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec *iov) {
        أكد(iovw);

        اگر (!iov)
                واپس 0;

        واپس iovw_extend_بھرا(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحیح iovw_extend_iovw_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec_wrapper *ماخذ) {
        صحیح r;

        أكد(iovw);

        /* This duplicates the source and merges it into the iovw. */

        اگر (iovw_isempty(ماخذ))
                واپس 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        اگر (iovw == ماخذ)
                واپس -EINVAL;

        اگر (iovw->گنتی > SIZE_MAX - ماخذ->گنتی)
                واپس -E2BIG;
        اگر (iovw->گنتی + ماخذ->گنتی > IOV_MAX)
                واپس -E2BIG;

        size_t original_گنتی = iovw->گنتی;

        FOREACH_ARRAY(iovec, ماخذ->iovec, ماخذ->گنتی) {
                r = iovw_extend_iov_بھرا(iovw, accept_zero, iovec);
                اگر (r < 0)
                        جاؤ rollback;
        }

        واپس 0;

rollback:
        برائے (size_t i = original_گنتی; i < iovw->گنتی; i++)
                iovec_مکمل(iovw->iovec + i);

        iovw->گنتی = original_گنتی;
        واپس r;
}

صحیح iovw_put_string_میدان_بھرا(ساخت iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *قدر) {
        _cleanup_free_ حرف *x = تہی;
        صحیح r;

        أكد(iovw);

        x = strjoin(میدان, قدر);
        اگر (!x)
                واپس -ENOMEM;

        اگر (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->گنتی)
                        اگر (حافظہ_startswith(iovec->iov_base, iovec->iov_len, میدان)) {
                                iovec->iov_len = لڑی_لمبائی(x);
                                واپس free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, لڑی_لمبائی(x));
        اگر (r >= 0)
                TAKE_PTR(x);

        واپس r;
}

صحیح iovw_put_string_fieldf_بھرا(ساخت iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *قدر = تہی;
        va_list ap;
        صحیح r;

        أكد(format);

        va_start(ap, format);
        r = vasprintf(&قدر, format, ap);
        va_end(ap);
        اگر (r < 0)
                واپس -ENOMEM;

        واپس iovw_put_string_میدان_بھرا(iovw, replace, میدان, قدر);
}

صحیح iovw_put_string_میدان_free(ساخت iovec_wrapper *iovw, ثابت حرف *میدان, حرف *قدر) {
        _cleanup_free_ غیر_مستعمل حرف *free_ptr = قدر;

        واپس iovw_put_string_میدان(iovw, میدان, قدر);
}

خالی iovw_rebase(ساخت iovec_wrapper *iovw, خالی *old, خالی *new) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->گنتی) {
                أكد(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_سائز(ثابت ساخت iovec_wrapper *iovw) {
        اگر (iovw_isempty(iovw))
                واپس 0;

        واپس iovec_کل_سائز(iovw->iovec, iovw->گنتی);
}

صحیح iovw_concat(ثابت ساخت iovec_wrapper *iovw, ساخت iovec *ret) {
        أكد(iovw);
        أكد(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_سائز(iovw);
        اگر (len == SIZE_MAX)
                واپس -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = حافظہ_دو(len + 1);
        اگر (!buf)
                واپس -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->گنتی)
                p = mempcpy_محفوظ(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        واپس 0;
}

حرف* iovw_مقصد_cstring(ثابت ساخت iovec_wrapper *iovw) {
        أكد(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_مکمل) ساخت iovec iov = {};
        اگر (iovw_concat(iovw, &iov) < 0)
                واپس تہی;

        أكد(!memchr(iov.iov_base, 0, iov.iov_len));
        واپس TAKE_PTR(iov.iov_base);
}

صحیح iovec_بانٹو(ثابت ساخت iovec *iov, size_t لمبائی_سائز, ساخت iovec_wrapper *ret) {
        صحیح r;

        أكد(اندر_سیٹ(لمبائی_سائز, 1, 2, 4));
        أكد(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        اگر (!iovec_is_سیٹ(iov)) {
                *ret = (ساخت iovec_wrapper) {};
                واپس 0;
        }

        _cleanup_(iovw_مکمل_free) ساخت iovec_wrapper iovw = {};
        برائے (ساخت iovec i = *iov; iovec_is_سیٹ(&i); ) {
                اگر (i.iov_len < لمبائی_سائز)
                        واپس -EBADMSG;

                size_t len;
                انتخاب (لمبائی_سائز) {
                حالت 1:
                        len = *(uint8_t*) i.iov_base;
                        توڑو;
                حالت 2:
                        len = unaligned_read_be16(i.iov_base);
                        توڑو;
                حالت 4:
                        len = unaligned_read_be32(i.iov_base);
                        توڑو;
                طےشدہ:
                        assert_not_reached();
                }

                iovec_inc(&i, لمبائی_سائز);

                اگر (len == 0)
                        جاری;

                اگر (i.iov_len < len)
                        واپس -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                اگر (r < 0)
                        واپس r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        واپس 0;
}

صحیح iovw_ملاؤ(ثابت ساخت iovec_wrapper *iovw, size_t لمبائی_سائز, ساخت iovec *ret) {
        أكد(اندر_سیٹ(لمبائی_سائز, 1, 2, 4));
        أكد(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_سائز(iovw);
        اگر (sz == 0) {
                *ret = (ساخت iovec) {};
                واپس 0;
        }
        اگر (sz == SIZE_MAX)
                واپس -E2BIG;

        اگر (سائز_multiply_حد_سے_تجاوز(لمبائی_سائز, iovw->گنتی))
                واپس -E2BIG;

        sz = سائز_جوڑو(sz, iovw->گنتی * لمبائی_سائز);
        اگر (sz == SIZE_MAX)
                واپس -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        اگر (!buf)
                واپس -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->گنتی) {
                اگر (iov->iov_len == 0)
                        جاری;

                انتخاب (لمبائی_سائز) {
                حالت 1:
                        اگر (iov->iov_len > UINT8_MAX)
                                واپس -ERANGE;

                        *p = iov->iov_len;
                        توڑو;
                حالت 2:
                        اگر (iov->iov_len > UINT16_MAX)
                                واپس -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        توڑو;
                حالت 4:
                        اگر (iov->iov_len > UINT32_MAX)
                                واپس -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        توڑو;
                طےشدہ:
                        assert_not_reached();
                }
                p += لمبائی_سائز;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        أكد(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        واپس 0;
}
