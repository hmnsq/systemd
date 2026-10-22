/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ᯀᯮᯝ (!pc)
                ᯀᯮᯞ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (v < 0)
                ᯀᯮᯞ -ERANGE;

        ᯀᯮᯞ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᯀᯮᯝ (!pc)
                ᯀᯮᯞ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᯀᯮᯝ (dot) {
                ᯀᯮᯝ (dot + 2 != pc)
                        ᯀᯮᯞ -EINVAL;
                ᯀᯮᯝ (dot[1] < '0' || dot[1] > '9')
                        ᯀᯮᯞ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ᯀᯮᯝᯂᯪ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (v < 0)
                ᯀᯮᯞ -ERANGE;
        ᯀᯮᯝ (v > (INT_MAX - q) / 10)
                ᯀᯮᯞ -ERANGE;

        v = v * 10 + q;
        ᯀᯮᯞ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᯀᯮᯝ (!pc)
                ᯀᯮᯞ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᯀᯮᯝ (dot) {
                ᯀᯮᯝ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ᯀᯮᯝ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ᯀᯮᯞ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ᯀᯮᯝ (dot[1] < '0' || dot[1] > '9')
                                ᯀᯮᯞ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ᯀᯮᯝᯂᯪ
                        /* We do not support zero or more than two places */
                        ᯀᯮᯞ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ᯀᯮᯝᯂᯪ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (v < 0)
                ᯀᯮᯞ -ERANGE;
        ᯀᯮᯝ (v > (INT_MAX - q) / 100)
                ᯀᯮᯞ -ERANGE;

        v = v * 100 + q;
        ᯀᯮᯞ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ᯀᯮᯞ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ᯀᯮᯝ (v > 100)
                ᯀᯮᯞ -ERANGE;

        ᯀᯮᯞ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ᯀᯮᯝ (pm)
                ᯀᯮᯞ parse_parts_value_whole(p, "‰");

        ᯀᯮᯞ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ᯀᯮᯝ (v > 1000)
                ᯀᯮᯞ -ERANGE;

        ᯀᯮᯞ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ᯀᯮᯝ (pm)
                ᯀᯮᯞ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ᯀᯮᯝ (pm)
                ᯀᯮᯞ parse_parts_value_with_tenths_place(p, "‰");

        ᯀᯮᯞ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ᯀᯮᯝ (v > 10000)
                ᯀᯮᯞ -ERANGE;

        ᯀᯮᯞ v;
}
