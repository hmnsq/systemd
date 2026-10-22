/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "nulstr-util.h"
#ಅನ್ತರ್ಭಾವ "set.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಧ್ರುವ ವರ್ಣ* nulstr_get(ಧ್ರುವ ವರ್ಣ *nulstr, ಧ್ರುವ ವರ್ಣ *needle) {
        ಯದಿ (!nulstr)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        NULSTR_FOREACH(i, nulstr)
                ಯದಿ (streq(i, needle))
                        ಪ್ರತ್ಯಾಗಮ i;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ** strv_parse_nulstr_full(ಧ್ರುವ ವರ್ಣ *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ ವರ್ಣ **v = ರಿಕ್ತ;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        ಪ್ರತಿಜ್ಞಾ(s || l <= 0);

        ಯದಿ (drop_trailing_nuls)
                ಯಾವತ್ (l > 0 && s[l-1] == '\0')
                        l--;

        ಯದಿ (l <= 0)
                ಪ್ರತ್ಯಾಗಮ new0(ವರ್ಣ*, 1);

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = s; p < s + l; p++)
                ಯದಿ (*p == 0)
                        c++;

        ಯದಿ (s[l-1] != 0)
                c++;

        v = new0(ವರ್ಣ*, c+1);
        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = s; p < s + l;) {
                ಧ್ರುವ ವರ್ಣ *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                ಯದಿ (!v[i])
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                i++;

                ಯದಿ (!e)
                        ಭಙ್ಗ;

                p = e + 1;
        }

        ಪ್ರತಿಜ್ಞಾ(i == c);

        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(v);
}

ವರ್ಣ** strv_split_nulstr(ಧ್ರುವ ವರ್ಣ *s) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                ಯದಿ (strv_extend(&l, i) < 0)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ l ? TAKE_PTR(l) : strv_new(ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ strv_make_nulstr(ವರ್ಣ * ಧ್ರುವ *l, ವರ್ಣ **ret, size_t *ret_size) {
        _cleanup_free_ ವರ್ಣ *m = ರಿಕ್ತ;
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

                z = strlen(*i) + 1;

                ಯದಿ (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        ಯದಿ (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(ವರ್ಣ, 2);
                ಯದಿ (!m)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                n = 0;
        } ಅನ್ಯಥಾ
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        ಯದಿ (ret_size)
                *ret_size = n;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ set_make_nulstr(Set *s, ವರ್ಣ **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ ವರ್ಣ **strv = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        strv = set_get_strv(s);
        ಯದಿ (!strv)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_make_nulstr(strv, ret, ret_size);
}
