/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static цяло parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        цяло r, v;

        pc = endswith(p, symbol);
        ако (!pc)
                върни -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ако (r < 0)
                върни r;
        ако (v < 0)
                върни -ERANGE;

        върни v;
}

static цяло parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        цяло r, q, v;

        pc = endswith(p, symbol);
        ако (!pc)
                върни -EINVAL;

        dot = memchr(p, '.', pc - p);
        ако (dot) {
                ако (dot + 2 != pc)
                        върни -EINVAL;
                ако (dot[1] < '0' || dot[1] > '9')
                        върни -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } иначе {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ако (r < 0)
                върни r;
        ако (v < 0)
                върни -ERANGE;
        ако (v > (INT_MAX - q) / 10)
                върни -ERANGE;

        v = v * 10 + q;
        върни v;
}

static цяло parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        цяло r, q, v;

        pc = endswith(p, symbol);
        ако (!pc)
                върни -EINVAL;

        dot = memchr(p, '.', pc - p);
        ако (dot) {
                ако (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ако (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                върни -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } иначе ако (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ако (dot[1] < '0' || dot[1] > '9')
                                върни -EINVAL;
                        q = (dot[1] - '0') * 10;
                } иначе
                        /* We do not support zero or more than two places */
                        върни -EINVAL;

                n = strndupa_safe(p, dot - p);
        } иначе {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ако (r < 0)
                върни r;
        ако (v < 0)
                върни -ERANGE;
        ако (v > (INT_MAX - q) / 100)
                върни -ERANGE;

        v = v * 100 + q;
        върни v;
}

цяло parse_percent_unbounded(const char *p) {
        върни parse_parts_value_whole(p, "%");
}

цяло parse_percent(const char *p) {
        цяло v;

        v = parse_percent_unbounded(p);
        ако (v > 100)
                върни -ERANGE;

        върни v;
}

цяло parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        ако (pm)
                върни parse_parts_value_whole(p, "‰");

        върни parse_parts_value_with_tenths_place(p, "%");
}

цяло parse_permille(const char *p) {
        цяло v;

        v = parse_permille_unbounded(p);
        ако (v > 1000)
                върни -ERANGE;

        върни v;
}

цяло parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        ако (pm)
                върни parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ако (pm)
                върни parse_parts_value_with_tenths_place(p, "‰");

        върни parse_parts_value_with_hundredths_place(p, "%");
}

цяло parse_permyriad(const char *p) {
        цяло v;

        v = parse_permyriad_unbounded(p);
        ако (v > 10000)
                върни -ERANGE;

        върни v;
}
