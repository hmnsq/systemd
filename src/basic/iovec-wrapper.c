/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "iovec-util.h"
#ضمن "iovec-wrapper.h"
#ضمن "string-util.h"
#ضمن "unaligned.h"

فراغ iovw_پورا(بنية iovec_wrapper *iovw) {
        أكد(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->گنتی = 0;
}

فراغ iovw_پورا_free(بنية iovec_wrapper *iovw) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->گنتی)
                iovec_پورا(i);

        iovw_پورا(iovw);
}

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw) {
        إذا (!iovw)
                أرجع عدم;

        iovw_پورا(iovw);
        أرجع mfree(iovw);
}

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw) {
        إذا (!iovw)
                أرجع عدم;

        iovw_پورا_free(iovw);
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

        لكل (size_t i = 0, n = MIN(a->گنتی, b->گنتی); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                إذا (r != 0)
                        أرجع r;
        }

        أرجع CMP(a->گنتی, b->گنتی);
}

صحيح iovw_put_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *انکڑے, size_t len) {
        أكد(iovw);
        أكد(انکڑے || len == 0);

        إذا (len == 0 && !accept_zero)
                أرجع 0;

        إذا (iovw->گنتی >= IOV_MAX)
                أرجع -E2BIG;

        إذا (!GREEDY_REALLOC(iovw->iovec, iovw->گنتی + 1))
                أرجع -ENOMEM;

        iovw->iovec[iovw->گنتی++] = IOVEC_MAKE(انکڑے, len);
        أرجع 1;
}

صحيح iovw_put_iov_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        إذا (!iov)
                أرجع 0;

        أرجع iovw_put_بھریا(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_put_iovw_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *سروت) {
        صحيح r;

        أكد(iovw);

        إذا (iovw_isempty(سروت))
                أرجع 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        إذا (iovw == سروت)
                أرجع -EINVAL;

        إذا (iovw->گنتی > SIZE_MAX - سروت->گنتی)
                أرجع -E2BIG;
        إذا (iovw->گنتی + سروت->گنتی > IOV_MAX)
                أرجع -E2BIG;

        إذا (accept_zero) {
                إذا (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->گنتی, سروت->iovec, سروت->گنتی))
                        أرجع -ENOMEM;

                أرجع 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_گنتی = iovw->گنتی;

        FOREACH_ARRAY(iovec, سروت->iovec, سروت->گنتی) {
                r = iovw_put_iov_بھریا(iovw, accept_zero, iovec);
                إذا (r < 0)
                        اذهب rollback;
        }

        أرجع 0;

rollback:
        iovw->گنتی = original_گنتی;
        أرجع r;
}

صحيح iovw_consume_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *انکڑے, size_t len) {
        /* Move data into iovw or free on error */
        صحيح r;

        r = iovw_put_بھریا(iovw, accept_zero, انکڑے, len);
        إذا (r <= 0)
                حرر(انکڑے);

        أرجع r;
}

صحيح iovw_consume_iov_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov) {
        صحيح r;

        أكد(iovw);

        إذا (!iov)
                أرجع 0;

        r = iovw_put_iov_بھریا(iovw, accept_zero, iov);
        إذا (r <= 0)
                iovec_پورا(iov);
        وإلا
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (بنية iovec) {};

        أرجع r;
}

صحيح iovw_extend_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *انکڑے, size_t len) {
        أكد(iovw);
        أكد(انکڑے || len == 0);

        إذا (len == 0)
                أرجع iovw_put_بھریا(iovw, accept_zero, /* data= */ عدم, /* len= */ 0);

        فراغ *c = memdup(انکڑے, len);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع iovw_consume_بھریا(iovw, accept_zero, c, len);
}

