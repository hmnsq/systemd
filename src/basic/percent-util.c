/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ᱡᱩᱫᱤ (!pc)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (v < 0)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        ᱨᱩᱣᱟᱹᱲ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᱡᱩᱫᱤ (!pc)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᱡᱩᱫᱤ (dot) {
                ᱡᱩᱫᱤ (dot + 2 != pc)
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;
                ᱡᱩᱫᱤ (dot[1] < '0' || dot[1] > '9')
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ᱵᱟᱝ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (v < 0)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;
        ᱡᱩᱫᱤ (v > (INT_MAX - q) / 10)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        v = v * 10 + q;
        ᱨᱩᱣᱟᱹᱲ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᱡᱩᱫᱤ (!pc)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᱡᱩᱫᱤ (dot) {
                ᱡᱩᱫᱤ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ᱡᱩᱫᱤ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ᱨᱩᱣᱟᱹᱲ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ᱵᱟᱝ ᱡᱩᱫᱤ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ᱡᱩᱫᱤ (dot[1] < '0' || dot[1] > '9')
                                ᱨᱩᱣᱟᱹᱲ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ᱵᱟᱝ
                        /* We do not support zero or more than two places */
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ᱵᱟᱝ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (v < 0)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;
        ᱡᱩᱫᱤ (v > (INT_MAX - q) / 100)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        v = v * 100 + q;
        ᱨᱩᱣᱟᱹᱲ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ᱨᱩᱣᱟᱹᱲ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ᱡᱩᱫᱤ (v > 100)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        ᱨᱩᱣᱟᱹᱲ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ᱡᱩᱫᱤ (pm)
                ᱨᱩᱣᱟᱹᱲ parse_parts_value_whole(p, "‰");

        ᱨᱩᱣᱟᱹᱲ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ᱡᱩᱫᱤ (v > 1000)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        ᱨᱩᱣᱟᱹᱲ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ᱡᱩᱫᱤ (pm)
                ᱨᱩᱣᱟᱹᱲ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ᱡᱩᱫᱤ (pm)
                ᱨᱩᱣᱟᱹᱲ parse_parts_value_with_tenths_place(p, "‰");

        ᱨᱩᱣᱟᱹᱲ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ᱡᱩᱫᱤ (v > 10000)
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        ᱨᱩᱣᱟᱹᱲ v;
}
