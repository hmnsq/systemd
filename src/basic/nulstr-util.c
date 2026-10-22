/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "nulstr-util.h"
#অন্তর্ভাৱ "set.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"

ধ্রুৱ ৱর্ণ* nulstr_get(ধ্রুৱ ৱর্ণ *nulstr, ধ্রুৱ ৱর্ণ *needle) {
        যদি (!nulstr)
                ঘূৰাই রিক্ত;

        NULSTR_FOREACH(i, nulstr)
                যদি (streq(i, needle))
                        ঘূৰাই i;

        ঘূৰাই রিক্ত;
}

ৱর্ণ** strv_parse_nulstr_full(ধ্রুৱ ৱর্ণ *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ ৱর্ণ **v = রিক্ত;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        প্রতিজ্ঞা(s || l <= 0);

        যদি (drop_trailing_nuls)
                যেতিয়ালৈ (l > 0 && s[l-1] == '\0')
                        l--;

        যদি (l <= 0)
                ঘূৰাই new0(ৱর্ণ*, 1);

        বাবে (ধ্রুৱ ৱর্ণ *p = s; p < s + l; p++)
                যদি (*p == 0)
                        c++;

        যদি (s[l-1] != 0)
                c++;

        v = new0(ৱর্ণ*, c+1);
        যদি (!v)
                ঘূৰাই রিক্ত;

        বাবে (ধ্রুৱ ৱর্ণ *p = s; p < s + l;) {
                ধ্রুৱ ৱর্ণ *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                যদি (!v[i])
                        ঘূৰাই রিক্ত;
                i++;

                যদি (!e)
                        ভঙ্গ;

                p = e + 1;
        }

        প্রতিজ্ঞা(i == c);

        ঘূৰাই TAKE_PTR(v);
}

ৱর্ণ** strv_split_nulstr(ধ্রুৱ ৱর্ণ *s) {
        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                যদি (strv_extend(&l, i) < 0)
                        ঘূৰাই রিক্ত;

        ঘূৰাই l ? TAKE_PTR(l) : strv_new(রিক্ত);
}

পূর্ণাঙ্ক strv_make_nulstr(ৱর্ণ * ধ্রুৱ *l, ৱর্ণ **ret, size_t *ret_size) {
        _cleanup_free_ ৱর্ণ *m = রিক্ত;
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

        প্রতিজ্ঞা(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = strlen(*i) + 1;

                যদি (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        ঘূৰাই -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        যদি (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(ৱর্ণ, 2);
                যদি (!m)
                        ঘূৰাই -ENOMEM;

                n = 0;
        } অন্যথা
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        যদি (ret_size)
                *ret_size = n;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক set_make_nulstr(Set *s, ৱর্ণ **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ ৱর্ণ **strv = রিক্ত;

        প্রতিজ্ঞা(ret);

        strv = set_get_strv(s);
        যদি (!strv)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_make_nulstr(strv, ret, ret_size);
}
