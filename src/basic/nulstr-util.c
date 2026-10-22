/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "nulstr-util.h"
#ಅನ್ತರ್ಭಾವ "set.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* nulstr_get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nulstr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಆದರೆ (!nulstr)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        NULSTR_FOREACH(i, nulstr)
                ಆದರೆ (streq(i, needle))
                        ಹಿಂತಿರುಗಿ i;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ** strv_parse_nulstr_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ ಅಕ್ಷರ **v = ಖಾಲಿ;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        ಪ್ರತಿಜ್ಞಾ(s || l <= 0);

        ಆದರೆ (drop_trailing_nuls)
                ಆಗ (l > 0 && s[l-1] == '\0')
                        l--;

        ಆದರೆ (l <= 0)
                ಹಿಂತಿರುಗಿ new0(ಅಕ್ಷರ*, 1);

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = s; p < s + l; p++)
                ಆದರೆ (*p == 0)
                        c++;

        ಆದರೆ (s[l-1] != 0)
                c++;

        v = new0(ಅಕ್ಷರ*, c+1);
        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = s; p < s + l;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                ಆದರೆ (!v[i])
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                i++;

                ಆದರೆ (!e)
                        ನಿಲ್ಲಿಸು;

                p = e + 1;
        }

        ಪ್ರತಿಜ್ಞಾ(i == c);

        ಹಿಂತಿರುಗಿ TAKE_PTR(v);
}

ಅಕ್ಷರ** strv_ವಿಭಜಿಸು_nulstr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                ಆದರೆ (strv_extend(&l, i) < 0)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ l ? TAKE_PTR(l) : strv_new(ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ strv_make_nulstr(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ) {
        _cleanup_free_ ಅಕ್ಷರ *m = ಖಾಲಿ;
        size_t n = 0;

        /* Builds a nulstr and returns it together with the size. An extra NUL byte will be appended (⚠️ but
         * not included in the size! ⚠️). This is done so that the nulstr can be used both in
         * strv_parse_nulstr() and in NULSTR_FOREACH()/strv_split_nulstr() contexts, i.e. with and without a
         * size parameter. In the former case we can include empty strings, in the latter case we cannot (as
         * that is the end marker).
         *
         * When NULSTR_FOREACH()/strv_split_nulstr() is used it is often assumed that the nulstr ends in two
         * NUL bytes (which it will, if not empty). To ensure that this assumption *always* holds, we'll
         * return a buffer with two NUL bytes in that case, but return a size of zero. */

        ಪ್ರತಿಜ್ಞಾ(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*i) + 1;

                ಆದರೆ (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಸ್ಮರಣೆ_ನಕಲು(m + n, *i, z);
                n += z;
        }

        ಆದರೆ (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(ಅಕ್ಷರ, 2);
                ಆದರೆ (!m)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                n = 0;
        } ಇಲ್ಲದಿದ್ದರೆ
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = n;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಗಣ_make_nulstr(ಗಣ_2 *s, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ ಅಕ್ಷರ **strv = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        strv = ಗಣ_get_strv(s);
        ಆದರೆ (!strv)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_make_nulstr(strv, ret, ret_ಗಾತ್ರ);
}
