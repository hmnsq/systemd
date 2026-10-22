/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "nulstr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "set.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ਅਚਲ ਅੱਖਰ* nulstr_get(ਅਚਲ ਅੱਖਰ *nulstr, ਅਚਲ ਅੱਖਰ *needle) {
        ਜੇ (!nulstr)
                ਵਾਪਸ ਨਿਰਾ;

        NULSTR_FOREACH(i, nulstr)
                ਜੇ (streq(i, needle))
                        ਵਾਪਸ i;

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ** strv_parse_nulstr_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ ਅੱਖਰ **v = ਨਿਰਾ;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        ਪ੍ਰਤਿਜ੍ਞਾ(s || l <= 0);

        ਜੇ (drop_trailing_nuls)
                ਜਦੋਂ (l > 0 && s[l-1] == '\0')
                        l--;

        ਜੇ (l <= 0)
                ਵਾਪਸ new0(ਅੱਖਰ*, 1);

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = s; p < s + l; p++)
                ਜੇ (*p == 0)
                        c++;

        ਜੇ (s[l-1] != 0)
                c++;

        v = new0(ਅੱਖਰ*, c+1);
        ਜੇ (!v)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = s; p < s + l;) {
                ਅਚਲ ਅੱਖਰ *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                ਜੇ (!v[i])
                        ਵਾਪਸ ਨਿਰਾ;
                i++;

                ਜੇ (!e)
                        ਰੋਕੋ;

                p = e + 1;
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(i == c);

        ਵਾਪਸ TAKE_PTR(v);
}

ਅੱਖਰ** strv_ਵੰਡੋ_nulstr(ਅਚਲ ਅੱਖਰ *s) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                ਜੇ (strv_extend(&l, i) < 0)
                        ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ l ? TAKE_PTR(l) : strv_new(ਨਿਰਾ);
}

ਪੂਰਨ strv_make_nulstr(ਅੱਖਰ * ਅਚਲ *l, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ) {
        _cleanup_free_ ਅੱਖਰ *m = ਨਿਰਾ;
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

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = ਲੜੀ_ਲੰਬਾਈ(*i) + 1;

                ਜੇ (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        ਵਾਪਸ -ENOMEM;

                ਸਮ੍ਰਿਤੀ_ਨਕਲ(m + n, *i, z);
                n += z;
        }

        ਜੇ (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(ਅੱਖਰ, 2);
                ਜੇ (!m)
                        ਵਾਪਸ -ENOMEM;

                n = 0;
        } ਨਹੀਂਤਾਂ
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = n;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੈੱਟ_make_nulstr(ਸੈੱਟ_2 *s, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ ਅੱਖਰ **strv = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        strv = ਸੈੱਟ_get_strv(s);
        ਜੇ (!strv)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_make_nulstr(strv, ret, ret_ਮਾਪ);
}
