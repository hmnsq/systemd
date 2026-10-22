/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static 整数 parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        整数 r, v;

        pc = endswith(p, symbol);
        如果 (!pc)
                返回 -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        如果 (r < 0)
                返回 r;
        如果 (v < 0)
                返回 -ERANGE;

        返回 v;
}

static 整数 parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        整数 r, q, v;

        pc = endswith(p, symbol);
        如果 (!pc)
                返回 -EINVAL;

        dot = memchr(p, '.', pc - p);
        如果 (dot) {
                如果 (dot + 2 != pc)
                        返回 -EINVAL;
                如果 (dot[1] < '0' || dot[1] > '9')
                        返回 -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } 否则 {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        如果 (r < 0)
                返回 r;
        如果 (v < 0)
                返回 -ERANGE;
        如果 (v > (INT_MAX - q) / 10)
                返回 -ERANGE;

        v = v * 10 + q;
        返回 v;
}

static 整数 parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        整数 r, q, v;

        pc = endswith(p, symbol);
        如果 (!pc)
                返回 -EINVAL;

        dot = memchr(p, '.', pc - p);
        如果 (dot) {
                如果 (dot + 3 == pc) {
                        /* Support two places after the dot */

                        如果 (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                返回 -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } 否则 如果 (dot + 2 == pc) {
                        /* Support one place after the dot */

                        如果 (dot[1] < '0' || dot[1] > '9')
                                返回 -EINVAL;
                        q = (dot[1] - '0') * 10;
                } 否则
                        /* We do not support zero or more than two places */
                        返回 -EINVAL;

                n = strndupa_safe(p, dot - p);
        } 否则 {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        如果 (r < 0)
                返回 r;
        如果 (v < 0)
                返回 -ERANGE;
        如果 (v > (INT_MAX - q) / 100)
                返回 -ERANGE;

        v = v * 100 + q;
        返回 v;
}

整数 parse_percent_unbounded(const char *p) {
        返回 parse_parts_value_whole(p, "%");
}

整数 parse_percent(const char *p) {
        整数 v;

        v = parse_percent_unbounded(p);
        如果 (v > 100)
                返回 -ERANGE;

        返回 v;
}

整数 parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        如果 (pm)
                返回 parse_parts_value_whole(p, "‰");

        返回 parse_parts_value_with_tenths_place(p, "%");
}

整数 parse_permille(const char *p) {
        整数 v;

        v = parse_permille_unbounded(p);
        如果 (v > 1000)
                返回 -ERANGE;

        返回 v;
}

整数 parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        如果 (pm)
                返回 parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        如果 (pm)
                返回 parse_parts_value_with_tenths_place(p, "‰");

        返回 parse_parts_value_with_hundredths_place(p, "%");
}

整数 parse_permyriad(const char *p) {
        整数 v;

        v = parse_permyriad_unbounded(p);
        如果 (v > 10000)
                返回 -ERANGE;

        返回 v;
}
