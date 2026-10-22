/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "parse-util.h"
#include "percent-util.h"
#include "string-util.h"

static ចំនួនគត់ parse_parts_value_whole(const var_na *p, const var_na *symbol) {
        const var_na *pc, *n;
        ចំនួនគត់ r, v;

        pc = endswith(p, symbol);
        ប្រសិនបើ (!pc)
                ត្រឡប់ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (v < 0)
                ត្រឡប់ -ERANGE;

        ត្រឡប់ v;
}

static ចំនួនគត់ parse_parts_value_with_tenths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ចំនួនគត់ r, q, v;

        pc = endswith(p, symbol);
        ប្រសិនបើ (!pc)
                ត្រឡប់ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ប្រសិនបើ (dot) {
                ប្រសិនបើ (dot + 2 != pc)
                        ត្រឡប់ -EINVAL;
                ប្រសិនបើ (dot[1] < '0' || dot[1] > '9')
                        ត្រឡប់ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } បើមិនដូច្នេះ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (v < 0)
                ត្រឡប់ -ERANGE;
        ប្រសិនបើ (v > (INT_MAX - q) / 10)
                ត្រឡប់ -ERANGE;

        v = v * 10 + q;
        ត្រឡប់ v;
}

static ចំនួនគត់ parse_parts_value_with_hundredths_place(const var_na *p, const var_na *symbol) {
        const var_na *pc, *dot, *n;
        ចំនួនគត់ r, q, v;

        pc = endswith(p, symbol);
        ប្រសិនបើ (!pc)
                ត្រឡប់ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ប្រសិនបើ (dot) {
                ប្រសិនបើ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ប្រសិនបើ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ត្រឡប់ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } បើមិនដូច្នេះ ប្រសិនបើ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ប្រសិនបើ (dot[1] < '0' || dot[1] > '9')
                                ត្រឡប់ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } បើមិនដូច្នេះ
                        /* We do not support zero or more than two places */
                        ត្រឡប់ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } បើមិនដូច្នេះ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (v < 0)
                ត្រឡប់ -ERANGE;
        ប្រសិនបើ (v > (INT_MAX - q) / 100)
                ត្រឡប់ -ERANGE;

        v = v * 100 + q;
        ត្រឡប់ v;
}

ចំនួនគត់ parse_percent_unbounded(const var_na *p) {
        ត្រឡប់ parse_parts_value_whole(p, "%");
}

ចំនួនគត់ parse_percent(const var_na *p) {
        ចំនួនគត់ v;

        v = parse_percent_unbounded(p);
        ប្រសិនបើ (v > 100)
                ត្រឡប់ -ERANGE;

        ត្រឡប់ v;
}

ចំនួនគត់ parse_permille_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‰");
        ប្រសិនបើ (pm)
                ត្រឡប់ parse_parts_value_whole(p, "‰");

        ត្រឡប់ parse_parts_value_with_tenths_place(p, "%");
}

ចំនួនគត់ parse_permille(const var_na *p) {
        ចំនួនគត់ v;

        v = parse_permille_unbounded(p);
        ប្រសិនបើ (v > 1000)
                ត្រឡប់ -ERANGE;

        ត្រឡប់ v;
}

ចំនួនគត់ parse_permyriad_unbounded(const var_na *p) {
        const var_na *pm;

        pm = endswith(p, "‱");
        ប្រសិនបើ (pm)
                ត្រឡប់ parse_parts_value_whole(p, "‱");

        pm = endswith(p, "‰");
        ប្រសិនបើ (pm)
                ត្រឡប់ parse_parts_value_with_tenths_place(p, "‰");

        ត្រឡប់ parse_parts_value_with_hundredths_place(p, "%");
}

ចំនួនគត់ parse_permyriad(const var_na *p) {
        ចំនួនគត់ v;

        v = parse_permyriad_unbounded(p);
        ប្រសិនបើ (v > 10000)
                ត្រឡប់ -ERANGE;

        ត្រឡប់ v;
}
