/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ᬬᬾᬦ᭄ (!pc)
                ᬯᬮᬶ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (v < 0)
                ᬯᬮᬶ -ERANGE;

        ᬯᬮᬶ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᬬᬾᬦ᭄ (!pc)
                ᬯᬮᬶ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᬬᬾᬦ᭄ (dot) {
                ᬬᬾᬦ᭄ (dot + 2 != pc)
                        ᬯᬮᬶ -EINVAL;
                ᬬᬾᬦ᭄ (dot[1] < '0' || dot[1] > '9')
                        ᬯᬮᬶ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ᬮᬶᬬ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (v < 0)
                ᬯᬮᬶ -ERANGE;
        ᬬᬾᬦ᭄ (v > (INT_MAX - q) / 10)
                ᬯᬮᬶ -ERANGE;

        v = v * 10 + q;
        ᬯᬮᬶ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᬬᬾᬦ᭄ (!pc)
                ᬯᬮᬶ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᬬᬾᬦ᭄ (dot) {
                ᬬᬾᬦ᭄ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ᬬᬾᬦ᭄ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ᬯᬮᬶ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ᬮᬶᬬ ᬬᬾᬦ᭄ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ᬬᬾᬦ᭄ (dot[1] < '0' || dot[1] > '9')
                                ᬯᬮᬶ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ᬮᬶᬬ
                        /* We do not support zero or more than two places */
                        ᬯᬮᬶ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ᬮᬶᬬ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (v < 0)
                ᬯᬮᬶ -ERANGE;
        ᬬᬾᬦ᭄ (v > (INT_MAX - q) / 100)
                ᬯᬮᬶ -ERANGE;

        v = v * 100 + q;
        ᬯᬮᬶ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ᬯᬮᬶ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ᬬᬾᬦ᭄ (v > 100)
                ᬯᬮᬶ -ERANGE;

        ᬯᬮᬶ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ᬬᬾᬦ᭄ (pm)
                ᬯᬮᬶ parse_parts_value_whole(p, "‰");

        ᬯᬮᬶ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ᬬᬾᬦ᭄ (v > 1000)
                ᬯᬮᬶ -ERANGE;

        ᬯᬮᬶ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ᬬᬾᬦ᭄ (pm)
                ᬯᬮᬶ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ᬬᬾᬦ᭄ (pm)
                ᬯᬮᬶ parse_parts_value_with_tenths_place(p, "‰");

        ᬯᬮᬶ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ᬬᬾᬦ᭄ (v > 10000)
                ᬯᬮᬶ -ERANGE;

        ᬯᬮᬶ v;
}
