/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "percent-util.h"
#অন্তর্ভাৱ "string-util.h"

স্থির পূর্ণাঙ্ক parse_parts_মান_whole(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *প্ৰতীক) {
        ধ্রুৱ ৱর্ণ *pc, *n;
        পূর্ণাঙ্ক r, v;

        pc = endswith(p, প্ৰতীক);
        যদি (!pc)
                ঘূৰাই -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (v < 0)
                ঘূৰাই -ERANGE;

        ঘূৰাই v;
}

স্থির পূর্ণাঙ্ক parse_parts_মান_with_tenths_place(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *প্ৰতীক) {
        ধ্রুৱ ৱর্ণ *pc, *dot, *n;
        পূর্ণাঙ্ক r, q, v;

        pc = endswith(p, প্ৰতীক);
        যদি (!pc)
                ঘূৰাই -EINVAL;

        dot = memchr(p, '.', pc - p);
        যদি (dot) {
                যদি (dot + 2 != pc)
                        ঘূৰাই -EINVAL;
                যদি (dot[1] < '0' || dot[1] > '9')
                        ঘূৰাই -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } অন্যথা {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (v < 0)
                ঘূৰাই -ERANGE;
        যদি (v > (INT_MAX - q) / 10)
                ঘূৰাই -ERANGE;

        v = v * 10 + q;
        ঘূৰাই v;
}

স্থির পূর্ণাঙ্ক parse_parts_মান_with_hundredths_place(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *প্ৰতীক) {
        ধ্রুৱ ৱর্ণ *pc, *dot, *n;
        পূর্ণাঙ্ক r, q, v;

        pc = endswith(p, প্ৰতীক);
        যদি (!pc)
                ঘূৰাই -EINVAL;

        dot = memchr(p, '.', pc - p);
        যদি (dot) {
                যদি (dot + 3 == pc) {
                        /* Support two places after the dot */

                        যদি (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ঘূৰাই -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } অন্যথা যদি (dot + 2 == pc) {
                        /* Support one place after the dot */

                        যদি (dot[1] < '0' || dot[1] > '9')
                                ঘূৰাই -EINVAL;
                        q = (dot[1] - '0') * 10;
                } অন্যথা
                        /* We do not support zero or more than two places */
                        ঘূৰাই -EINVAL;

                n = strndupa_safe(p, dot - p);
        } অন্যথা {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (v < 0)
                ঘূৰাই -ERANGE;
        যদি (v > (INT_MAX - q) / 100)
                ঘূৰাই -ERANGE;

        v = v * 100 + q;
        ঘূৰাই v;
}

পূর্ণাঙ্ক parse_percent_unbounded(ধ্রুৱ ৱর্ণ *p) {
        ঘূৰাই parse_parts_মান_whole(p, "%");
}

পূর্ণাঙ্ক parse_percent(ধ্রুৱ ৱর্ণ *p) {
        পূর্ণাঙ্ক v;

        v = parse_percent_unbounded(p);
        যদি (v > 100)
                ঘূৰাই -ERANGE;

        ঘূৰাই v;
}

পূর্ণাঙ্ক parse_permille_unbounded(ধ্রুৱ ৱর্ণ *p) {
        ধ্রুৱ ৱর্ণ *pm;

        pm = endswith(p, "‰");
        যদি (pm)
                ঘূৰাই parse_parts_মান_whole(p, "‰");

        ঘূৰাই parse_parts_মান_with_tenths_place(p, "%");
}

পূর্ণাঙ্ক parse_permille(ধ্রুৱ ৱর্ণ *p) {
        পূর্ণাঙ্ক v;

        v = parse_permille_unbounded(p);
        যদি (v > 1000)
                ঘূৰাই -ERANGE;

        ঘূৰাই v;
}

পূর্ণাঙ্ক parse_permyriad_unbounded(ধ্রুৱ ৱর্ণ *p) {
        ধ্রুৱ ৱর্ণ *pm;

        pm = endswith(p, "‱");
        যদি (pm)
                ঘূৰাই parse_parts_মান_whole(p, "‱");

        pm = endswith(p, "‰");
        যদি (pm)
                ঘূৰাই parse_parts_মান_with_tenths_place(p, "‰");

        ঘূৰাই parse_parts_মান_with_hundredths_place(p, "%");
}

পূর্ণাঙ্ক parse_permyriad(ধ্রুৱ ৱর্ণ *p) {
        পূর্ণাঙ্ক v;

        v = parse_permyriad_unbounded(p);
        যদি (v > 10000)
                ঘূৰাই -ERANGE;

        ঘূৰাই v;
}
