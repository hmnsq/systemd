/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "nulstr-util.h"
#شامل "set.h"
#شامل "string-util.h"
#شامل "strv.h"

ثابت حرف* nulstr_get(ثابت حرف *nulstr, ثابت حرف *needle) {
        اگر (!nulstr)
                واپس تہی;

        NULSTR_FOREACH(i, nulstr)
                اگر (streq(i, needle))
                        واپس i;

        واپس تہی;
}

حرف** strv_parse_nulstr_بھرا(ثابت حرف *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ حرف **v = تہی;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        أكد(s || l <= 0);

        اگر (drop_trailing_nuls)
                جب (l > 0 && s[l-1] == '\0')
                        l--;

        اگر (l <= 0)
                واپس new0(حرف*, 1);

        برائے (ثابت حرف *p = s; p < s + l; p++)
                اگر (*p == 0)
                        c++;

        اگر (s[l-1] != 0)
                c++;

        v = new0(حرف*, c+1);
        اگر (!v)
                واپس تہی;

        برائے (ثابت حرف *p = s; p < s + l;) {
                ثابت حرف *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                اگر (!v[i])
                        واپس تہی;
                i++;

                اگر (!e)
                        توڑو;

                p = e + 1;
        }

        أكد(i == c);

        واپس TAKE_PTR(v);
}

حرف** strv_بانٹو_nulstr(ثابت حرف *s) {
        _cleanup_strv_free_ حرف **l = تہی;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                اگر (strv_extend(&l, i) < 0)
                        واپس تہی;

        واپس l ? TAKE_PTR(l) : strv_new(تہی);
}

صحیح strv_make_nulstr(حرف * ثابت *l, حرف **ret, size_t *ret_سائز) {
        _cleanup_free_ حرف *m = تہی;
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

        أكد(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = لڑی_لمبائی(*i) + 1;

                اگر (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        واپس -ENOMEM;

                حافظہ_نقل(m + n, *i, z);
                n += z;
        }

        اگر (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(حرف, 2);
                اگر (!m)
                        واپس -ENOMEM;

                n = 0;
        } ورنہ
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        اگر (ret_سائز)
                *ret_سائز = n;

        واپس 0;
}

صحیح سیٹ_make_nulstr(سیٹ_2 *s, حرف **ret, size_t *ret_سائز) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ حرف **strv = تہی;

        أكد(ret);

        strv = سیٹ_get_strv(s);
        اگر (!strv)
                واپس -ENOMEM;

        واپس strv_make_nulstr(strv, ret, ret_سائز);
}
