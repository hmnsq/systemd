/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static puur_naa_nka parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        puur_naa_nka r, v;

        pc = endswith(p, symbol);
        ပေႃး (!pc)
                ပွၵ်ႈ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;
        ပေႃး (v < 0)
                ပွၵ်ႈ -ERANGE;

        ပွၵ်ႈ v;
}

static puur_naa_nka parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ပေႃး (!pc)
                ပွၵ်ႈ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ပေႃး (dot) {
                ပေႃး (dot + 2 != pc)
                        ပွၵ်ႈ -EINVAL;
                ပေႃး (dot[1] < '0' || dot[1] > '9')
                        ပွၵ်ႈ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ပေႃးဢမ်ႇ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;
        ပေႃး (v < 0)
                ပွၵ်ႈ -ERANGE;
        ပေႃး (v > (INT_MAX - q) / 10)
                ပွၵ်ႈ -ERANGE;

        v = v * 10 + q;
        ပွၵ်ႈ v;
}

static puur_naa_nka parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        puur_naa_nka r, q, v;

        pc = endswith(p, symbol);
        ပေႃး (!pc)
                ပွၵ်ႈ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ပေႃး (dot) {
                ပေႃး (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ပေႃး (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ပွၵ်ႈ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ပေႃးဢမ်ႇ ပေႃး (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ပေႃး (dot[1] < '0' || dot[1] > '9')
                                ပွၵ်ႈ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ပေႃးဢမ်ႇ
                        /* We do not support zero or more than two places */
                        ပွၵ်ႈ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ပေႃးဢမ်ႇ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;
        ပေႃး (v < 0)
                ပွၵ်ႈ -ERANGE;
        ပေႃး (v > (INT_MAX - q) / 100)
                ပွၵ်ႈ -ERANGE;

        v = v * 100 + q;
        ပွၵ်ႈ v;
}

puur_naa_nka parse_percent_unbounded(const var_na *p) {
        ပွၵ်ႈ parse_parts_value_whole(p, "%");
}

puur_naa_nka parse_percent(const var_na *p) {
        puur_naa_nka v;

        v = parse_percent_unbounded(p);
        ပေႃး (v > 100)
                ပွၵ်ႈ -ERANGE;

        ပွၵ်ႈ v;
}

puur_naa_nka parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ပေႃး (pm)
                ပွၵ်ႈ parse_parts_value_whole(p, "‰");

        ပွၵ်ႈ parse_parts_value_with_tenths_place(p, "%");
}

puur_naa_nka parse_permille(const var_na *p) {
        puur_naa_nka v;

        v = parse_permille_unbounded(p);
        ပေႃး (v > 1000)
                ပွၵ်ႈ -ERANGE;

        ပွၵ်ႈ v;
}

puur_naa_nka parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ပေႃး (pm)
                ပွၵ်ႈ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ပေႃး (pm)
                ပွၵ်ႈ parse_parts_value_with_tenths_place(p, "‰");

        ပွၵ်ႈ parse_parts_value_with_hundredths_place(p, "%");
}

puur_naa_nka parse_permyriad(const var_na *p) {
        puur_naa_nka v;

        v = parse_permyriad_unbounded(p);
        ပေႃး (v > 10000)
                ပွၵ်ႈ -ERANGE;

        ပွၵ်ႈ v;
}
