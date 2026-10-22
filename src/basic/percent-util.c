/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "parse-util.h"
#ضمن "percent-util.h"
#ضمن "string-util.h"

ساكن صحيح parse_parts_درج_whole(ثابت حرف *p, ثابت حرف *علام) {
        ثابت حرف *pc, *n;
        صحيح r, v;

        pc = endswith(p, علام);
        إذا (!pc)
                أرجع -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v < 0)
                أرجع -ERANGE;

        أرجع v;
}

ساكن صحيح parse_parts_درج_with_tenths_place(ثابت حرف *p, ثابت حرف *علام) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, علام);
        إذا (!pc)
                أرجع -EINVAL;

        dot = memchr(p, '.', pc - p);
        إذا (dot) {
                إذا (dot + 2 != pc)
                        أرجع -EINVAL;
                إذا (dot[1] < '0' || dot[1] > '9')
                        أرجع -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } وإلا {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v < 0)
                أرجع -ERANGE;
        إذا (v > (INT_MAX - q) / 10)
                أرجع -ERANGE;

        v = v * 10 + q;
        أرجع v;
}

ساكن صحيح parse_parts_درج_with_hundredths_place(ثابت حرف *p, ثابت حرف *علام) {
        ثابت حرف *pc, *dot, *n;
        صحيح r, q, v;

        pc = endswith(p, علام);
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

                n = strndupa_safe(p, dot - p);
        } وإلا {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        إذا (r < 0)
                أرجع r;
        إذا (v < 0)
                أرجع -ERANGE;
        إذا (v > (INT_MAX - q) / 100)
                أرجع -ERANGE;

        v = v * 100 + q;
        أرجع v;
}

صحيح parse_percent_unbounded(ثابت حرف *p) {
        أرجع parse_parts_درج_whole(p, "%");
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
                أرجع parse_parts_درج_whole(p, "‰");

        أرجع parse_parts_درج_with_tenths_place(p, "%");
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
                أرجع parse_parts_درج_whole(p, "‱");

        pm = endswith(p, "‰");
        إذا (pm)
                أرجع parse_parts_درج_with_tenths_place(p, "‰");

        أرجع parse_parts_درج_with_hundredths_place(p, "%");
}

صحيح parse_permyriad(ثابت حرف *p) {
        صحيح v;

        v = parse_permyriad_unbounded(p);
        إذا (v > 10000)
                أرجع -ERANGE;

        أرجع v;
}
