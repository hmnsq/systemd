/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "nulstr-util.h"
#അന്തര്ഭാവ "set.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

സ്ഥിരം അക്ഷരം* nulstr_get(സ്ഥിരം അക്ഷരം *nulstr, സ്ഥിരം അക്ഷരം *needle) {
        എങ്കിൽ (!nulstr)
                തിരികെ ശൂന്യ;

        NULSTR_FOREACH(i, nulstr)
                എങ്കിൽ (streq(i, needle))
                        തിരികെ i;

        തിരികെ ശൂന്യ;
}

അക്ഷരം** strv_parse_nulstr_full(സ്ഥിരം അക്ഷരം *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ അക്ഷരം **v = ശൂന്യ;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        പ്രതിജ്ഞാ(s || l <= 0);

        എങ്കിൽ (drop_trailing_nuls)
                ആകുമ്പോൾ (l > 0 && s[l-1] == '\0')
                        l--;

        എങ്കിൽ (l <= 0)
                തിരികെ new0(അക്ഷരം*, 1);

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = s; p < s + l; p++)
                എങ്കിൽ (*p == 0)
                        c++;

        എങ്കിൽ (s[l-1] != 0)
                c++;

        v = new0(അക്ഷരം*, c+1);
        എങ്കിൽ (!v)
                തിരികെ ശൂന്യ;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = s; p < s + l;) {
                സ്ഥിരം അക്ഷരം *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                എങ്കിൽ (!v[i])
                        തിരികെ ശൂന്യ;
                i++;

                എങ്കിൽ (!e)
                        നിർത്തുക;

                p = e + 1;
        }

        പ്രതിജ്ഞാ(i == c);

        തിരികെ TAKE_PTR(v);
}

അക്ഷരം** strv_split_nulstr(സ്ഥിരം അക്ഷരം *s) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                എങ്കിൽ (strv_extend(&l, i) < 0)
                        തിരികെ ശൂന്യ;

        തിരികെ l ? TAKE_PTR(l) : strv_new(ശൂന്യ);
}

പൂർണ്ണസംഖ്യ strv_make_nulstr(അക്ഷരം * സ്ഥിരം *l, അക്ഷരം **ret, size_t *ret_size) {
        _cleanup_free_ അക്ഷരം *m = ശൂന്യ;
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

        പ്രതിജ്ഞാ(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = strlen(*i) + 1;

                എങ്കിൽ (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        തിരികെ -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        എങ്കിൽ (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(അക്ഷരം, 2);
                എങ്കിൽ (!m)
                        തിരികെ -ENOMEM;

                n = 0;
        } അല്ലെങ്കിൽ
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        എങ്കിൽ (ret_size)
                *ret_size = n;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ set_make_nulstr(Set *s, അക്ഷരം **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ അക്ഷരം **strv = ശൂന്യ;

        പ്രതിജ്ഞാ(ret);

        strv = set_get_strv(s);
        എങ്കിൽ (!strv)
                തിരികെ -ENOMEM;

        തിരികെ strv_make_nulstr(strv, ret, ret_size);
}
