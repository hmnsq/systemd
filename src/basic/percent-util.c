/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "parse-util.h"
#ضمن "percent-util.h"
#ضمن "string-util.h"

ساكن صحيح parse_parts_value_whole(ثابت حرف *p, ثابت حرف *symbol) {
        ثابت حرف *pc, *n;
        صحيح r, v;

        pc = endswith(p, symbol);
        ܐܢ (!pc)
                ܦܢܐ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (v < 0)
                ܦܢܐ -ERANGE;

        ܦܢܐ v;
}

ساكن صحيح parse_parts_value_with_tenths_place(ثابت حرف *p, ثابت حرف *symbol) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, symbol);
        ܐܢ (!pc)
                ܦܢܐ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ܐܢ (dot) {
                ܐܢ (dot + 2 != pc)
                        ܦܢܐ -EINVAL;
                ܐܢ (dot[1] < '0' || dot[1] > '9')
                        ܦܢܐ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ܐܚܪܢܐ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (v < 0)
                ܦܢܐ -ERANGE;
        ܐܢ (v > (INT_MAX - q) / 10)
                ܦܢܐ -ERANGE;

        v = v * 10 + q;
        ܦܢܐ v;
}

ساكن صحيح parse_parts_value_with_hundredths_place(ثابت حرف *p, ثابت حرف *symbol) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, symbol);
        ܐܢ (!pc)
                ܦܢܐ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ܐܢ (dot) {
                ܐܢ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ܐܢ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ܦܢܐ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ܐܚܪܢܐ ܐܢ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ܐܢ (dot[1] < '0' || dot[1] > '9')
                                ܦܢܐ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ܐܚܪܢܐ
                        /* We do not support zero or more than two places */
                        ܦܢܐ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ܐܚܪܢܐ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (v < 0)
                ܦܢܐ -ERANGE;
        ܐܢ (v > (INT_MAX - q) / 100)
                ܦܢܐ -ERANGE;

        v = v * 100 + q;
        ܦܢܐ v;
}

صحيح parse_percent_unbounded(ثابت حرف *p) {
        ܦܢܐ parse_parts_value_whole(p, "%");
}

صحيح parse_percent(ثابت حرف *p) {
        صحيح v;

        v = parse_percent_unbounded(p);
        ܐܢ (v > 100)
                ܦܢܐ -ERANGE;

        ܦܢܐ v;
}

صحيح parse_permille_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‰");
        ܐܢ (pm)
                ܦܢܐ parse_parts_value_whole(p, "‰");

        ܦܢܐ parse_parts_value_with_tenths_place(p, "%");
}

صحيح parse_permille(ثابت حرف *p) {
        صحيح v;

        v = parse_permille_unbounded(p);
        ܐܢ (v > 1000)
                ܦܢܐ -ERANGE;

        ܦܢܐ v;
}

صحيح parse_permyriad_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‱");
        ܐܢ (pm)
                ܦܢܐ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ܐܢ (pm)
                ܦܢܐ parse_parts_value_with_tenths_place(p, "‰");

        ܦܢܐ parse_parts_value_with_hundredths_place(p, "%");
}

صحيح parse_permyriad(ثابت حرف *p) {
        صحيح v;

        v = parse_permyriad_unbounded(p);
        ܐܢ (v > 10000)
                ܦܢܐ -ERANGE;

        ܦܢܐ v;
}
