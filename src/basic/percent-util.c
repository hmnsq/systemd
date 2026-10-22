/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static целое parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        целое r, v;

        pc = endswith(p, symbol);
        если (!pc)
                вернуть -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        если (r < 0)
                вернуть r;
        если (v < 0)
                вернуть -ERANGE;

        вернуть v;
}

static целое parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        целое r, q, v;

        pc = endswith(p, symbol);
        если (!pc)
                вернуть -EINVAL;

        dot = memchr(p, '.', pc - p);
        если (dot) {
                если (dot + 2 != pc)
                        вернуть -EINVAL;
                если (dot[1] < '0' || dot[1] > '9')
                        вернуть -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } иначе {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        если (r < 0)
                вернуть r;
        если (v < 0)
                вернуть -ERANGE;
        если (v > (INT_MAX - q) / 10)
                вернуть -ERANGE;

        v = v * 10 + q;
        вернуть v;
}

static целое parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        целое r, q, v;

        pc = endswith(p, symbol);
        если (!pc)
                вернуть -EINVAL;

        dot = memchr(p, '.', pc - p);
        если (dot) {
                если (dot + 3 == pc) {
                        /* Support two places after the dot */

                        если (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                вернуть -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } иначе если (dot + 2 == pc) {
                        /* Support one place after the dot */

                        если (dot[1] < '0' || dot[1] > '9')
                                вернуть -EINVAL;
                        q = (dot[1] - '0') * 10;
                } иначе
                        /* We do not support zero or more than two places */
                        вернуть -EINVAL;

                n = strndupa_safe(p, dot - p);
        } иначе {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        если (r < 0)
                вернуть r;
        если (v < 0)
                вернуть -ERANGE;
        если (v > (INT_MAX - q) / 100)
                вернуть -ERANGE;

        v = v * 100 + q;
        вернуть v;
}

целое parse_percent_unbounded(const char *p) {
        вернуть parse_parts_value_whole(p, "%");
}

целое parse_percent(const char *p) {
        целое v;

        v = parse_percent_unbounded(p);
        если (v > 100)
                вернуть -ERANGE;

        вернуть v;
}

целое parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        если (pm)
                вернуть parse_parts_value_whole(p, "‰");

        вернуть parse_parts_value_with_tenths_place(p, "%");
}

целое parse_permille(const char *p) {
        целое v;

        v = parse_permille_unbounded(p);
        если (v > 1000)
                вернуть -ERANGE;

        вернуть v;
}

целое parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        если (pm)
                вернуть parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        если (pm)
                вернуть parse_parts_value_with_tenths_place(p, "‰");

        вернуть parse_parts_value_with_hundredths_place(p, "%");
}

целое parse_permyriad(const char *p) {
        целое v;

        v = parse_permyriad_unbounded(p);
        если (v > 10000)
                вернуть -ERANGE;

        вернуть v;
}
