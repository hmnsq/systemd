/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static ကိန်းပြည့် parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        ကိန်းပြည့် r, v;

        pc = endswith(p, symbol);
        အကယ်၍ (!pc)
                ပြန်ပေး -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (v < 0)
                ပြန်ပေး -ERANGE;

        ပြန်ပေး v;
}

static ကိန်းပြည့် parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ကိန်းပြည့် r, q, v;

        pc = endswith(p, symbol);
        အကယ်၍ (!pc)
                ပြန်ပေး -EINVAL;

        dot = memchr(p, '.', pc - p);
        အကယ်၍ (dot) {
                အကယ်၍ (dot + 2 != pc)
                        ပြန်ပေး -EINVAL;
                အကယ်၍ (dot[1] < '0' || dot[1] > '9')
                        ပြန်ပေး -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } မဟုတ်ပါက {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (v < 0)
                ပြန်ပေး -ERANGE;
        အကယ်၍ (v > (INT_MAX - q) / 10)
                ပြန်ပေး -ERANGE;

        v = v * 10 + q;
        ပြန်ပေး v;
}

static ကိန်းပြည့် parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ကိန်းပြည့် r, q, v;

        pc = endswith(p, symbol);
        အကယ်၍ (!pc)
                ပြန်ပေး -EINVAL;

        dot = memchr(p, '.', pc - p);
        အကယ်၍ (dot) {
                အကယ်၍ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        အကယ်၍ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ပြန်ပေး -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } မဟုတ်ပါက အကယ်၍ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        အကယ်၍ (dot[1] < '0' || dot[1] > '9')
                                ပြန်ပေး -EINVAL;
                        q = (dot[1] - '0') * 10;
                } မဟုတ်ပါက
                        /* We do not support zero or more than two places */
                        ပြန်ပေး -EINVAL;

                n = strndupa_safe(p, dot - p);
        } မဟုတ်ပါက {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (v < 0)
                ပြန်ပေး -ERANGE;
        အကယ်၍ (v > (INT_MAX - q) / 100)
                ပြန်ပေး -ERANGE;

        v = v * 100 + q;
        ပြန်ပေး v;
}

ကိန်းပြည့် parse_percent_unbounded(const var_na *p) {
        ပြန်ပေး parse_parts_value_whole(p, "%");
}

ကိန်းပြည့် parse_percent(const var_na *p) {
        ကိန်းပြည့် v;

        v = parse_percent_unbounded(p);
        အကယ်၍ (v > 100)
                ပြန်ပေး -ERANGE;

        ပြန်ပေး v;
}

ကိန်းပြည့် parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        အကယ်၍ (pm)
                ပြန်ပေး parse_parts_value_whole(p, "‰");

        ပြန်ပေး parse_parts_value_with_tenths_place(p, "%");
}

ကိန်းပြည့် parse_permille(const var_na *p) {
        ကိန်းပြည့် v;

        v = parse_permille_unbounded(p);
        အကယ်၍ (v > 1000)
                ပြန်ပေး -ERANGE;

        ပြန်ပေး v;
}

ကိန်းပြည့် parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        အကယ်၍ (pm)
                ပြန်ပေး parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        အကယ်၍ (pm)
                ပြန်ပေး parse_parts_value_with_tenths_place(p, "‰");

        ပြန်ပေး parse_parts_value_with_hundredths_place(p, "%");
}

ကိန်းပြည့် parse_permyriad(const var_na *p) {
        ကိန်းပြည့် v;

        v = parse_permyriad_unbounded(p);
        အကယ်၍ (v > 10000)
                ပြန်ပေး -ERANGE;

        ပြန်ပေး v;
}
