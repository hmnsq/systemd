/* SPDX-License-Identifier: LGPL-2.1-or-later */

/*
 * Concatenates/copies strings. In any case, terminates in all cases
 * with '\0' and moves the @dest pointer forward to the added '\0'.
 * Returns the remaining size, and 0 if the string was truncated.
 *
 * Due to the intended usage, these helpers silently noop invocations
 * having zero size.  This is technically an exception to the above
 * statement "terminates in all cases".  It's unexpected for such calls to
 * occur outside of a loop where this is the preferred behavior.
 */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "strxcpyx.h"

size_t strnpcpy_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        ಆದರೆ (ಗಾತ್ರ_2 == 0) {
                ಆದರೆ (ret_truncated)
                        *ret_truncated = len > 0;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (len >= ಗಾತ್ರ_2) {
                ಆದರೆ (ಗಾತ್ರ_2 > 1)
                        *dest = mempcpy(*dest, src, ಗಾತ್ರ_2-1);
                ಗಾತ್ರ_2 = 0;
                truncated = true;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > 0) {
                *dest = mempcpy(*dest, src, len);
                ಗಾತ್ರ_2 -= len;
        }

        ಆದರೆ (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

size_t strpcpy_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, bool *ret_truncated) {
        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        ಹಿಂತಿರುಗಿ strnpcpy_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, src, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(src), ret_truncated);
}

size_t strpcpyf_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, bool *ret_truncated, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ...) {
        bool truncated = false;
        va_list va;
        ಪೂರ್ಣಾಂಕ i;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        va_start(va, src);
        i = vsnprintf(*dest, ಗಾತ್ರ_2, src, va);
        va_end(va);

        ಪ್ರತಿಜ್ಞಾ(i >= 0);

        ಆದರೆ (i < (ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ_2) {
                *dest += i;
                ಗಾತ್ರ_2 -= i;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಾತ್ರ_2 = 0;
                truncated = i > 0;
        }

        ಆದರೆ (ret_truncated)
                *ret_truncated = truncated;

        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

size_t strpcpyl_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, bool *ret_truncated, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ...) {
        bool truncated = false;
        va_list va;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        va_start(va, src);
        ಮಾಡು {
                bool t;

                ಗಾತ್ರ_2 = strpcpy_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ಅಕ್ಷರ *);
        } ಆಗ (src);
        va_end(va);

        ಆದರೆ (ret_truncated)
                *ret_truncated = truncated;
        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

size_t strnscpy_ತುಂಬಿದ(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t len, bool *ret_truncated) {
        ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        s = dest;
        ಹಿಂತಿರುಗಿ strnpcpy_ತುಂಬಿದ(&s, ಗಾತ್ರ_2, src, len, ret_truncated);
}

size_t strscpy_ತುಂಬಿದ(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, bool *ret_truncated) {
        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        ಹಿಂತಿರುಗಿ strnscpy_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, src, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(src), ret_truncated);
}

size_t strscpyl_ತುಂಬಿದ(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, bool *ret_truncated, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ...) {
        bool truncated = false;
        va_list va;
        ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(dest);
        ಪ್ರತಿಜ್ಞಾ(src);

        va_start(va, src);
        s = dest;
        ಮಾಡು {
                bool t;

                ಗಾತ್ರ_2 = strpcpy_ತುಂಬಿದ(&s, ಗಾತ್ರ_2, src, &t);
                truncated = truncated || t;
                src = va_arg(va, ಅಕ್ಷರ *);
        } ಆಗ (src);
        va_end(va);

        ಆದರೆ (ret_truncated)
                *ret_truncated = truncated;

        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}
