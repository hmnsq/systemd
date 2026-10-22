/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static ཧྲིལ parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        ཧྲིལ r, v;

        pc = endswith(p, symbol);
        གལ་ཏེ (!pc)
                ཕྱིར -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (v < 0)
                ཕྱིར -ERANGE;

        ཕྱིར v;
}

static ཧྲིལ parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ཧྲིལ r, q, v;

        pc = endswith(p, symbol);
        གལ་ཏེ (!pc)
                ཕྱིར -EINVAL;

        dot = memchr(p, '.', pc - p);
        གལ་ཏེ (dot) {
                གལ་ཏེ (dot + 2 != pc)
                        ཕྱིར -EINVAL;
                གལ་ཏེ (dot[1] < '0' || dot[1] > '9')
                        ཕྱིར -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } དེ་མིན {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (v < 0)
                ཕྱིར -ERANGE;
        གལ་ཏེ (v > (INT_MAX - q) / 10)
                ཕྱིར -ERANGE;

        v = v * 10 + q;
        ཕྱིར v;
}

static ཧྲིལ parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ཧྲིལ r, q, v;

        pc = endswith(p, symbol);
        གལ་ཏེ (!pc)
                ཕྱིར -EINVAL;

        dot = memchr(p, '.', pc - p);
        གལ་ཏེ (dot) {
                གལ་ཏེ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        གལ་ཏེ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ཕྱིར -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } དེ་མིན གལ་ཏེ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        གལ་ཏེ (dot[1] < '0' || dot[1] > '9')
                                ཕྱིར -EINVAL;
                        q = (dot[1] - '0') * 10;
                } དེ་མིན
                        /* We do not support zero or more than two places */
                        ཕྱིར -EINVAL;

                n = strndupa_safe(p, dot - p);
        } དེ་མིན {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (v < 0)
                ཕྱིར -ERANGE;
        གལ་ཏེ (v > (INT_MAX - q) / 100)
                ཕྱིར -ERANGE;

        v = v * 100 + q;
        ཕྱིར v;
}

ཧྲིལ parse_percent_unbounded(const var_na *p) {
        ཕྱིར parse_parts_value_whole(p, "%");
}

ཧྲིལ parse_percent(const var_na *p) {
        ཧྲིལ v;

        v = parse_percent_unbounded(p);
        གལ་ཏེ (v > 100)
                ཕྱིར -ERANGE;

        ཕྱིར v;
}

ཧྲིལ parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        གལ་ཏེ (pm)
                ཕྱིར parse_parts_value_whole(p, "‰");

        ཕྱིར parse_parts_value_with_tenths_place(p, "%");
}

ཧྲིལ parse_permille(const var_na *p) {
        ཧྲིལ v;

        v = parse_permille_unbounded(p);
        གལ་ཏེ (v > 1000)
                ཕྱིར -ERANGE;

        ཕྱིར v;
}

ཧྲིལ parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        གལ་ཏེ (pm)
                ཕྱིར parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        གལ་ཏེ (pm)
                ཕྱིར parse_parts_value_with_tenths_place(p, "‰");

        ཕྱིར parse_parts_value_with_hundredths_place(p, "%");
}

ཧྲིལ parse_permyriad(const var_na *p) {
        ཧྲིལ v;

        v = parse_permyriad_unbounded(p);
        གལ་ཏེ (v > 10000)
                ཕྱིར -ERANGE;

        ཕྱིར v;
}
