/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-wrapper.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

ಶೂನ್ಯ iovw_ಮುಗಿದ(ರಚನೆ iovec_wrapper *iovw) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->ಎಣಿಕೆ_2 = 0;
}

ಶೂನ್ಯ iovw_ಮುಗಿದ_free(ರಚನೆ iovec_wrapper *iovw) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ಎಣಿಕೆ_2)
                iovec_ಮುಗಿದ(i);

        iovw_ಮುಗಿದ(iovw);
}

ರಚನೆ iovec_wrapper* iovw_free(ರಚನೆ iovec_wrapper *iovw) {
        ಆದರೆ (!iovw)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        iovw_ಮುಗಿದ(iovw);
        ಹಿಂತಿರುಗಿ mfree(iovw);
}

ರಚನೆ iovec_wrapper* iovw_free_free(ರಚನೆ iovec_wrapper *iovw) {
        ಆದರೆ (!iovw)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        iovw_ಮುಗಿದ_free(iovw);
        ಹಿಂತಿರುಗಿ mfree(iovw);
}

ಪೂರ್ಣಾಂಕ iovw_compare(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *a, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (a == b)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!a || !b)
                ಹಿಂತಿರುಗಿ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        ಗಾಗಿ (size_t i = 0, n = MIN(a->ಎಣಿಕೆ_2, b->ಎಣಿಕೆ_2); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                ಆದರೆ (r != 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ CMP(a->ಎಣಿಕೆ_2, b->ಎಣಿಕೆ_2);
}

ಪೂರ್ಣಾಂಕ iovw_put_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len) {
        ಪ್ರತಿಜ್ಞಾ(iovw);
        ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ || len == 0);

        ಆದರೆ (len == 0 && !accept_zero)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (iovw->ಎಣಿಕೆ_2 >= IOV_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        ಆದರೆ (!GREEDY_REALLOC(iovw->iovec, iovw->ಎಣಿಕೆ_2 + 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        iovw->iovec[iovw->ಎಣಿಕೆ_2++] = IOVEC_MAKE(ದತ್ತಾಂಶ, len);
        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ iovw_put_iov_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಆದರೆ (!iov)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ iovw_put_ತುಂಬಿದ(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ಪೂರ್ಣಾಂಕ iovw_put_iovw_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *ಮೂಲ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಆದರೆ (iovw_isempty(ಮೂಲ))
                ಹಿಂತಿರುಗಿ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        ಆದರೆ (iovw == ಮೂಲ)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (iovw->ಎಣಿಕೆ_2 > SIZE_MAX - ಮೂಲ->ಎಣಿಕೆ_2)
                ಹಿಂತಿರುಗಿ -E2BIG;
        ಆದರೆ (iovw->ಎಣಿಕೆ_2 + ಮೂಲ->ಎಣಿಕೆ_2 > IOV_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        ಆದರೆ (accept_zero) {
                ಆದರೆ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->ಎಣಿಕೆ_2, ಮೂಲ->iovec, ಮೂಲ->ಎಣಿಕೆ_2))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಹಿಂತಿರುಗಿ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_ಎಣಿಕೆ = iovw->ಎಣಿಕೆ_2;

        FOREACH_ARRAY(iovec, ಮೂಲ->iovec, ಮೂಲ->ಎಣಿಕೆ_2) {
                r = iovw_put_iov_ತುಂಬಿದ(iovw, accept_zero, iovec);
                ಆದರೆ (r < 0)
                        ಹೋಗು rollback;
        }

        ಹಿಂತಿರುಗಿ 0;

rollback:
        iovw->ಎಣಿಕೆ_2 = original_ಎಣಿಕೆ;
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ iovw_consume_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len) {
        /* Move data into iovw or free on error */
        ಪೂರ್ಣಾಂಕ r;

        r = iovw_put_ತುಂಬಿದ(iovw, accept_zero, ದತ್ತಾಂಶ, len);
        ಆದರೆ (r <= 0)
                ಬಿಡುಗಡೆ(ದತ್ತಾಂಶ);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ iovw_consume_iov_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ರಚನೆ iovec *iov) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಆದರೆ (!iov)
                ಹಿಂತಿರುಗಿ 0;

        r = iovw_put_iov_ತುಂಬಿದ(iovw, accept_zero, iov);
        ಆದರೆ (r <= 0)
                iovec_ಮುಗಿದ(iov);
        ಇಲ್ಲದಿದ್ದರೆ
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (ರಚನೆ iovec) {};

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ iovw_extend_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len) {
        ಪ್ರತಿಜ್ಞಾ(iovw);
        ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ || len == 0);

        ಆದರೆ (len == 0)
                ಹಿಂತಿರುಗಿ iovw_put_ತುಂಬಿದ(iovw, accept_zero, /* data= */ ಖಾಲಿ, /* len= */ 0);

        ಶೂನ್ಯ *c = memdup(ದತ್ತಾಂಶ, len);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ iovw_consume_ತುಂಬಿದ(iovw, accept_zero, c, len);
}

ಪೂರ್ಣಾಂಕ iovw_extend_iov_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        ಆದರೆ (!iov)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ iovw_extend_ತುಂಬಿದ(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

ಪೂರ್ಣಾಂಕ iovw_extend_iovw_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *ಮೂಲ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        /* This duplicates the source and merges it into the iovw. */

        ಆದರೆ (iovw_isempty(ಮೂಲ))
                ಹಿಂತಿರುಗಿ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        ಆದರೆ (iovw == ಮೂಲ)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (iovw->ಎಣಿಕೆ_2 > SIZE_MAX - ಮೂಲ->ಎಣಿಕೆ_2)
                ಹಿಂತಿರುಗಿ -E2BIG;
        ಆದರೆ (iovw->ಎಣಿಕೆ_2 + ಮೂಲ->ಎಣಿಕೆ_2 > IOV_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        size_t original_ಎಣಿಕೆ = iovw->ಎಣಿಕೆ_2;

        FOREACH_ARRAY(iovec, ಮೂಲ->iovec, ಮೂಲ->ಎಣಿಕೆ_2) {
                r = iovw_extend_iov_ತುಂಬಿದ(iovw, accept_zero, iovec);
                ಆದರೆ (r < 0)
                        ಹೋಗು rollback;
        }

        ಹಿಂತಿರುಗಿ 0;

rollback:
        ಗಾಗಿ (size_t i = original_ಎಣಿಕೆ; i < iovw->ಎಣಿಕೆ_2; i++)
                iovec_ಮುಗಿದ(iovw->iovec + i);

        iovw->ಎಣಿಕೆ_2 = original_ಎಣಿಕೆ;
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ iovw_put_string_ಕ್ಷೇತ್ರ_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool replace, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *x = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(iovw);

        x = strjoin(ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
        ಆದರೆ (!x)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->ಎಣಿಕೆ_2)
                        ಆದರೆ (ಸ್ಮರಣೆ_startswith(iovec->iov_base, iovec->iov_len, ಕ್ಷೇತ್ರ)) {
                                iovec->iov_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(x);
                                ಹಿಂತಿರುಗಿ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(x));
        ಆದರೆ (r >= 0)
                TAKE_PTR(x);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ iovw_put_string_fieldf_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool replace, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(format);

        va_start(ap, format);
        r = vasprintf(&ಮೌಲ್ಯ, format, ap);
        va_end(ap);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ iovw_put_string_ಕ್ಷೇತ್ರ_ತುಂಬಿದ(iovw, replace, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ iovw_put_string_ಕ್ಷೇತ್ರ_free(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        _cleanup_free_ ಬಳಸದ ಅಕ್ಷರ *free_ptr = ಮೌಲ್ಯ;

        ಹಿಂತಿರುಗಿ iovw_put_string_ಕ್ಷೇತ್ರ(iovw, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
}

ಶೂನ್ಯ iovw_rebase(ರಚನೆ iovec_wrapper *iovw, ಶೂನ್ಯ *old, ಶೂನ್ಯ *new) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->ಎಣಿಕೆ_2) {
                ಪ್ರತಿಜ್ಞಾ(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw) {
        ಆದರೆ (iovw_isempty(iovw))
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ iovec_ಒಟ್ಟು_ಗಾತ್ರ(iovw->iovec, iovw->ಎಣಿಕೆ_2);
}

ಪೂರ್ಣಾಂಕ iovw_concat(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw, ರಚನೆ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(iovw);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_ಗಾತ್ರ(iovw);
        ಆದರೆ (len == SIZE_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = ಸ್ಮರಣೆ_ಕೊಡು(len + 1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->ಎಣಿಕೆ_2)
                p = mempcpy_ಸುರಕ್ಷಿತ(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* iovw_ಗಮ್ಯ_cstring(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw) {
        ಪ್ರತಿಜ್ಞಾ(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_ಮುಗಿದ) ರಚನೆ iovec iov = {};
        ಆದರೆ (iovw_concat(iovw, &iov) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(!memchr(iov.iov_base, 0, iov.iov_len));
        ಹಿಂತಿರುಗಿ TAKE_PTR(iov.iov_base);
}

ಪೂರ್ಣಾಂಕ iovec_ವಿಭಜಿಸು(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov, size_t ಉದ್ದಳತೆ_ಗಾತ್ರ, ರಚನೆ iovec_wrapper *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(ಉದ್ದಳತೆ_ಗಾತ್ರ, 1, 2, 4));
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        ಆದರೆ (!iovec_is_ಗಣ(iov)) {
                *ret = (ರಚನೆ iovec_wrapper) {};
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_(iovw_ಮುಗಿದ_free) ರಚನೆ iovec_wrapper iovw = {};
        ಗಾಗಿ (ರಚನೆ iovec i = *iov; iovec_is_ಗಣ(&i); ) {
                ಆದರೆ (i.iov_len < ಉದ್ದಳತೆ_ಗಾತ್ರ)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                size_t len;
                ಆಯ್ಕೆ (ಉದ್ದಳತೆ_ಗಾತ್ರ) {
                ಸ್ಥಿತಿ 1:
                        len = *(uint8_t*) i.iov_base;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ 2:
                        len = unaligned_read_be16(i.iov_base);
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ 4:
                        len = unaligned_read_be32(i.iov_base);
                        ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                        assert_not_reached();
                }

                iovec_inc(&i, ಉದ್ದಳತೆ_ಗಾತ್ರ);

                ಆದರೆ (len == 0)
                        ಮುಂದುವರಿ;

                ಆದರೆ (i.iov_len < len)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ iovw_ವಿಲೀನ(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw, size_t ಉದ್ದಳತೆ_ಗಾತ್ರ, ರಚನೆ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(ಉದ್ದಳತೆ_ಗಾತ್ರ, 1, 2, 4));
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_ಗಾತ್ರ(iovw);
        ಆದರೆ (sz == 0) {
                *ret = (ರಚನೆ iovec) {};
                ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (sz == SIZE_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        ಆದರೆ (ಗಾತ್ರ_multiply_ಅತಿಪ್ರವಾಹ(ಉದ್ದಳತೆ_ಗಾತ್ರ, iovw->ಎಣಿಕೆ_2))
                ಹಿಂತಿರುಗಿ -E2BIG;

        sz = ಗಾತ್ರ_ಕೂಡಿಸು(sz, iovw->ಎಣಿಕೆ_2 * ಉದ್ದಳತೆ_ಗಾತ್ರ);
        ಆದರೆ (sz == SIZE_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->ಎಣಿಕೆ_2) {
                ಆದರೆ (iov->iov_len == 0)
                        ಮುಂದುವರಿ;

                ಆಯ್ಕೆ (ಉದ್ದಳತೆ_ಗಾತ್ರ) {
                ಸ್ಥಿತಿ 1:
                        ಆದರೆ (iov->iov_len > UINT8_MAX)
                                ಹಿಂತಿರುಗಿ -ERANGE;

                        *p = iov->iov_len;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ 2:
                        ಆದರೆ (iov->iov_len > UINT16_MAX)
                                ಹಿಂತಿರುಗಿ -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ 4:
                        ಆದರೆ (iov->iov_len > UINT32_MAX)
                                ಹಿಂತಿರುಗಿ -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                        assert_not_reached();
                }
                p += ಉದ್ದಳತೆ_ಗಾತ್ರ;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        ಪ್ರತಿಜ್ಞಾ(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        ಹಿಂತಿರುಗಿ 0;
}
