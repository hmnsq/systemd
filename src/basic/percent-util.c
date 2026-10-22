/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "parse-util.h"
#ضمن "percent-util.h"
#ضمن "string-util.h"

ساكن صحيح parse_parts_مقدار_کامل(ثابت حرف *p, ثابت حرف *نماد) {
        ثابت حرف *pc, *n;
        صحيح r, v;

        pc = endswith(p, نماد);
        إذا (!pc)
                أرجع -EINVAL;

        n = strndupa_امن(p, pc - p);
        r = امن_atoi(n, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v < 0)
                أرجع -ERANGE;

        أرجع v;
}

ساكن صحيح parse_parts_مقدار_with_tenths_قراربده(ثابت حرف *p, ثابت حرف *نماد) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, نماد);
        إذا (!pc)
                أرجع -EINVAL;

        dot = memchr(p, '.', pc - p);
        إذا (dot) {
                إذا (dot + 2 != pc)
                        أرجع -EINVAL;
                إذا (dot[1] < '0' || dot[1] > '9')
                        أرجع -EINVAL;
                q = dot[1] - '0';
                n = strndupa_امن(p, dot - p);
        } وإلا {
                q = 0;
                n = strndupa_امن(p, pc - p);
        }
        r = امن_atoi(n, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v < 0)
                أرجع -ERANGE;
        إذا (v > (بیشترین_صحیح - q) / 10)
                أرجع -ERANGE;

        v = v * 10 + q;
        أرجع v;
}

ساكن صحيح parse_parts_مقدار_with_hundredths_قراربده(ثابت حرف *p, ثابت حرف *نماد) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, نماد);
        إذا (!pc)
                أرجع -EINVAL;

        dot = memchr(p, '.', pc - p);
        إذا (dot) {
                إذا (dot + 3 == pc) {
                        /* Support two places after the dot */

                        إذا (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                أرجع -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } وإلا إذا (dot + 2 == pc) {
                        /* Support one place after the dot */

                        إذا (dot[1] < '0' || dot[1] > '9')
                                أرجع -EINVAL;
                        q = (dot[1] - '0') * 10;
                } وإلا
                        /* We do not support zero or more than two places */
                        أرجع -EINVAL;

                n = strndupa_امن(p, dot - p);
        } وإلا {
                q = 0;
                n = strndupa_امن(p, pc - p);
        }
        r = امن_atoi(n, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v < 0)
                أرجع -ERANGE;
        إذا (v > (بیشترین_صحیح - q) / 100)
                أرجع -ERANGE;

        v = v * 100 + q;
        أرجع v;
}

صحيح parse_percent_unbounded(ثابت حرف *p) {
        أرجع parse_parts_مقدار_کامل(p, "%");
}

صحيح parse_percent(ثابت حرف *p) {
        صحيح v;

        v = parse_percent_unbounded(p);
        إذا (v > 100)
                أرجع -ERANGE;

        أرجع v;
}

صحيح parse_permille_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‰");
        إذا (pm)
                أرجع parse_parts_مقدار_کامل(p, "‰");

        أرجع parse_parts_مقدار_with_tenths_قراربده(p, "%");
}

صحيح parse_permille(ثابت حرف *p) {
        صحيح v;

        v = parse_permille_unbounded(p);
        إذا (v > 1000)
                أرجع -ERANGE;

        أرجع v;
}

صحيح parse_permyriad_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‱");
        إذا (pm)
                أرجع parse_parts_مقدار_کامل(p, "‱");

        pm = endswith(p, "‰");
        إذا (pm)
                أرجع parse_parts_مقدار_with_tenths_قراربده(p, "‰");

        أرجع parse_parts_مقدار_with_hundredths_قراربده(p, "%");
}

صحيح parse_permyriad(ثابت حرف *p) {
        صحيح v;

        v = parse_permyriad_unbounded(p);
        إذا (v > 10000)
                أرجع -ERANGE;

        أرجع v;
}
