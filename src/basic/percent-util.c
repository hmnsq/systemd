/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static số_nguyên parse_parts_value_whole(const char *p, const char *symbol) {
        const char *pc, *n;
        số_nguyên r, v;

        pc = endswith(p, symbol);
        nếu (!pc)
                trả_về -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        nếu (r < 0)
                trả_về r;
        nếu (v < 0)
                trả_về -ERANGE;

        trả_về v;
}

static số_nguyên parse_parts_value_with_tenths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        số_nguyên r, q, v;

        pc = endswith(p, symbol);
        nếu (!pc)
                trả_về -EINVAL;

        dot = memchr(p, '.', pc - p);
        nếu (dot) {
                nếu (dot + 2 != pc)
                        trả_về -EINVAL;
                nếu (dot[1] < '0' || dot[1] > '9')
                        trả_về -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } không_thì {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        nếu (r < 0)
                trả_về r;
        nếu (v < 0)
                trả_về -ERANGE;
        nếu (v > (INT_MAX - q) / 10)
                trả_về -ERANGE;

        v = v * 10 + q;
        trả_về v;
}

static số_nguyên parse_parts_value_with_hundredths_place(const char *p, const char *symbol) {
        const char *pc, *dot, *n;
        số_nguyên r, q, v;

        pc = endswith(p, symbol);
        nếu (!pc)
                trả_về -EINVAL;

        dot = memchr(p, '.', pc - p);
        nếu (dot) {
                nếu (dot + 3 == pc) {
                        /* Support two places after the dot */

                        nếu (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                trả_về -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } không_thì nếu (dot + 2 == pc) {
                        /* Support one place after the dot */

                        nếu (dot[1] < '0' || dot[1] > '9')
                                trả_về -EINVAL;
                        q = (dot[1] - '0') * 10;
                } không_thì
                        /* We do not support zero or more than two places */
                        trả_về -EINVAL;

                n = strndupa_safe(p, dot - p);
        } không_thì {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        nếu (r < 0)
                trả_về r;
        nếu (v < 0)
                trả_về -ERANGE;
        nếu (v > (INT_MAX - q) / 100)
                trả_về -ERANGE;

        v = v * 100 + q;
        trả_về v;
}

số_nguyên parse_percent_unbounded(const char *p) {
        trả_về parse_parts_value_whole(p, "%");
}

số_nguyên parse_percent(const char *p) {
        số_nguyên v;

        v = parse_percent_unbounded(p);
        nếu (v > 100)
                trả_về -ERANGE;

        trả_về v;
}

số_nguyên parse_permille_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‰");
        nếu (pm)
                trả_về parse_parts_value_whole(p, "‰");

        trả_về parse_parts_value_with_tenths_place(p, "%");
}

số_nguyên parse_permille(const char *p) {
        số_nguyên v;

        v = parse_permille_unbounded(p);
        nếu (v > 1000)
                trả_về -ERANGE;

        trả_về v;
}

số_nguyên parse_permyriad_unbounded(const char *p) {
        const char *pm;

        pm = endswith(p, "‱");
        nếu (pm)
                trả_về parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        nếu (pm)
                trả_về parse_parts_value_with_tenths_place(p, "‰");

        trả_về parse_parts_value_with_hundredths_place(p, "%");
}

số_nguyên parse_permyriad(const char *p) {
        số_nguyên v;

        v = parse_permyriad_unbounded(p);
        nếu (v > 10000)
                trả_về -ERANGE;

        trả_về v;
}
