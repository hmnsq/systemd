/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ꦪꦼꦤ꧀ (!pc)
                ꦮꦭꦶ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (v < 0)
                ꦮꦭꦶ -ERANGE;

        ꦮꦭꦶ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ꦪꦼꦤ꧀ (!pc)
                ꦮꦭꦶ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ꦪꦼꦤ꧀ (dot) {
                ꦪꦼꦤ꧀ (dot + 2 != pc)
                        ꦮꦭꦶ -EINVAL;
                ꦪꦼꦤ꧀ (dot[1] < '0' || dot[1] > '9')
                        ꦮꦭꦶ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ꦭꦶꦪ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (v < 0)
                ꦮꦭꦶ -ERANGE;
        ꦪꦼꦤ꧀ (v > (INT_MAX - q) / 10)
                ꦮꦭꦶ -ERANGE;

        v = v * 10 + q;
        ꦮꦭꦶ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ꦪꦼꦤ꧀ (!pc)
                ꦮꦭꦶ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ꦪꦼꦤ꧀ (dot) {
                ꦪꦼꦤ꧀ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ꦪꦼꦤ꧀ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ꦮꦭꦶ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ꦭꦶꦪ ꦪꦼꦤ꧀ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ꦪꦼꦤ꧀ (dot[1] < '0' || dot[1] > '9')
                                ꦮꦭꦶ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ꦭꦶꦪ
                        /* We do not support zero or more than two places */
                        ꦮꦭꦶ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ꦭꦶꦪ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (v < 0)
                ꦮꦭꦶ -ERANGE;
        ꦪꦼꦤ꧀ (v > (INT_MAX - q) / 100)
                ꦮꦭꦶ -ERANGE;

        v = v * 100 + q;
        ꦮꦭꦶ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ꦮꦭꦶ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ꦪꦼꦤ꧀ (v > 100)
                ꦮꦭꦶ -ERANGE;

        ꦮꦭꦶ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ꦪꦼꦤ꧀ (pm)
                ꦮꦭꦶ parse_parts_value_whole(p, "‰");

        ꦮꦭꦶ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ꦪꦼꦤ꧀ (v > 1000)
                ꦮꦭꦶ -ERANGE;

        ꦮꦭꦶ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ꦪꦼꦤ꧀ (pm)
                ꦮꦭꦶ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ꦪꦼꦤ꧀ (pm)
                ꦮꦭꦶ parse_parts_value_with_tenths_place(p, "‰");

        ꦮꦭꦶ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ꦪꦼꦤ꧀ (v > 10000)
                ꦮꦭꦶ -ERANGE;

        ꦮꦭꦶ v;
}
