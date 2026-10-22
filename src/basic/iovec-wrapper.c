/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "iovec-util.h"
#ضمن "iovec-wrapper.h"
#ضمن "string-util.h"
#ضمن "unaligned.h"

فراغ iovw_پورو(بنية iovec_wrapper *iovw) {
        أكد(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->ڳڻپ = 0;
}

فراغ iovw_پورو_free(بنية iovec_wrapper *iovw) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ڳڻپ)
                iovec_پورو(i);

        iovw_پورو(iovw);
}

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw) {
        إذا (!iovw)
                أرجع عدم;

        iovw_پورو(iovw);
        أرجع mfree(iovw);
}

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw) {
        إذا (!iovw)
                أرجع عدم;

        iovw_پورو_free(iovw);
        أرجع mfree(iovw);
}

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        صحيح r;

        إذا (a == b)
                أرجع 0;

        إذا (!a || !b)
                أرجع CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        لكل (size_t i = 0, n = MIN(a->ڳڻپ, b->ڳڻپ); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                إذا (r != 0)
                        أرجع r;
        }

        أرجع CMP(a->ڳڻپ, b->ڳڻپ);
}

صحيح iovw_put_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *معلومات, size_t len) {
        أكد(iovw);
        أكد(معلومات || len == 0);

        إذا (len == 0 && !accept_zero)
                أرجع 0;

        إذا (iovw->ڳڻپ >= IOV_MAX)
                أرجع -E2BIG;

        إذا (!GREEDY_REALLOC(iovw->iovec, iovw->ڳڻپ + 1))
                أرجع -ENOMEM;

        iovw->iovec[iovw->ڳڻپ++] = IOVEC_MAKE(معلومات, len);
        أرجع 1;
}

صحيح iovw_put_iov_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        إذا (!iov)
                أرجع 0;

        أرجع iovw_put_ڀريل(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_put_iovw_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *ذريعو) {
        صحيح r;

        أكد(iovw);

        إذا (iovw_isempty(ذريعو))
                أرجع 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        إذا (iovw == ذريعو)
                أرجع -EINVAL;

        إذا (iovw->ڳڻپ > SIZE_MAX - ذريعو->ڳڻپ)
                أرجع -E2BIG;
        إذا (iovw->ڳڻپ + ذريعو->ڳڻپ > IOV_MAX)
                أرجع -E2BIG;

        إذا (accept_zero) {
                إذا (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->ڳڻپ, ذريعو->iovec, ذريعو->ڳڻپ))
                        أرجع -ENOMEM;

                أرجع 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_ڳڻپ = iovw->ڳڻپ;

        FOREACH_ARRAY(iovec, ذريعو->iovec, ذريعو->ڳڻپ) {
                r = iovw_put_iov_ڀريل(iovw, accept_zero, iovec);
                إذا (r < 0)
                        اذهب rollback;
        }

        أرجع 0;

rollback:
        iovw->ڳڻپ = original_ڳڻپ;
        أرجع r;
}

صحيح iovw_consume_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *معلومات, size_t len) {
        /* Move data into iovw or free on error */
        صحيح r;

        r = iovw_put_ڀريل(iovw, accept_zero, معلومات, len);
        إذا (r <= 0)
                حرر(معلومات);

        أرجع r;
}

صحيح iovw_consume_iov_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov) {
        صحيح r;

        أكد(iovw);

        إذا (!iov)
                أرجع 0;

        r = iovw_put_iov_ڀريل(iovw, accept_zero, iov);
        إذا (r <= 0)
                iovec_پورو(iov);
        وإلا
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (بنية iovec) {};

        أرجع r;
}

صحيح iovw_extend_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *معلومات, size_t len) {
        أكد(iovw);
        أكد(معلومات || len == 0);

        إذا (len == 0)
                أرجع iovw_put_ڀريل(iovw, accept_zero, /* data= */ عدم, /* len= */ 0);

        فراغ *c = memdup(معلومات, len);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع iovw_consume_ڀريل(iovw, accept_zero, c, len);
}

صحيح iovw_extend_iov_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        إذا (!iov)
                أرجع 0;

        أرجع iovw_extend_ڀريل(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_extend_iovw_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *ذريعو) {
        صحيح r;

        أكد(iovw);

        /* This duplicates the source and merges it into the iovw. */

        إذا (iovw_isempty(ذريعو))
                أرجع 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        إذا (iovw == ذريعو)
                أرجع -EINVAL;

        إذا (iovw->ڳڻپ > SIZE_MAX - ذريعو->ڳڻپ)
                أرجع -E2BIG;
        إذا (iovw->ڳڻپ + ذريعو->ڳڻپ > IOV_MAX)
                أرجع -E2BIG;

        size_t original_ڳڻپ = iovw->ڳڻپ;

        FOREACH_ARRAY(iovec, ذريعو->iovec, ذريعو->ڳڻپ) {
                r = iovw_extend_iov_ڀريل(iovw, accept_zero, iovec);
                إذا (r < 0)
                        اذهب rollback;
        }

        أرجع 0;

rollback:
        لكل (size_t i = original_ڳڻپ; i < iovw->ڳڻپ; i++)
                iovec_پورو(iovw->iovec + i);

        iovw->ڳڻپ = original_ڳڻپ;
        أرجع r;
}

