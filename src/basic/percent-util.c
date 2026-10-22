/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static цео parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        цео r, v;

        pc = endswith(p, symbol);
        ако (!pc)
                врати -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ако (r < 0)
                врати r;
        ако (v < 0)
                врати -ERANGE;

        врати v;
}

static цео parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        цео r, q, v;

        pc = endswith(p, symbol);
        ако (!pc)
                врати -EINVAL;

        dot = memchr(p, '.', pc - p);
        ако (dot) {
                ако (dot + 2 != pc)
                        врати -EINVAL;
                ако (dot[1] < '0' || dot[1] > '9')
                        врати -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } инако {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ако (r < 0)
                врати r;
        ако (v < 0)
                врати -ERANGE;
        ако (v > (INT_MAX - q) / 10)
                врати -ERANGE;

        v = v * 10 + q;
        врати v;
}

static цео parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        цео r, q, v;

        pc = endswith(p, symbol);
        ако (!pc)
                врати -EINVAL;

        dot = memchr(p, '.', pc - p);
        ако (dot) {
                ако (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ако (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                врати -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } инако ако (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ако (dot[1] < '0' || dot[1] > '9')
                                врати -EINVAL;
                        q = (dot[1] - '0') * 10;
                } инако
                        /* We do not support zero or more than two places */
                        врати -EINVAL;

                n = strndupa_safe(p, dot - p);
        } инако {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ако (r < 0)
                врати r;
        ако (v < 0)
                врати -ERANGE;
        ако (v > (INT_MAX - q) / 100)
                врати -ERANGE;

        v = v * 100 + q;
        врати v;
}

цео parse_percent_unbounded(const char *p) {
        врати parse_parts_value_whole(p, "%");
}

цео parse_percent(const char *p) {
        цео v;

        v = parse_percent_unbounded(p);
        ако (v > 100)
                врати -ERANGE;

        врати v;
}

цео parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        ако (pm)
                врати parse_parts_value_whole(p, "‰");

        врати parse_parts_value_with_tenths_place(p, "%");
}

цео parse_permille(const char *p) {
        цео v;

        v = parse_permille_unbounded(p);
        ако (v > 1000)
                врати -ERANGE;

        врати v;
}

цео parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        ако (pm)
                врати parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ако (pm)
                врати parse_parts_value_with_tenths_place(p, "‰");

        врати parse_parts_value_with_hundredths_place(p, "%");
}

цео parse_permyriad(const char *p) {
        цео v;

        v = parse_permyriad_unbounded(p);
        ако (v > 10000)
                врати -ERANGE;

        врати v;
}
