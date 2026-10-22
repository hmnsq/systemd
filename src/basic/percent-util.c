/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ᨀᨘ (!pc)
                ᨀᨘᨒ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (v < 0)
                ᨀᨘᨒ -ERANGE;

        ᨀᨘᨒ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᨀᨘ (!pc)
                ᨀᨘᨒ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᨀᨘ (dot) {
                ᨀᨘ (dot + 2 != pc)
                        ᨀᨘᨒ -EINVAL;
                ᨀᨘ (dot[1] < '0' || dot[1] > '9')
                        ᨀᨘᨒ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ᨀᨘᨁ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (v < 0)
                ᨀᨘᨒ -ERANGE;
        ᨀᨘ (v > (INT_MAX - q) / 10)
                ᨀᨘᨒ -ERANGE;

        v = v * 10 + q;
        ᨀᨘᨒ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᨀᨘ (!pc)
                ᨀᨘᨒ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᨀᨘ (dot) {
                ᨀᨘ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ᨀᨘ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ᨀᨘᨒ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ᨀᨘᨁ ᨀᨘ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ᨀᨘ (dot[1] < '0' || dot[1] > '9')
                                ᨀᨘᨒ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ᨀᨘᨁ
                        /* We do not support zero or more than two places */
                        ᨀᨘᨒ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ᨀᨘᨁ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (v < 0)
                ᨀᨘᨒ -ERANGE;
        ᨀᨘ (v > (INT_MAX - q) / 100)
                ᨀᨘᨒ -ERANGE;

        v = v * 100 + q;
        ᨀᨘᨒ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ᨀᨘᨒ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ᨀᨘ (v > 100)
                ᨀᨘᨒ -ERANGE;

        ᨀᨘᨒ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ᨀᨘ (pm)
                ᨀᨘᨒ parse_parts_value_whole(p, "‰");

        ᨀᨘᨒ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ᨀᨘ (v > 1000)
                ᨀᨘᨒ -ERANGE;

        ᨀᨘᨒ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ᨀᨘ (pm)
                ᨀᨘᨒ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ᨀᨘ (pm)
                ᨀᨘᨒ parse_parts_value_with_tenths_place(p, "‰");

        ᨀᨘᨒ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ᨀᨘ (v > 10000)
                ᨀᨘᨒ -ERANGE;

        ᨀᨘᨒ v;
}
