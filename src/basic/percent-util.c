/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "parse-util.h"
#شامل "percent-util.h"
#شامل "string-util.h"

ایستا صحیح parse_parts_مقدار_whole(ثابت نویسه *p, ثابت نویسه *نماد) {
        ثابت نویسه *pc, *n;
        صحیح r, v;

        pc = endswith(p, نماد);
        اگر (!pc)
                بازگشت -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        اگر (r < 0)
                بازگشت r;
        اگر (v < 0)
                بازگشت -ERANGE;

        بازگشت v;
}

ایستا صحیح parse_parts_مقدار_with_tenths_place(ثابت نویسه *p, ثابت نویسه *نماد) {
        ثابت نویسه *pc, *dot, *n;
        صحیح r, q, v;

        pc = endswith(p, نماد);
        اگر (!pc)
                بازگشت -EINVAL;

        dot = memchr(p, '.', pc - p);
        اگر (dot) {
                اگر (dot + 2 != pc)
                        بازگشت -EINVAL;
                اگر (dot[1] < '0' || dot[1] > '9')
                        بازگشت -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } وگرنه {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        اگر (r < 0)
                بازگشت r;
        اگر (v < 0)
                بازگشت -ERANGE;
        اگر (v > (INT_MAX - q) / 10)
                بازگشت -ERANGE;

        v = v * 10 + q;
        بازگشت v;
}

ایستا صحیح parse_parts_مقدار_with_hundredths_place(ثابت نویسه *p, ثابت نویسه *نماد) {
        ثابت نویسه *pc, *dot, *n;
        صحیح r, q, v;

        pc = endswith(p, نماد);
        اگر (!pc)
                بازگشت -EINVAL;

        dot = memchr(p, '.', pc - p);
        اگر (dot) {
                اگر (dot + 3 == pc) {
                        /* Support two places after the dot */

                        اگر (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                بازگشت -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } وگرنه اگر (dot + 2 == pc) {
                        /* Support one place after the dot */

                        اگر (dot[1] < '0' || dot[1] > '9')
                                بازگشت -EINVAL;
                        q = (dot[1] - '0') * 10;
                } وگرنه
                        /* We do not support zero or more than two places */
                        بازگشت -EINVAL;

                n = strndupa_safe(p, dot - p);
        } وگرنه {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        اگر (r < 0)
                بازگشت r;
        اگر (v < 0)
                بازگشت -ERANGE;
        اگر (v > (INT_MAX - q) / 100)
                بازگشت -ERANGE;

        v = v * 100 + q;
        بازگشت v;
}

صحیح parse_percent_unbounded(ثابت نویسه *p) {
        بازگشت parse_parts_مقدار_whole(p, "%");
}

صحیح parse_percent(ثابت نویسه *p) {
        صحیح v;

        v = parse_percent_unbounded(p);
        اگر (v > 100)
                بازگشت -ERANGE;

        بازگشت v;
}

صحیح parse_permille_unbounded(ثابت نویسه *p) {
        ثابت نویسه *pm;

        pm = endswith(p, "‰");
        اگر (pm)
                بازگشت parse_parts_مقدار_whole(p, "‰");

        بازگشت parse_parts_مقدار_with_tenths_place(p, "%");
}

صحیح parse_permille(ثابت نویسه *p) {
        صحیح v;

        v = parse_permille_unbounded(p);
        اگر (v > 1000)
                بازگشت -ERANGE;

        بازگشت v;
}

صحیح parse_permyriad_unbounded(ثابت نویسه *p) {
        ثابت نویسه *pm;

        pm = endswith(p, "‱");
        اگر (pm)
                بازگشت parse_parts_مقدار_whole(p, "‱");

        pm = endswith(p, "‰");
        اگر (pm)
                بازگشت parse_parts_مقدار_with_tenths_place(p, "‰");

        بازگشت parse_parts_مقدار_with_hundredths_place(p, "%");
}

صحیح parse_permyriad(ثابت نویسه *p) {
        صحیح v;

        v = parse_permyriad_unbounded(p);
        اگر (v > 10000)
                بازگشت -ERANGE;

        بازگشت v;
}
