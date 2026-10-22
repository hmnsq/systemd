/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "parse-util.h"
#ضمن "percent-util.h"
#ضمن "string-util.h"

ساكن صحيح parse_parts_value_whole(ثابت حرف *p, ثابت حرف *symbol) {
        ثابت حرف *pc, *n;
        صحيح r, v;

        pc = endswith(p, symbol);
        אם (!pc)
                החזר -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        אם (r < 0)
                החזר r;
        אם (v < 0)
                החזר -ERANGE;

        החזר v;
}

ساكن صحيح parse_parts_value_with_tenths_place(ثابت حرف *p, ثابت حرف *symbol) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, symbol);
        אם (!pc)
                החזר -EINVAL;

        dot = memchr(p, '.', pc - p);
        אם (dot) {
                אם (dot + 2 != pc)
                        החזר -EINVAL;
                אם (dot[1] < '0' || dot[1] > '9')
                        החזר -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } אחרת {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        אם (r < 0)
                החזר r;
        אם (v < 0)
                החזר -ERANGE;
        אם (v > (INT_MAX - q) / 10)
                החזר -ERANGE;

        v = v * 10 + q;
        החזר v;
}

ساكن صحيح parse_parts_value_with_hundredths_place(ثابت حرف *p, ثابت حرف *symbol) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, symbol);
        אם (!pc)
                החזר -EINVAL;

        dot = memchr(p, '.', pc - p);
        אם (dot) {
                אם (dot + 3 == pc) {
                        /* Support two places after the dot */

                        אם (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                החזר -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } אחרת אם (dot + 2 == pc) {
                        /* Support one place after the dot */

                        אם (dot[1] < '0' || dot[1] > '9')
                                החזר -EINVAL;
                        q = (dot[1] - '0') * 10;
                } אחרת
                        /* We do not support zero or more than two places */
                        החזר -EINVAL;

                n = strndupa_safe(p, dot - p);
        } אחרת {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        אם (r < 0)
                החזר r;
        אם (v < 0)
                החזר -ERANGE;
        אם (v > (INT_MAX - q) / 100)
                החזר -ERANGE;

        v = v * 100 + q;
        החזר v;
}

صحيح parse_percent_unbounded(ثابت حرف *p) {
        החזר parse_parts_value_whole(p, "%");
}

صحيح parse_percent(ثابت حرف *p) {
        صحيح v;

        v = parse_percent_unbounded(p);
        אם (v > 100)
                החזר -ERANGE;

        החזר v;
}

صحيح parse_permille_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‰");
        אם (pm)
                החזר parse_parts_value_whole(p, "‰");

        החזר parse_parts_value_with_tenths_place(p, "%");
}

صحيح parse_permille(ثابت حرف *p) {
        صحيح v;

        v = parse_permille_unbounded(p);
        אם (v > 1000)
                החזר -ERANGE;

        החזר v;
}

صحيح parse_permyriad_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‱");
        אם (pm)
                החזר parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        אם (pm)
                החזר parse_parts_value_with_tenths_place(p, "‰");

        החזר parse_parts_value_with_hundredths_place(p, "%");
}

صحيح parse_permyriad(ثابت حرف *p) {
        صحيح v;

        v = parse_permyriad_unbounded(p);
        אם (v > 10000)
                החזר -ERANGE;

        החזר v;
}
