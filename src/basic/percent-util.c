/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static ཧྲིལ parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        ཧྲིལ r, v;

        pc = endswith(p, symbol);
        གལ་སྲིད (!pc)
                ལོག -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (v < 0)
                ལོག -ERANGE;

        ལོག v;
}

static ཧྲིལ parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ཧྲིལ r, q, v;

        pc = endswith(p, symbol);
        གལ་སྲིད (!pc)
                ལོག -EINVAL;

        dot = memchr(p, '.', pc - p);
        གལ་སྲིད (dot) {
                གལ་སྲིད (dot + 2 != pc)
                        ལོག -EINVAL;
                གལ་སྲིད (dot[1] < '0' || dot[1] > '9')
                        ལོག -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } དེ་མིན {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (v < 0)
                ལོག -ERANGE;
        གལ་སྲིད (v > (INT_MAX - q) / 10)
                ལོག -ERANGE;

        v = v * 10 + q;
        ལོག v;
}

static ཧྲིལ parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ཧྲིལ r, q, v;

        pc = endswith(p, symbol);
        གལ་སྲིད (!pc)
                ལོག -EINVAL;

        dot = memchr(p, '.', pc - p);
        གལ་སྲིད (dot) {
                གལ་སྲིད (dot + 3 == pc) {
                        /* Support two places after the dot */

                        གལ་སྲིད (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ལོག -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } དེ་མིན གལ་སྲིད (dot + 2 == pc) {
                        /* Support one place after the dot */

                        གལ་སྲིད (dot[1] < '0' || dot[1] > '9')
                                ལོག -EINVAL;
                        q = (dot[1] - '0') * 10;
                } དེ་མིན
                        /* We do not support zero or more than two places */
                        ལོག -EINVAL;

                n = strndupa_safe(p, dot - p);
        } དེ་མིན {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (v < 0)
                ལོག -ERANGE;
        གལ་སྲིད (v > (INT_MAX - q) / 100)
                ལོག -ERANGE;

        v = v * 100 + q;
        ལོག v;
}

ཧྲིལ parse_percent_unbounded(const var_na *p) {
        ལོག parse_parts_value_whole(p, "%");
}

ཧྲིལ parse_percent(const var_na *p) {
        ཧྲིལ v;

        v = parse_percent_unbounded(p);
        གལ་སྲིད (v > 100)
                ལོག -ERANGE;

        ལོག v;
}

ཧྲིལ parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        གལ་སྲིད (pm)
                ལོག parse_parts_value_whole(p, "‰");

        ལོག parse_parts_value_with_tenths_place(p, "%");
}

ཧྲིལ parse_permille(const var_na *p) {
        ཧྲིལ v;

        v = parse_permille_unbounded(p);
        གལ་སྲིད (v > 1000)
                ལོག -ERANGE;

        ལོག v;
}

ཧྲིལ parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        གལ་སྲིད (pm)
                ལོག parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        གལ་སྲིད (pm)
                ལོག parse_parts_value_with_tenths_place(p, "‰");

        ལོག parse_parts_value_with_hundredths_place(p, "%");
}

ཧྲིལ parse_permyriad(const var_na *p) {
        ཧྲིལ v;

        v = parse_permyriad_unbounded(p);
        གལ་སྲིད (v > 10000)
                ལོག -ERANGE;

        ལོག v;
}
