/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "percent-util.h"
#অন্তর্ভাব "string-util.h"

স্থির পূর্ণসংখ্যা parse_parts_মান_whole(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *প্রতীক) {
        ধ্রুবক অক্ষর *pc, *n;
        পূর্ণসংখ্যা r, v;

        pc = endswith(p, প্রতীক);
        যদি (!pc)
                ফেরত -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        যদি (r < 0)
                ফেরত r;
        যদি (v < 0)
                ফেরত -ERANGE;

        ফেরত v;
}

স্থির পূর্ণসংখ্যা parse_parts_মান_with_tenths_place(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *প্রতীক) {
        ধ্রুবক অক্ষর *pc, *dot, *n;
        পূর্ণসংখ্যা r, q, v;

        pc = endswith(p, প্রতীক);
        যদি (!pc)
                ফেরত -EINVAL;

        dot = memchr(p, '.', pc - p);
        যদি (dot) {
                যদি (dot + 2 != pc)
                        ফেরত -EINVAL;
                যদি (dot[1] < '0' || dot[1] > '9')
                        ফেরত -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } অন্যথা {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        যদি (r < 0)
                ফেরত r;
        যদি (v < 0)
                ফেরত -ERANGE;
        যদি (v > (INT_MAX - q) / 10)
                ফেরত -ERANGE;

        v = v * 10 + q;
        ফেরত v;
}

স্থির পূর্ণসংখ্যা parse_parts_মান_with_hundredths_place(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *প্রতীক) {
        ধ্রুবক অক্ষর *pc, *dot, *n;
        পূর্ণসংখ্যা r, q, v;

        pc = endswith(p, প্রতীক);
        যদি (!pc)
                ফেরত -EINVAL;

        dot = memchr(p, '.', pc - p);
        যদি (dot) {
                যদি (dot + 3 == pc) {
                        /* Support two places after the dot */

                        যদি (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ফেরত -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } অন্যথা যদি (dot + 2 == pc) {
                        /* Support one place after the dot */

                        যদি (dot[1] < '0' || dot[1] > '9')
                                ফেরত -EINVAL;
                        q = (dot[1] - '0') * 10;
                } অন্যথা
                        /* We do not support zero or more than two places */
                        ফেরত -EINVAL;

                n = strndupa_safe(p, dot - p);
        } অন্যথা {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        যদি (r < 0)
                ফেরত r;
        যদি (v < 0)
                ফেরত -ERANGE;
        যদি (v > (INT_MAX - q) / 100)
                ফেরত -ERANGE;

        v = v * 100 + q;
        ফেরত v;
}

পূর্ণসংখ্যা parse_percent_unbounded(ধ্রুবক অক্ষর *p) {
        ফেরত parse_parts_মান_whole(p, "%");
}

পূর্ণসংখ্যা parse_percent(ধ্রুবক অক্ষর *p) {
        পূর্ণসংখ্যা v;

        v = parse_percent_unbounded(p);
        যদি (v > 100)
                ফেরত -ERANGE;

        ফেরত v;
}

পূর্ণসংখ্যা parse_permille_unbounded(ধ্রুবক অক্ষর *p) {
        ধ্রুবক অক্ষর *pm;

        pm = endswith(p, "‰");
        যদি (pm)
                ফেরত parse_parts_মান_whole(p, "‰");

        ফেরত parse_parts_মান_with_tenths_place(p, "%");
}

পূর্ণসংখ্যা parse_permille(ধ্রুবক অক্ষর *p) {
        পূর্ণসংখ্যা v;

        v = parse_permille_unbounded(p);
        যদি (v > 1000)
                ফেরত -ERANGE;

        ফেরত v;
}

পূর্ণসংখ্যা parse_permyriad_unbounded(ধ্রুবক অক্ষর *p) {
        ধ্রুবক অক্ষর *pm;

        pm = endswith(p, "‱");
        যদি (pm)
                ফেরত parse_parts_মান_whole(p, "‱");

        pm = endswith(p, "‰");
        যদি (pm)
                ফেরত parse_parts_মান_with_tenths_place(p, "‰");

        ফেরত parse_parts_মান_with_hundredths_place(p, "%");
}

পূর্ণসংখ্যা parse_permyriad(ধ্রুবক অক্ষর *p) {
        পূর্ণসংখ্যা v;

        v = parse_permyriad_unbounded(p);
        যদি (v > 10000)
                ফেরত -ERANGE;

        ফেরত v;
}
