/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "nulstr-util.h"
#अन्तर्भाव "set.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

ध्रुव वर्ण* nulstr_get(ध्रुव वर्ण *nulstr, ध्रुव वर्ण *needle) {
        जइ (!nulstr)
                प्रत्यागम रिक्त;

        NULSTR_FOREACH(i, nulstr)
                जइ (streq(i, needle))
                        प्रत्यागम i;

        प्रत्यागम रिक्त;
}

वर्ण** strv_parse_nulstr_full(ध्रुव वर्ण *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ वर्ण **v = रिक्त;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        प्रतिज्ञा(s || l <= 0);

        जइ (drop_trailing_nuls)
                जाव (l > 0 && s[l-1] == '\0')
                        l--;

        जइ (l <= 0)
                प्रत्यागम new0(वर्ण*, 1);

        प्रति (ध्रुव वर्ण *p = s; p < s + l; p++)
                जइ (*p == 0)
                        c++;

        जइ (s[l-1] != 0)
                c++;

        v = new0(वर्ण*, c+1);
        जइ (!v)
                प्रत्यागम रिक्त;

        प्रति (ध्रुव वर्ण *p = s; p < s + l;) {
                ध्रुव वर्ण *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                जइ (!v[i])
                        प्रत्यागम रिक्त;
                i++;

                जइ (!e)
                        भङ्ग;

                p = e + 1;
        }

        प्रतिज्ञा(i == c);

        प्रत्यागम TAKE_PTR(v);
}

वर्ण** strv_split_nulstr(ध्रुव वर्ण *s) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                जइ (strv_extend(&l, i) < 0)
                        प्रत्यागम रिक्त;

        प्रत्यागम l ? TAKE_PTR(l) : strv_new(रिक्त);
}

पूर्णाङ्क strv_make_nulstr(वर्ण * ध्रुव *l, वर्ण **ret, size_t *ret_size) {
        _cleanup_free_ वर्ण *m = रिक्त;
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

        प्रतिज्ञा(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = strlen(*i) + 1;

                जइ (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        प्रत्यागम -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        जइ (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(वर्ण, 2);
                जइ (!m)
                        प्रत्यागम -ENOMEM;

                n = 0;
        } अण्णहा
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        जइ (ret_size)
                *ret_size = n;

        प्रत्यागम 0;
}

पूर्णाङ्क set_make_nulstr(Set *s, वर्ण **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ वर्ण **strv = रिक्त;

        प्रतिज्ञा(ret);

        strv = set_get_strv(s);
        जइ (!strv)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_make_nulstr(strv, ret, ret_size);
}
