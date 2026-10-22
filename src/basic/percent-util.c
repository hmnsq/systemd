/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static จำนวนเต็ม parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        จำนวนเต็ม r, v;

        pc = endswith(p, symbol);
        ถ้า (!pc)
                คืน -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ถ้า (r < 0)
                คืน r;
        ถ้า (v < 0)
                คืน -ERANGE;

        คืน v;
}

static จำนวนเต็ม parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        จำนวนเต็ม r, q, v;

        pc = endswith(p, symbol);
        ถ้า (!pc)
                คืน -EINVAL;

        dot = memchr(p, '.', pc - p);
        ถ้า (dot) {
                ถ้า (dot + 2 != pc)
                        คืน -EINVAL;
                ถ้า (dot[1] < '0' || dot[1] > '9')
                        คืน -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } มิฉะนั้น {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ถ้า (r < 0)
                คืน r;
        ถ้า (v < 0)
                คืน -ERANGE;
        ถ้า (v > (INT_MAX - q) / 10)
                คืน -ERANGE;

        v = v * 10 + q;
        คืน v;
}

static จำนวนเต็ม parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        จำนวนเต็ม r, q, v;

        pc = endswith(p, symbol);
        ถ้า (!pc)
                คืน -EINVAL;

        dot = memchr(p, '.', pc - p);
        ถ้า (dot) {
                ถ้า (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ถ้า (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                คืน -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } มิฉะนั้น ถ้า (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ถ้า (dot[1] < '0' || dot[1] > '9')
                                คืน -EINVAL;
                        q = (dot[1] - '0') * 10;
                } มิฉะนั้น
                        /* We do not support zero or more than two places */
                        คืน -EINVAL;

                n = strndupa_safe(p, dot - p);
        } มิฉะนั้น {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ถ้า (r < 0)
                คืน r;
        ถ้า (v < 0)
                คืน -ERANGE;
        ถ้า (v > (INT_MAX - q) / 100)
                คืน -ERANGE;

        v = v * 100 + q;
        คืน v;
}

จำนวนเต็ม parse_percent_unbounded(const var_na *p) {
        คืน parse_parts_value_whole(p, "%");
}

จำนวนเต็ม parse_percent(const var_na *p) {
        จำนวนเต็ม v;

        v = parse_percent_unbounded(p);
        ถ้า (v > 100)
                คืน -ERANGE;

        คืน v;
}

จำนวนเต็ม parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ถ้า (pm)
                คืน parse_parts_value_whole(p, "‰");

        คืน parse_parts_value_with_tenths_place(p, "%");
}

จำนวนเต็ม parse_permille(const var_na *p) {
        จำนวนเต็ม v;

        v = parse_permille_unbounded(p);
        ถ้า (v > 1000)
                คืน -ERANGE;

        คืน v;
}

จำนวนเต็ม parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ถ้า (pm)
                คืน parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ถ้า (pm)
                คืน parse_parts_value_with_tenths_place(p, "‰");

        คืน parse_parts_value_with_hundredths_place(p, "%");
}

จำนวนเต็ม parse_permyriad(const var_na *p) {
        จำนวนเต็ม v;

        v = parse_permyriad_unbounded(p);
        ถ้า (v > 10000)
                คืน -ERANGE;

        คืน v;
}
