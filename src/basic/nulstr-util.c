/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "nulstr-util.h"
#ضمن "set.h"
#ضمن "string-util.h"
#ضمن "strv.h"

ثابت حرف* nulstr_get(ثابت حرف *nulstr, ثابت حرف *needle) {
        إذا (!nulstr)
                أرجع عدم;

        NULSTR_FOREACH(i, nulstr)
                إذا (streq(i, needle))
                        أرجع i;

        أرجع عدم;
}

حرف** strv_parse_nulstr_full(ثابت حرف *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ حرف **v = عدم;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        أكد(s || l <= 0);

        إذا (drop_trailing_nuls)
                طالما (l > 0 && s[l-1] == '\0')
                        l--;

        إذا (l <= 0)
                أرجع new0(حرف*, 1);

        لكل (ثابت حرف *p = s; p < s + l; p++)
                إذا (*p == 0)
                        c++;

        إذا (s[l-1] != 0)
                c++;

        v = new0(حرف*, c+1);
        إذا (!v)
                أرجع عدم;

        لكل (ثابت حرف *p = s; p < s + l;) {
                ثابت حرف *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                إذا (!v[i])
                        أرجع عدم;
                i++;

                إذا (!e)
                        اكسر;

                p = e + 1;
        }

        أكد(i == c);

        أرجع TAKE_PTR(v);
}

حرف** strv_split_nulstr(ثابت حرف *s) {
        _cleanup_strv_free_ حرف **l = عدم;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                إذا (strv_extend(&l, i) < 0)
                        أرجع عدم;

        أرجع l ? TAKE_PTR(l) : strv_new(عدم);
}

صحيح strv_make_nulstr(حرف * ثابت *l, حرف **ret, size_t *ret_size) {
        _cleanup_free_ حرف *m = عدم;
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

                z = strlen(*i) + 1;

                إذا (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        أرجع -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        إذا (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(حرف, 2);
                إذا (!m)
                        أرجع -ENOMEM;

                n = 0;
        } وإلا
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        إذا (ret_size)
                *ret_size = n;

        أرجع 0;
}

صحيح set_make_nulstr(Set *s, حرف **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ حرف **strv = عدم;

        أكد(ret);

        strv = set_get_strv(s);
        إذا (!strv)
                أرجع -ENOMEM;

        أرجع strv_make_nulstr(strv, ret, ret_size);
}
