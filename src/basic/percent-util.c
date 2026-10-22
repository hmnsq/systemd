/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static 整数 parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        整数 r, v;

        pc = endswith(p, symbol);
        もし (!pc)
                戻る -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        もし (r < 0)
                戻る r;
        もし (v < 0)
                戻る -ERANGE;

        戻る v;
}

static 整数 parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        整数 r, q, v;

        pc = endswith(p, symbol);
        もし (!pc)
                戻る -EINVAL;

        dot = memchr(p, '.', pc - p);
        もし (dot) {
                もし (dot + 2 != pc)
                        戻る -EINVAL;
                もし (dot[1] < '0' || dot[1] > '9')
                        戻る -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } 否则 {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        もし (r < 0)
                戻る r;
        もし (v < 0)
                戻る -ERANGE;
        もし (v > (INT_MAX - q) / 10)
                戻る -ERANGE;

        v = v * 10 + q;
        戻る v;
}

static 整数 parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        整数 r, q, v;

        pc = endswith(p, symbol);
        もし (!pc)
                戻る -EINVAL;

        dot = memchr(p, '.', pc - p);
        もし (dot) {
                もし (dot + 3 == pc) {
                        /* Support two places after the dot */

                        もし (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                戻る -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } 否则 もし (dot + 2 == pc) {
                        /* Support one place after the dot */

                        もし (dot[1] < '0' || dot[1] > '9')
                                戻る -EINVAL;
                        q = (dot[1] - '0') * 10;
                } 否则
                        /* We do not support zero or more than two places */
                        戻る -EINVAL;

                n = strndupa_safe(p, dot - p);
        } 否则 {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        もし (r < 0)
                戻る r;
        もし (v < 0)
                戻る -ERANGE;
        もし (v > (INT_MAX - q) / 100)
                戻る -ERANGE;

        v = v * 100 + q;
        戻る v;
}

整数 parse_percent_unbounded(const char *p) {
        戻る parse_parts_value_whole(p, "%");
}

整数 parse_percent(const char *p) {
        整数 v;

        v = parse_percent_unbounded(p);
        もし (v > 100)
                戻る -ERANGE;

        戻る v;
}

整数 parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        もし (pm)
                戻る parse_parts_value_whole(p, "‰");

        戻る parse_parts_value_with_tenths_place(p, "%");
}

整数 parse_permille(const char *p) {
        整数 v;

        v = parse_permille_unbounded(p);
        もし (v > 1000)
                戻る -ERANGE;

        戻る v;
}

整数 parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        もし (pm)
                戻る parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        もし (pm)
                戻る parse_parts_value_with_tenths_place(p, "‰");

        戻る parse_parts_value_with_hundredths_place(p, "%");
}

整数 parse_permyriad(const char *p) {
        整数 v;

        v = parse_permyriad_unbounded(p);
        もし (v > 10000)
                戻る -ERANGE;

        戻る v;
}
