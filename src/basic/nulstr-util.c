/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "nulstr-util.h"
#अन्तर्भाव "set.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

स्थिरांक अक्षर* nulstr_get(स्थिरांक अक्षर *nulstr, स्थिरांक अक्षर *needle) {
        जर (!nulstr)
                परत रिक्त;

        NULSTR_FOREACH(i, nulstr)
                जर (streq(i, needle))
                        परत i;

        परत रिक्त;
}

अक्षर** strv_parse_nulstr_full(स्थिरांक अक्षर *s, माप_प्रकार l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ अक्षर **v = रिक्त;
        माप_प्रकार c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        प्रतिज्ञा(s || l <= 0);

        जर (drop_trailing_nuls)
                पर्यंत (l > 0 && s[l-1] == '\0')
                        l--;

        जर (l <= 0)
                परत new0(अक्षर*, 1);

        साठी (स्थिरांक अक्षर *p = s; p < s + l; p++)
                जर (*p == 0)
                        c++;

        जर (s[l-1] != 0)
                c++;

        v = new0(अक्षर*, c+1);
        जर (!v)
                परत रिक्त;

        साठी (स्थिरांक अक्षर *p = s; p < s + l;) {
                स्थिरांक अक्षर *e;

                e = स_प्रथम(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                जर (!v[i])
                        परत रिक्त;
                i++;

                जर (!e)
                        थांबा;

                p = e + 1;
        }

        प्रतिज्ञा(i == c);

        परत TAKE_PTR(v);
}

अक्षर** strv_split_nulstr(स्थिरांक अक्षर *s) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                जर (strv_extend(&l, i) < 0)
                        परत रिक्त;

        परत l ? TAKE_PTR(l) : strv_new(रिक्त);
}

पूर्णांक strv_make_nulstr(अक्षर * स्थिरांक *l, अक्षर **ret, माप_प्रकार *ret_size) {
        _cleanup_free_ अक्षर *m = रिक्त;
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

        प्रतिज्ञा(ret);

        STRV_FOREACH(i, l) {
                माप_प्रकार z;

                z = म_माप(*i) + 1;

                जर (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        परत -ENOMEM;

                स_नकल(m + n, *i, z);
                n += z;
        }

        जर (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(अक्षर, 2);
                जर (!m)
                        परत -ENOMEM;

                n = 0;
        } अन्यथा
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        जर (ret_size)
                *ret_size = n;

        परत 0;
}

पूर्णांक set_make_nulstr(Set *s, अक्षर **ret, माप_प्रकार *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ अक्षर **strv = रिक्त;

        प्रतिज्ञा(ret);

        strv = set_get_strv(s);
        जर (!strv)
                परत -ENOMEM;

        परत strv_make_nulstr(strv, ret, ret_size);
}
