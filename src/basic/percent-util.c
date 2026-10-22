/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static ຈຳນວນເຕັມ parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        ຈຳນວນເຕັມ r, v;

        pc = endswith(p, symbol);
        ຖ້າ (!pc)
                ສົ່ງຄືນ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (v < 0)
                ສົ່ງຄືນ -ERANGE;

        ສົ່ງຄືນ v;
}

static ຈຳນວນເຕັມ parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ຈຳນວນເຕັມ r, q, v;

        pc = endswith(p, symbol);
        ຖ້າ (!pc)
                ສົ່ງຄືນ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ຖ້າ (dot) {
                ຖ້າ (dot + 2 != pc)
                        ສົ່ງຄືນ -EINVAL;
                ຖ້າ (dot[1] < '0' || dot[1] > '9')
                        ສົ່ງຄືນ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ບໍ່ດັ່ງນັ້ນ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (v < 0)
                ສົ່ງຄືນ -ERANGE;
        ຖ້າ (v > (INT_MAX - q) / 10)
                ສົ່ງຄືນ -ERANGE;

        v = v * 10 + q;
        ສົ່ງຄືນ v;
}

static ຈຳນວນເຕັມ parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ຈຳນວນເຕັມ r, q, v;

        pc = endswith(p, symbol);
        ຖ້າ (!pc)
                ສົ່ງຄືນ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ຖ້າ (dot) {
                ຖ້າ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ຖ້າ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ສົ່ງຄືນ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ຖ້າ (dot[1] < '0' || dot[1] > '9')
                                ສົ່ງຄືນ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ບໍ່ດັ່ງນັ້ນ
                        /* We do not support zero or more than two places */
                        ສົ່ງຄືນ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ບໍ່ດັ່ງນັ້ນ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (v < 0)
                ສົ່ງຄືນ -ERANGE;
        ຖ້າ (v > (INT_MAX - q) / 100)
                ສົ່ງຄືນ -ERANGE;

        v = v * 100 + q;
        ສົ່ງຄືນ v;
}

ຈຳນວນເຕັມ parse_percent_unbounded(const var_na *p) {
        ສົ່ງຄືນ parse_parts_value_whole(p, "%");
}

ຈຳນວນເຕັມ parse_percent(const var_na *p) {
        ຈຳນວນເຕັມ v;

        v = parse_percent_unbounded(p);
        ຖ້າ (v > 100)
                ສົ່ງຄືນ -ERANGE;

        ສົ່ງຄືນ v;
}

ຈຳນວນເຕັມ parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ຖ້າ (pm)
                ສົ່ງຄືນ parse_parts_value_whole(p, "‰");

        ສົ່ງຄືນ parse_parts_value_with_tenths_place(p, "%");
}

ຈຳນວນເຕັມ parse_permille(const var_na *p) {
        ຈຳນວນເຕັມ v;

        v = parse_permille_unbounded(p);
        ຖ້າ (v > 1000)
                ສົ່ງຄືນ -ERANGE;

        ສົ່ງຄືນ v;
}

ຈຳນວນເຕັມ parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ຖ້າ (pm)
                ສົ່ງຄືນ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ຖ້າ (pm)
                ສົ່ງຄືນ parse_parts_value_with_tenths_place(p, "‰");

        ສົ່ງຄືນ parse_parts_value_with_hundredths_place(p, "%");
}

ຈຳນວນເຕັມ parse_permyriad(const var_na *p) {
        ຈຳນວນເຕັມ v;

        v = parse_permyriad_unbounded(p);
        ຖ້າ (v > 10000)
                ສົ່ງຄືນ -ERANGE;

        ສົ່ງຄືນ v;
}
