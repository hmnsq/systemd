/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "nulstr-util.h"
#ଅନ୍ତର୍ଭାଵ "set.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ଅଚଳ ଅକ୍ଷର* nulstr_get(ଅଚଳ ଅକ୍ଷର *nulstr, ଅଚଳ ଅକ୍ଷର *needle) {
        ଯଦି (!nulstr)
                ଫେରନ୍ତୁ ଖାଲି;

        NULSTR_FOREACH(i, nulstr)
                ଯଦି (streq(i, needle))
                        ଫେରନ୍ତୁ i;

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର** strv_parse_nulstr_full(ଅଚଳ ଅକ୍ଷର *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ ଅକ୍ଷର **v = ଖାଲି;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        ପ୍ରତିଜ୍ଞା(s || l <= 0);

        ଯଦି (drop_trailing_nuls)
                ଯେତେବେଳେ (l > 0 && s[l-1] == '\0')
                        l--;

        ଯଦି (l <= 0)
                ଫେରନ୍ତୁ new0(ଅକ୍ଷର*, 1);

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = s; p < s + l; p++)
                ଯଦି (*p == 0)
                        c++;

        ଯଦି (s[l-1] != 0)
                c++;

        v = new0(ଅକ୍ଷର*, c+1);
        ଯଦି (!v)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = s; p < s + l;) {
                ଅଚଳ ଅକ୍ଷର *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                ଯଦି (!v[i])
                        ଫେରନ୍ତୁ ଖାଲି;
                i++;

                ଯଦି (!e)
                        ଭାଙ୍ଗ;

                p = e + 1;
        }

        ପ୍ରତିଜ୍ଞା(i == c);

        ଫେରନ୍ତୁ TAKE_PTR(v);
}

ଅକ୍ଷର** strv_split_nulstr(ଅଚଳ ଅକ୍ଷର *s) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                ଯଦି (strv_extend(&l, i) < 0)
                        ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ l ? TAKE_PTR(l) : strv_new(ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_make_nulstr(ଅକ୍ଷର * ଅଚଳ *l, ଅକ୍ଷର **ret, size_t *ret_size) {
        _cleanup_free_ ଅକ୍ଷର *m = ଖାଲି;
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

        ପ୍ରତିଜ୍ଞା(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = strlen(*i) + 1;

                ଯଦି (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        ଫେରନ୍ତୁ -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        ଯଦି (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(ଅକ୍ଷର, 2);
                ଯଦି (!m)
                        ଫେରନ୍ତୁ -ENOMEM;

                n = 0;
        } ଅନ୍ୟଥା
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        ଯଦି (ret_size)
                *ret_size = n;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_make_nulstr(Set *s, ଅକ୍ଷର **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ ଅକ୍ଷର **strv = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(ret);

        strv = set_get_strv(s);
        ଯଦି (!strv)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_make_nulstr(strv, ret, ret_size);
}