صحيح iovw_extend_iov_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov) {
        أكد(iovw);

        إذا (!iov)
                أرجع 0;

        أرجع iovw_extend_بھریا(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

صحيح iovw_extend_iovw_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *سروت) {
        صحيح r;

        أكد(iovw);

        /* This duplicates the source and merges it into the iovw. */

        إذا (iovw_isempty(سروت))
                أرجع 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        إذا (iovw == سروت)
                أرجع -EINVAL;

        إذا (iovw->گنتی > SIZE_MAX - سروت->گنتی)
                أرجع -E2BIG;
        إذا (iovw->گنتی + سروت->گنتی > IOV_MAX)
                أرجع -E2BIG;

        size_t original_گنتی = iovw->گنتی;

        FOREACH_ARRAY(iovec, سروت->iovec, سروت->گنتی) {
                r = iovw_extend_iov_بھریا(iovw, accept_zero, iovec);
                إذا (r < 0)
                        اذهب rollback;
        }

        أرجع 0;

rollback:
        لكل (size_t i = original_گنتی; i < iovw->گنتی; i++)
                iovec_پورا(iovw->iovec + i);

        iovw->گنتی = original_گنتی;
        أرجع r;
}

صحيح iovw_put_string_میدان_بھریا(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *قدر) {
        _cleanup_free_ حرف *x = عدم;
        صحيح r;

        أكد(iovw);

        x = strjoin(میدان, قدر);
        إذا (!x)
                أرجع -ENOMEM;

        إذا (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->گنتی)
                        إذا (حافظہ_startswith(iovec->iov_base, iovec->iov_len, میدان)) {
                                iovec->iov_len = لڑی_لمبائی(x);
                                أرجع free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, لڑی_لمبائی(x));
        إذا (r >= 0)
                TAKE_PTR(x);

        أرجع r;
}

صحيح iovw_put_string_fieldf_بھریا(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *قدر = عدم;
        va_list ap;
        صحيح r;

        أكد(format);

        va_start(ap, format);
        r = vasprintf(&قدر, format, ap);
        va_end(ap);
        إذا (r < 0)
                أرجع -ENOMEM;

        أرجع iovw_put_string_میدان_بھریا(iovw, replace, میدان, قدر);
}

صحيح iovw_put_string_میدان_free(بنية iovec_wrapper *iovw, ثابت حرف *میدان, حرف *قدر) {
        _cleanup_free_ بن_ورتیا حرف *free_ptr = قدر;

        أرجع iovw_put_string_میدان(iovw, میدان, قدر);
}

فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new) {
        أكد(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->گنتی) {
                أكد(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_ناپ(ثابت بنية iovec_wrapper *iovw) {
        إذا (iovw_isempty(iovw))
                أرجع 0;

        أرجع iovec_کل_ناپ(iovw->iovec, iovw->گنتی);
}

صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret) {
        أكد(iovw);
        أكد(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_ناپ(iovw);
        إذا (len == SIZE_MAX)
                أرجع -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = حافظہ_دیو(len + 1);
        إذا (!buf)
                أرجع -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->گنتی)
                p = mempcpy_محفوظ(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        أرجع 0;
}

حرف* iovw_منزل_cstring(ثابت بنية iovec_wrapper *iovw) {
        أكد(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_پورا) بنية iovec iov = {};
        إذا (iovw_concat(iovw, &iov) < 0)
                أرجع عدم;

        أكد(!memchr(iov.iov_base, 0, iov.iov_len));
        أرجع TAKE_PTR(iov.iov_base);
}

صحيح iovec_ونڈو(ثابت بنية iovec *iov, size_t لمائی_ناپ, بنية iovec_wrapper *ret) {
        صحيح r;

        أكد(وچ_سیٹ(لمائی_ناپ, 1, 2, 4));
        أكد(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        إذا (!iovec_is_سیٹ(iov)) {
                *ret = (بنية iovec_wrapper) {};
                أرجع 0;
        }

        _cleanup_(iovw_پورا_free) بنية iovec_wrapper iovw = {};
        لكل (بنية iovec i = *iov; iovec_is_سیٹ(&i); ) {
                إذا (i.iov_len < لمائی_ناپ)
                        أرجع -EBADMSG;

                size_t len;
                بدل (لمائی_ناپ) {
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

                iovec_inc(&i, لمائی_ناپ);

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

صحيح iovw_ملاؤ(ثابت بنية iovec_wrapper *iovw, size_t لمائی_ناپ, بنية iovec *ret) {
        أكد(وچ_سیٹ(لمائی_ناپ, 1, 2, 4));
        أكد(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_ناپ(iovw);
        إذا (sz == 0) {
                *ret = (بنية iovec) {};
                أرجع 0;
        }
        إذا (sz == SIZE_MAX)
                أرجع -E2BIG;

        إذا (ناپ_multiply_حد_توں_ودھ(لمائی_ناپ, iovw->گنتی))
                أرجع -E2BIG;

        sz = ناپ_جوڑو(sz, iovw->گنتی * لمائی_ناپ);
        إذا (sz == SIZE_MAX)
                أرجع -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        إذا (!buf)
                أرجع -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->گنتی) {
                إذا (iov->iov_len == 0)
                        استمر;

                بدل (لمائی_ناپ) {
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
                p += لمائی_ناپ;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        أكد(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        أرجع 0;
}
