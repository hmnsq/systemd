/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "parse-util.h"
#شامل "percent-util.h"
#شامل "string-util.h"

ساکن صحیح parse_parts_قدر_پورا(ثابت حرف *p, ثابت حرف *علامت) {
        ثابت حرف *pc, *n;
        صحیح r, v;

        pc = endswith(p, علامت);
        اگر (!pc)
                واپس -EINVAL;

        n = strndupa_محفوظ(p, pc - p);
        r = محفوظ_atoi(n, &v);
        اگر (r < 0)
                واپس r;
        اگر (v < 0)
                واپس -ERANGE;

        واپس v;
}

ساکن صحیح parse_parts_قدر_with_tenths_رکھو(ثابت حرف *p, ثابت حرف *علامت) {
        ثابت حرف *pc, *dot, *n;
        صحیح r, q, v;

        pc = endswith(p, علامت);
        اگر (!pc)
                واپس -EINVAL;

        dot = memchr(p, '.', pc - p);
        اگر (dot) {
                اگر (dot + 2 != pc)
                        واپس -EINVAL;
                اگر (dot[1] < '0' || dot[1] > '9')
                        واپس -EINVAL;
                q = dot[1] - '0';
                n = strndupa_محفوظ(p, dot - p);
        } ورنہ {
                q = 0;
                n = strndupa_محفوظ(p, pc - p);
        }
        r = محفوظ_atoi(n, &v);
        اگر (r < 0)
                واپس r;
        اگر (v < 0)
                واپس -ERANGE;
        اگر (v > (زیادہ_سے_زیادہ_عدد - q) / 10)
                واپس -ERANGE;

        v = v * 10 + q;
        واپس v;
}

ساکن صحیح parse_parts_قدر_with_hundredths_رکھو(ثابت حرف *p, ثابت حرف *علامت) {
        ثابت حرف *pc, *dot, *n;
        صحیح r, q, v;

        pc = endswith(p, علامت);
        اگر (!pc)
                واپس -EINVAL;

        dot = memchr(p, '.', pc - p);
        اگر (dot) {
                اگر (dot + 3 == pc) {
                        /* Support two places after the dot */

                        اگر (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                واپس -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ورنہ اگر (dot + 2 == pc) {
                        /* Support one place after the dot */

                        اگر (dot[1] < '0' || dot[1] > '9')
                                واپس -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ورنہ
                        /* We do not support zero or more than two places */
                        واپس -EINVAL;

                n = strndupa_محفوظ(p, dot - p);
        } ورنہ {
                q = 0;
                n = strndupa_محفوظ(p, pc - p);
        }
        r = محفوظ_atoi(n, &v);
        اگر (r < 0)
                واپس r;
        اگر (v < 0)
                واپس -ERANGE;
        اگر (v > (زیادہ_سے_زیادہ_عدد - q) / 100)
                واپس -ERANGE;

        v = v * 100 + q;
        واپس v;
}

صحیح parse_percent_unbounded(ثابت حرف *p) {
        واپس parse_parts_قدر_پورا(p, "%");
}

صحیح parse_percent(ثابت حرف *p) {
        صحیح v;

        v = parse_percent_unbounded(p);
        اگر (v > 100)
                واپس -ERANGE;

        واپس v;
}

صحیح parse_permille_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‰");
        اگر (pm)
                واپس parse_parts_قدر_پورا(p, "‰");

        واپس parse_parts_قدر_with_tenths_رکھو(p, "%");
}

صحیح parse_permille(ثابت حرف *p) {
        صحیح v;

        v = parse_permille_unbounded(p);
        اگر (v > 1000)
                واپس -ERANGE;

        واپس v;
}

صحیح parse_permyriad_unbounded(ثابت حرف *p) {
        ثابت حرف *pm;

        pm = endswith(p, "‱");
        اگر (pm)
                واپس parse_parts_قدر_پورا(p, "‱");

        pm = endswith(p, "‰");
        اگر (pm)
                واپس parse_parts_قدر_with_tenths_رکھو(p, "‰");

        واپس parse_parts_قدر_with_hundredths_رکھو(p, "%");
}

صحیح parse_permyriad(ثابت حرف *p) {
        صحیح v;

        v = parse_permyriad_unbounded(p);
        اگر (v > 10000)
                واپس -ERANGE;

        واپس v;
}
