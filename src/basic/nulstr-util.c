/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "nulstr-util.h"
#అన్తర్భావ "set.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

స్థిరరాశి అక్షరం* nulstr_get(స్థిరరాశి అక్షరం *nulstr, స్థిరరాశి అక్షరం *needle) {
        ఒకవేళ (!nulstr)
                తిరిగి ఖాళీ;

        NULSTR_FOREACH(i, nulstr)
                ఒకవేళ (streq(i, needle))
                        తిరిగి i;

        తిరిగి ఖాళీ;
}

అక్షరం** strv_parse_nulstr_నిండిన(స్థిరరాశి అక్షరం *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ అక్షరం **v = ఖాళీ;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        ప్రతిజ్ఞా(s || l <= 0);

        ఒకవేళ (drop_trailing_nuls)
                అయితే (l > 0 && s[l-1] == '\0')
                        l--;

        ఒకవేళ (l <= 0)
                తిరిగి new0(అక్షరం*, 1);

        కొరకు (స్థిరరాశి అక్షరం *p = s; p < s + l; p++)
                ఒకవేళ (*p == 0)
                        c++;

        ఒకవేళ (s[l-1] != 0)
                c++;

        v = new0(అక్షరం*, c+1);
        ఒకవేళ (!v)
                తిరిగి ఖాళీ;

        కొరకు (స్థిరరాశి అక్షరం *p = s; p < s + l;) {
                స్థిరరాశి అక్షరం *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                ఒకవేళ (!v[i])
                        తిరిగి ఖాళీ;
                i++;

                ఒకవేళ (!e)
                        ఆపు;

                p = e + 1;
        }

        ప్రతిజ్ఞా(i == c);

        తిరిగి TAKE_PTR(v);
}

అక్షరం** strv_విభజించు_nulstr(స్థిరరాశి అక్షరం *s) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                ఒకవేళ (strv_extend(&l, i) < 0)
                        తిరిగి ఖాళీ;

        తిరిగి l ? TAKE_PTR(l) : strv_new(ఖాళీ);
}

పూర్ణాంకం strv_make_nulstr(అక్షరం * స్థిరరాశి *l, అక్షరం **ret, size_t *ret_కొలత) {
        _cleanup_free_ అక్షరం *m = ఖాళీ;
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

        ప్రతిజ్ఞా(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = అక్షరశ్రేణి_పొడవు(*i) + 1;

                ఒకవేళ (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        తిరిగి -ENOMEM;

                జ్ఞాపకం_నకలు(m + n, *i, z);
                n += z;
        }

        ఒకవేళ (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(అక్షరం, 2);
                ఒకవేళ (!m)
                        తిరిగి -ENOMEM;

                n = 0;
        } లేకపోతే
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        ఒకవేళ (ret_కొలత)
                *ret_కొలత = n;

        తిరిగి 0;
}

పూర్ణాంకం సమితి_make_nulstr(సమితి_2 *s, అక్షరం **ret, size_t *ret_కొలత) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ అక్షరం **strv = ఖాళీ;

        ప్రతిజ్ఞా(ret);

        strv = సమితి_get_strv(s);
        ఒకవేళ (!strv)
                తిరిగి -ENOMEM;

        తిరిగి strv_make_nulstr(strv, ret, ret_కొలత);
}