صحيح iovw_put_string_ميدان_ڀريل(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *ميدان, ثابت حرف *قدر) {
        _cleanup_free_ حرف *x = عدم;
        صحيح r;

        أكد(iovw);

        x = strjoin(ميدان, قدر);
        إذا (!x)
                أرجع -ENOMEM;

        إذا (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->ڳڻپ)
                        إذا (يادگيري_startswith(iovec->iov_base, iovec->iov_len, ميدان)) {
                                iovec->iov_len = لڙي_ڊيگهه(x);
                                أرجع free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, لڙي_ڊيگهه(x));
        إذا (r >= 0)
                TAKE_PTR(x);

        أرجع r;
}

صحيح iovw_put_string_fieldf_ڀريل(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *ميدان, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *قدر = عدم;
        va_list ap;
        صحيح r;

        أكد(format);

        va_start(ap, format);
        r = vasprintf(&قدر, format, ap);
        va_end(ap);
        إذا (r < 0)
                أرجع -ENOMEM;

        أرجع iovw_put_string_ميدان_ڀريل(iovw, replace, ميدان, قدر);
}

صحيح iovw_put_string_ميدان_free(بنية iovec_wrapper *iovw, ثابت حرف *ميدان, حرف *قدر) {
        _cleanup_free_ اڻ_استعمال_ٿيل حرف *free_ptr = قدر;

        أرجع iovw_put_string_ميدان(iovw, ميدان, قدر);
}

فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ڳڻپ) {
                أكد(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_سائيز(ثابت بنية iovec_wrapper *iovw) {
        إذا (iovw_isempty(iovw))
                أرجع 0;

        أرجع iovec_ڪل_سائيز(iovw->iovec, iovw->ڳڻپ);
}

صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret) {
        أكد(iovw);
        أكد(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_سائيز(iovw);
        إذا (len == SIZE_MAX)
                أرجع -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = يادگيري_ڏيو(len + 1);
        إذا (!buf)
                أرجع -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->ڳڻپ)
                p = mempcpy_محفوظ(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        أرجع 0;
}

حرف* iovw_منزل_cstring(ثابت بنية iovec_wrapper *iovw) {
        أكد(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_پورو) بنية iovec iov = {};
        إذا (iovw_concat(iovw, &iov) < 0)
                أرجع عدم;

        أكد(!memchr(iov.iov_base, 0, iov.iov_len));
        أرجع TAKE_PTR(iov.iov_base);
}

صحيح iovec_ورهايو(ثابت بنية iovec *iov, size_t ڊيگهائي_سائيز, بنية iovec_wrapper *ret) {
        صحيح r;

        أكد(اندريون_سيٽ(ڊيگهائي_سائيز, 1, 2, 4));
        أكد(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        إذا (!iovec_is_سيٽ(iov)) {
                *ret = (بنية iovec_wrapper) {};
                أرجع 0;
        }

        _cleanup_(iovw_پورو_free) بنية iovec_wrapper iovw = {};
        لكل (بنية iovec i = *iov; iovec_is_سيٽ(&i); ) {
                إذا (i.iov_len < ڊيگهائي_سائيز)
                        أرجع -EBADMSG;

                size_t len;
                بدل (ڊيگهائي_سائيز) {
                حالة 1:
                        len = *(uint8_t*) i.iov_base;
                        اكسر;
                حالة 2:
                        len = unaligned_read_be16(i.iov_base);
                        اكسر;
                حالة 4:
                        len = unaligned_read_be32(i.iov_base);
                        اكسر;
                افتراضي:
                        assert_not_reached();
                }

                iovec_inc(&i, ڊيگهائي_سائيز);

                إذا (len == 0)
                        استمر;

                إذا (i.iov_len < len)
                        أرجع -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                إذا (r < 0)
                        أرجع r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        أرجع 0;
}

صحيح iovw_ملايو(ثابت بنية iovec_wrapper *iovw, size_t ڊيگهائي_سائيز, بنية iovec *ret) {
        أكد(اندريون_سيٽ(ڊيگهائي_سائيز, 1, 2, 4));
        أكد(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_سائيز(iovw);
        إذا (sz == 0) {
                *ret = (بنية iovec) {};
                أرجع 0;
        }
        إذا (sz == SIZE_MAX)
                أرجع -E2BIG;

        إذا (سائيز_multiply_حد_کان_مٿي(ڊيگهائي_سائيز, iovw->ڳڻپ))
                أرجع -E2BIG;

        sz = سائيز_جوڙيو(sz, iovw->ڳڻپ * ڊيگهائي_سائيز);
        إذا (sz == SIZE_MAX)
                أرجع -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        إذا (!buf)
                أرجع -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->ڳڻپ) {
                إذا (iov->iov_len == 0)
                        استمر;

                بدل (ڊيگهائي_سائيز) {
                حالة 1:
                        إذا (iov->iov_len > UINT8_MAX)
                                أرجع -ERANGE;

                        *p = iov->iov_len;
                        اكسر;
                حالة 2:
                        إذا (iov->iov_len > UINT16_MAX)
                                أرجع -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        اكسر;
                حالة 4:
                        إذا (iov->iov_len > UINT32_MAX)
                                أرجع -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        اكسر;
                افتراضي:
                        assert_not_reached();
                }
                p += ڊيگهائي_سائيز;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        أكد(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        أرجع 0;
}
