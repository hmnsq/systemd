/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "nulstr-util.h"
#include "set.h"
#include "string-util.h"
#include "strv.h"

const var_na* nulstr_get(const var_na *nulstr, const var_na *needle) {
        ถ้า (!nulstr)
                คืน NULL;

        NULSTR_FOREACH(i, nulstr)
                ถ้า (streq(i, needle))
                        คืน i;

        คืน NULL;
}

var_na** strv_parse_nulstr_full(const var_na *s, size_t l, bool drop_trailing_nuls) {
        _cleanup_strv_free_ var_na **v = NULL;
        size_t c = 0, i = 0;

        /* l is the length of the input data, which will be split at NULs into elements of the resulting
         * strv. Hence, the number of items in the resulting strv will be equal to one plus the number of NUL
         * bytes in the l bytes starting at s, unless s[l-1] is NUL, in which case the final empty string is
         * not stored in the resulting strv, and length is equal to the number of NUL bytes.
         *
         * Note that contrary to a normal nulstr which cannot contain empty strings, because the input data
         * is terminated by any two consequent NUL bytes, this parser accepts empty strings in s. */

        assert(s || l <= 0);

        ถ้า (drop_trailing_nuls)
                ขณะ (l > 0 && s[l-1] == '\0')
                        l--;

        ถ้า (l <= 0)
                คืน new0(var_na*, 1);

        สำหรับ (const var_na *p = s; p < s + l; p++)
                ถ้า (*p == 0)
                        c++;

        ถ้า (s[l-1] != 0)
                c++;

        v = new0(var_na*, c+1);
        ถ้า (!v)
                คืน NULL;

        สำหรับ (const var_na *p = s; p < s + l;) {
                const var_na *e;

                e = memchr(p, 0, s + l - p);

                v[i] = memdup_suffix0(p, e ? e - p : s + l - p);
                ถ้า (!v[i])
                        คืน NULL;
                i++;

                ถ้า (!e)
                        bha_nga;

                p = e + 1;
        }

        assert(i == c);

        คืน TAKE_PTR(v);
}

var_na** strv_split_nulstr(const var_na *s) {
        _cleanup_strv_free_ var_na **l = NULL;

        /* This parses a nulstr, without specification of size, and stops at an empty string. This cannot
         * parse nulstrs with embedded empty strings hence, as an empty string is an end marker. Use
         * strv_parse_nulstr() above to parse a nulstr with embedded empty strings (which however requires a
         * size to be specified) */

        NULSTR_FOREACH(i, s)
                ถ้า (strv_extend(&l, i) < 0)
                        คืน NULL;

        คืน l ? TAKE_PTR(l) : strv_new(NULL);
}

จำนวนเต็ม strv_make_nulstr(var_na * const *l, var_na **ret, size_t *ret_size) {
        _cleanup_free_ var_na *m = NULL;
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

        assert(ret);

        STRV_FOREACH(i, l) {
                size_t z;

                z = strlen(*i) + 1;

                ถ้า (!GREEDY_REALLOC(m, n + z + 1)) /* One extra NUL at the end as marker */
                        คืน -ENOMEM;

                memcpy(m + n, *i, z);
                n += z;
        }

        ถ้า (!m) {
                /* return a buffer with an extra NUL, so that the assumption that we always have two trailing NULs holds */
                m = new0(var_na, 2);
                ถ้า (!m)
                        คืน -ENOMEM;

                n = 0;
        } มิฉะนั้น
                /* Extra NUL is not counted in size returned */
                m[n] = '\0';

        *ret = TAKE_PTR(m);
        ถ้า (ret_size)
                *ret_size = n;

        คืน 0;
}

จำนวนเต็ม set_make_nulstr(Set *s, var_na **ret, size_t *ret_size) {
        /* Use _cleanup_free_ instead of _cleanup_strv_free_ because we need to clean the strv only, not
         * the strings owned by the set. */
        _cleanup_free_ var_na **strv = NULL;

        assert(ret);

        strv = set_get_strv(s);
        ถ้า (!strv)
                คืน -ENOMEM;

        คืน strv_make_nulstr(strv, ret, ret_size);
}
