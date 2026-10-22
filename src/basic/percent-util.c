/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ᰀᰧ (!pc)
                ᰀᰧᰌ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;
        ᰀᰧ (v < 0)
                ᰀᰧᰌ -ERANGE;

        ᰀᰧᰌ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᰀᰧ (!pc)
                ᰀᰧᰌ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᰀᰧ (dot) {
                ᰀᰧ (dot + 2 != pc)
                        ᰀᰧᰌ -EINVAL;
                ᰀᰧ (dot[1] < '0' || dot[1] > '9')
                        ᰀᰧᰌ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ᰀᰧᰀ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;
        ᰀᰧ (v < 0)
                ᰀᰧᰌ -ERANGE;
        ᰀᰧ (v > (INT_MAX - q) / 10)
                ᰀᰧᰌ -ERANGE;

        v = v * 10 + q;
        ᰀᰧᰌ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᰀᰧ (!pc)
                ᰀᰧᰌ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᰀᰧ (dot) {
                ᰀᰧ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ᰀᰧ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ᰀᰧᰌ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ᰀᰧᰀ ᰀᰧ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ᰀᰧ (dot[1] < '0' || dot[1] > '9')
                                ᰀᰧᰌ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ᰀᰧᰀ
                        /* We do not support zero or more than two places */
                        ᰀᰧᰌ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ᰀᰧᰀ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;
        ᰀᰧ (v < 0)
                ᰀᰧᰌ -ERANGE;
        ᰀᰧ (v > (INT_MAX - q) / 100)
                ᰀᰧᰌ -ERANGE;

        v = v * 100 + q;
        ᰀᰧᰌ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ᰀᰧᰌ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ᰀᰧ (v > 100)
                ᰀᰧᰌ -ERANGE;

        ᰀᰧᰌ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ᰀᰧ (pm)
                ᰀᰧᰌ parse_parts_value_whole(p, "‰");

        ᰀᰧᰌ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ᰀᰧ (v > 1000)
                ᰀᰧᰌ -ERANGE;

        ᰀᰧᰌ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ᰀᰧ (pm)
                ᰀᰧᰌ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ᰀᰧ (pm)
                ᰀᰧᰌ parse_parts_value_with_tenths_place(p, "‰");

        ᰀᰧᰌ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ᰀᰧ (v > 10000)
                ᰀᰧᰌ -ERANGE;

        ᰀᰧᰌ v;
}
