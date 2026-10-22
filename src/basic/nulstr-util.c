/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "nulstr-util.h"
#समावेश "set.h"
#समावेश "string-util.h"
#समावेश "strv.h"

स्थिर अक्षर* nulstr_get(स्थिर अक्षर *nulstr, स्थिर अक्षर *needle) {
        अगर (!nulstr)
                वापस शून्य;

        NULSTR_FOREACH(i, nulstr)
                अगर (streq(i, needle))
                        वापस i;

        वापस शून्य;
}

अक्षर** strv_parse_nulstr_full(स्थिर अक्षर *s, माप_प्रकार l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ अक्षर **v = शून्य;
        माप_प्रकार c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        निश्चित(s || l <= 0);

        अगर (drop_trailing_nuls)
                जबतक (l > 0 && s[l-1] == '\0')
                        l--;

        अगर (l <= 0)
                वापस new0(अक्षर*, 1);

        क्रम (स्थिर अक्षर *p = s; p < s + l; p++)
                अगर (*p == 0)
                        c++;

        अगर (s[l-1] != 0)
                c++;

        v = new0(अक्षर*, c+1);
        अगर (!v)
                वापस शून्य;

        क्रम (स्थिर अक्षर *p = s; p < s + l;) {
                स्थिर अक्षर *e;

                e = स_प्रथम(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                अगर (!v[i])
                        वापस शून्य;
                i++;

                अगर (!e)
                        अवरोध;

                p = e + 1;
        }

        निश्चित(i == c);

        वापस TAKE_PTR(v);
}

अक्षर** strv_split_nulstr(स्थिर अक्षर *s) {
        _cleanup_strv_free_ अक्षर **l = शून्य;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                अगर (strv_extend(&l, i) < 0)
                        वापस शून्य;

        वापस l ? TAKE_PTR(l) : strv_new(शून्य);
}

पूर्णांक strv_make_nulstr(अक्षर * स्थिर *l, अक्षर **ret, माप_प्रकार *ret_आकार) {
        _cleanup_free_ अक्षर *m = शून्य;
        माप_प्रकार n = 0;

        /* Builds a nulstr and returns it together with the size. An extra NUL byte will be appended (⚠️ but
         * not included in the size! ⚠️). This is done so that the nulstr can be used both in
         * strv_parse_nulstr() and in NULSTR_FOREACH()/strv_split_nulstr() contexts, i.e. with and without a
         * size parameter. In the former case we can include empty strings, in the latter case we cannot (as
         * that is the end marker).
         *
         * When NULSTR_FOREACH()/strv_split_nulstr() is used it is often assumed that the nulstr ends in two
         * NUL bytes (which it will, if not empty). To ensure that this assumption *always* holds, we'll
         * return a buffer with two NUL bytes in that case, but return a size of zero. */

        निश्चित(ret);

        STRV_FOREACH(i, l) {
                माप_प्रकार z;

                z = म_माप(*i) + 1;

                अगर (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        वापस -ENOMEM;

                स_नकल(m + n, *i, z);
                n += z;
        }

        अगर (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(अक्षर, 2);
                अगर (!m)
                        वापस -ENOMEM;

                n = 0;
        } अन्यथा
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        अगर (ret_आकार)
                *ret_आकार = n;

        वापस 0;
}

पूर्णांक set_make_nulstr(Set *s, अक्षर **ret, माप_प्रकार *ret_आकार) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ अक्षर **strv = शून्य;

        निश्चित(ret);

        strv = set_get_strv(s);
        अगर (!strv)
                वापस -ENOMEM;

        वापस strv_make_nulstr(strv, ret, ret_आकार);
}
