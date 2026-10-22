/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static ціле parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        ціле r, v;

        pc = endswith(p, symbol);
        якщо (!pc)
                повернути -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        якщо (r < 0)
                повернути r;
        якщо (v < 0)
                повернути -ERANGE;

        повернути v;
}

static ціле parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        ціле r, q, v;

        pc = endswith(p, symbol);
        якщо (!pc)
                повернути -EINVAL;

        dot = memchr(p, '.', pc - p);
        якщо (dot) {
                якщо (dot + 2 != pc)
                        повернути -EINVAL;
                якщо (dot[1] < '0' || dot[1] > '9')
                        повернути -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } інакше {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        якщо (r < 0)
                повернути r;
        якщо (v < 0)
                повернути -ERANGE;
        якщо (v > (INT_MAX - q) / 10)
                повернути -ERANGE;

        v = v * 10 + q;
        повернути v;
}

static ціле parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        ціле r, q, v;

        pc = endswith(p, symbol);
        якщо (!pc)
                повернути -EINVAL;

        dot = memchr(p, '.', pc - p);
        якщо (dot) {
                якщо (dot + 3 == pc) {
                        /* Support two places after the dot */

                        якщо (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                повернути -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } інакше якщо (dot + 2 == pc) {
                        /* Support one place after the dot */

                        якщо (dot[1] < '0' || dot[1] > '9')
                                повернути -EINVAL;
                        q = (dot[1] - '0') * 10;
                } інакше
                        /* We do not support zero or more than two places */
                        повернути -EINVAL;

                n = strndupa_safe(p, dot - p);
        } інакше {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        якщо (r < 0)
                повернути r;
        якщо (v < 0)
                повернути -ERANGE;
        якщо (v > (INT_MAX - q) / 100)
                повернути -ERANGE;

        v = v * 100 + q;
        повернути v;
}

ціле parse_percent_unbounded(const char *p) {
        повернути parse_parts_value_whole(p, "%");
}

ціле parse_percent(const char *p) {
        ціле v;

        v = parse_percent_unbounded(p);
        якщо (v > 100)
                повернути -ERANGE;

        повернути v;
}

ціле parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        якщо (pm)
                повернути parse_parts_value_whole(p, "‰");

        повернути parse_parts_value_with_tenths_place(p, "%");
}

ціле parse_permille(const char *p) {
        ціле v;

        v = parse_permille_unbounded(p);
        якщо (v > 1000)
                повернути -ERANGE;

        повернути v;
}

ціле parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        якщо (pm)
                повернути parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        якщо (pm)
                повернути parse_parts_value_with_tenths_place(p, "‰");

        повернути parse_parts_value_with_hundredths_place(p, "%");
}

ціле parse_permyriad(const char *p) {
        ціле v;

        v = parse_permyriad_unbounded(p);
        якщо (v > 10000)
                повернути -ERANGE;

        повернути v;
}
