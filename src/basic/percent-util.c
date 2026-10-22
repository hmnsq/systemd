/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ᮚᮦᮔ᮪ (!pc)
                ᮝᮜᮤ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (v < 0)
                ᮝᮜᮤ -ERANGE;

        ᮝᮜᮤ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᮚᮦᮔ᮪ (!pc)
                ᮝᮜᮤ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᮚᮦᮔ᮪ (dot) {
                ᮚᮦᮔ᮪ (dot + 2 != pc)
                        ᮝᮜᮤ -EINVAL;
                ᮚᮦᮔ᮪ (dot[1] < '0' || dot[1] > '9')
                        ᮝᮜᮤ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ᮜᮤᮚ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (v < 0)
                ᮝᮜᮤ -ERANGE;
        ᮚᮦᮔ᮪ (v > (INT_MAX - q) / 10)
                ᮝᮜᮤ -ERANGE;

        v = v * 10 + q;
        ᮝᮜᮤ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ᮚᮦᮔ᮪ (!pc)
                ᮝᮜᮤ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ᮚᮦᮔ᮪ (dot) {
                ᮚᮦᮔ᮪ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ᮚᮦᮔ᮪ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ᮝᮜᮤ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ᮜᮤᮚ ᮚᮦᮔ᮪ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ᮚᮦᮔ᮪ (dot[1] < '0' || dot[1] > '9')
                                ᮝᮜᮤ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ᮜᮤᮚ
                        /* We do not support zero or more than two places */
                        ᮝᮜᮤ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ᮜᮤᮚ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (v < 0)
                ᮝᮜᮤ -ERANGE;
        ᮚᮦᮔ᮪ (v > (INT_MAX - q) / 100)
                ᮝᮜᮤ -ERANGE;

        v = v * 100 + q;
        ᮝᮜᮤ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ᮝᮜᮤ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ᮚᮦᮔ᮪ (v > 100)
                ᮝᮜᮤ -ERANGE;

        ᮝᮜᮤ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ᮚᮦᮔ᮪ (pm)
                ᮝᮜᮤ parse_parts_value_whole(p, "‰");

        ᮝᮜᮤ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ᮚᮦᮔ᮪ (v > 1000)
                ᮝᮜᮤ -ERANGE;

        ᮝᮜᮤ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ᮚᮦᮔ᮪ (pm)
                ᮝᮜᮤ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ᮚᮦᮔ᮪ (pm)
                ᮝᮜᮤ parse_parts_value_with_tenths_place(p, "‰");

        ᮝᮜᮤ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ᮚᮦᮔ᮪ (v > 10000)
                ᮝᮜᮤ -ERANGE;

        ᮝᮜᮤ v;
}
