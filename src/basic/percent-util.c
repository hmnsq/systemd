/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ᤀᤡ᤺ (!pc)
                ᤀᤡᤴ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (v < 0)
                ᤀᤡᤴ -ERANGE;

        ᤀᤡᤴ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᤀᤡ᤺ (!pc)
                ᤀᤡᤴ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᤀᤡ᤺ (dot) {
                ᤀᤡ᤺ (dot + 2 != pc)
                        ᤀᤡᤴ -EINVAL;
                ᤀᤡ᤺ (dot[1] < '0' || dot[1] > '9')
                        ᤀᤡᤴ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ᤀᤡ᤺ᤆᤡ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (v < 0)
                ᤀᤡᤴ -ERANGE;
        ᤀᤡ᤺ (v > (INT_MAX - q) / 10)
                ᤀᤡᤴ -ERANGE;

        v = v * 10 + q;
        ᤀᤡᤴ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᤀᤡ᤺ (!pc)
                ᤀᤡᤴ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᤀᤡ᤺ (dot) {
                ᤀᤡ᤺ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ᤀᤡ᤺ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ᤀᤡᤴ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ᤀᤡ᤺ (dot[1] < '0' || dot[1] > '9')
                                ᤀᤡᤴ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ᤀᤡ᤺ᤆᤡ
                        /* We do not support zero or more than two places */
                        ᤀᤡᤴ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ᤀᤡ᤺ᤆᤡ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (v < 0)
                ᤀᤡᤴ -ERANGE;
        ᤀᤡ᤺ (v > (INT_MAX - q) / 100)
                ᤀᤡᤴ -ERANGE;

        v = v * 100 + q;
        ᤀᤡᤴ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ᤀᤡᤴ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ᤀᤡ᤺ (v > 100)
                ᤀᤡᤴ -ERANGE;

        ᤀᤡᤴ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ᤀᤡ᤺ (pm)
                ᤀᤡᤴ parse_parts_value_whole(p, "‰");

        ᤀᤡᤴ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ᤀᤡ᤺ (v > 1000)
                ᤀᤡᤴ -ERANGE;

        ᤀᤡᤴ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ᤀᤡ᤺ (pm)
                ᤀᤡᤴ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ᤀᤡ᤺ (pm)
                ᤀᤡᤴ parse_parts_value_with_tenths_place(p, "‰");

        ᤀᤡᤴ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ᤀᤡ᤺ (v > 10000)
                ᤀᤡᤴ -ERANGE;

        ᤀᤡᤴ v;
}
