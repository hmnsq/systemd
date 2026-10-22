/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "nulstr-util.h"
#અન્તર્ભાવ "set.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

અચલ અક્ષર* nulstr_get(અચલ અક્ષર *nulstr, અચલ અક્ષર *needle) {
        જો (!nulstr)
                પાછા ખાલી;

        NULSTR_FOREACH(i, nulstr)
                જો (streq(i, needle))
                        પાછા i;

        પાછા ખાલી;
}

અક્ષર** strv_parse_nulstr_full(અચલ અક્ષર *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ અક્ષર **v = ખાલી;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        પ્રતિજ્ઞા(s || l <= 0);

        જો (drop_trailing_nuls)
                જ્યારે (l > 0 && s[l-1] == '\0')
                        l--;

        જો (l <= 0)
                પાછા new0(અક્ષર*, 1);

        માટે (અચલ અક્ષર *p = s; p < s + l; p++)
                જો (*p == 0)
                        c++;

        જો (s[l-1] != 0)
                c++;

        v = new0(અક્ષર*, c+1);
        જો (!v)
                પાછા ખાલી;

        માટે (અચલ અક્ષર *p = s; p < s + l;) {
                અચલ અક્ષર *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                જો (!v[i])
                        પાછા ખાલી;
                i++;

                જો (!e)
                        તોડો;

                p = e + 1;
        }

        પ્રતિજ્ઞા(i == c);

        પાછા TAKE_PTR(v);
}

અક્ષર** strv_split_nulstr(અચલ અક્ષર *s) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                જો (strv_extend(&l, i) < 0)
                        પાછા ખાલી;

        પાછા l ? TAKE_PTR(l) : strv_new(ખાલી);
}

પૂર્ણાંક strv_make_nulstr(અક્ષર * અચલ *l, અક્ષર **ret, size_t *ret_size) {
        _cleanup_free_ અક્ષર *m = ખાલી;
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

        પ્રતિજ્ઞા(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = strlen(*i) + 1;

                જો (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        પાછા -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        જો (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(અક્ષર, 2);
                જો (!m)
                        પાછા -ENOMEM;

                n = 0;
        } નહીંતર
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        જો (ret_size)
                *ret_size = n;

        પાછા 0;
}

પૂર્ણાંક set_make_nulstr(Set *s, અક્ષર **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ અક્ષર **strv = ખાલી;

        પ્રતિજ્ઞા(ret);

        strv = set_get_strv(s);
        જો (!strv)
                પાછા -ENOMEM;

        પાછા strv_make_nulstr(strv, ret, ret_size);
}
