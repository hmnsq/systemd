/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static 정수 parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        정수 r, v;

        pc = endswith(p, symbol);
        만약 (!pc)
                반환 -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        만약 (r < 0)
                반환 r;
        만약 (v < 0)
                반환 -ERANGE;

        반환 v;
}

static 정수 parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        정수 r, q, v;

        pc = endswith(p, symbol);
        만약 (!pc)
                반환 -EINVAL;

        dot = memchr(p, '.', pc - p);
        만약 (dot) {
                만약 (dot + 2 != pc)
                        반환 -EINVAL;
                만약 (dot[1] < '0' || dot[1] > '9')
                        반환 -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } 아니면 {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        만약 (r < 0)
                반환 r;
        만약 (v < 0)
                반환 -ERANGE;
        만약 (v > (INT_MAX - q) / 10)
                반환 -ERANGE;

        v = v * 10 + q;
        반환 v;
}

static 정수 parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        정수 r, q, v;

        pc = endswith(p, symbol);
        만약 (!pc)
                반환 -EINVAL;

        dot = memchr(p, '.', pc - p);
        만약 (dot) {
                만약 (dot + 3 == pc) {
                        /* Support two places after the dot */

                        만약 (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                반환 -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } 아니면 만약 (dot + 2 == pc) {
                        /* Support one place after the dot */

                        만약 (dot[1] < '0' || dot[1] > '9')
                                반환 -EINVAL;
                        q = (dot[1] - '0') * 10;
                } 아니면
                        /* We do not support zero or more than two places */
                        반환 -EINVAL;

                n = strndupa_safe(p, dot - p);
        } 아니면 {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        만약 (r < 0)
                반환 r;
        만약 (v < 0)
                반환 -ERANGE;
        만약 (v > (INT_MAX - q) / 100)
                반환 -ERANGE;

        v = v * 100 + q;
        반환 v;
}

정수 parse_percent_unbounded(const char *p) {
        반환 parse_parts_value_whole(p, "%");
}

정수 parse_percent(const char *p) {
        정수 v;

        v = parse_percent_unbounded(p);
        만약 (v > 100)
                반환 -ERANGE;

        반환 v;
}

정수 parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        만약 (pm)
                반환 parse_parts_value_whole(p, "‰");

        반환 parse_parts_value_with_tenths_place(p, "%");
}

정수 parse_permille(const char *p) {
        정수 v;

        v = parse_permille_unbounded(p);
        만약 (v > 1000)
                반환 -ERANGE;

        반환 v;
}

정수 parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        만약 (pm)
                반환 parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        만약 (pm)
                반환 parse_parts_value_with_tenths_place(p, "‰");

        반환 parse_parts_value_with_hundredths_place(p, "%");
}

정수 parse_permyriad(const char *p) {
        정수 v;

        v = parse_permyriad_unbounded(p);
        만약 (v > 10000)
                반환 -ERANGE;

        반환 v;
}
