/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "nulstr-util.h"
#அந்தர்பாவ "set.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

மாறிலி எழுத்து* nulstr_get(மாறிலி எழுத்து *nulstr, மாறிலி எழுத்து *needle) {
        எனில் (!nulstr)
                திரும்பு வெற்று;

        NULSTR_FOREACH(i, nulstr)
                எனில் (streq(i, needle))
                        திரும்பு i;

        திரும்பு வெற்று;
}

எழுத்து** strv_parse_nulstr_full(மாறிலி எழுத்து *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ எழுத்து **v = வெற்று;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        ப்ரதிஜ்ஞா(s || l <= 0);

        எனில் (drop_trailing_nuls)
                போது (l > 0 && s[l-1] == '\0')
                        l--;

        எனில் (l <= 0)
                திரும்பு new0(எழுத்து*, 1);

        க்கு (மாறிலி எழுத்து *p = s; p < s + l; p++)
                எனில் (*p == 0)
                        c++;

        எனில் (s[l-1] != 0)
                c++;

        v = new0(எழுத்து*, c+1);
        எனில் (!v)
                திரும்பு வெற்று;

        க்கு (மாறிலி எழுத்து *p = s; p < s + l;) {
                மாறிலி எழுத்து *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                எனில் (!v[i])
                        திரும்பு வெற்று;
                i++;

                எனில் (!e)
                        நிறுத்து;

                p = e + 1;
        }

        ப்ரதிஜ்ஞா(i == c);

        திரும்பு TAKE_PTR(v);
}

எழுத்து** strv_split_nulstr(மாறிலி எழுத்து *s) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                எனில் (strv_extend(&l, i) < 0)
                        திரும்பு வெற்று;

        திரும்பு l ? TAKE_PTR(l) : strv_new(வெற்று);
}

முழு strv_make_nulstr(எழுத்து * மாறிலி *l, எழுத்து **ret, size_t *ret_size) {
        _cleanup_free_ எழுத்து *m = வெற்று;
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

        ப்ரதிஜ்ஞா(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = strlen(*i) + 1;

                எனில் (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        திரும்பு -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        எனில் (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(எழுத்து, 2);
                எனில் (!m)
                        திரும்பு -ENOMEM;

                n = 0;
        } இல்லையெனில்
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        எனில் (ret_size)
                *ret_size = n;

        திரும்பு 0;
}

முழு set_make_nulstr(Set *s, எழுத்து **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ எழுத்து **strv = வெற்று;

        ப்ரதிஜ்ஞா(ret);

        strv = set_get_strv(s);
        எனில் (!strv)
                திரும்பு -ENOMEM;

        திரும்பு strv_make_nulstr(strv, ret, ret_size);
}
