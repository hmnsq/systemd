/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "percent-util.h"
#অন্তর্ভাব "string-util.h"

স্থির পূর্ণাঙ্ক parse_parts_মান_whole(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *প্রতীক) {
        ধ্রুব বর্ণ *pc, *n;
        পূর্ণাঙ্ক r, v;

        pc = endswith(p, প্রতীক);
        যদি (!pc)
                প্রত্যাগম -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (v < 0)
                প্রত্যাগম -ERANGE;

        প্রত্যাগম v;
}

স্থির পূর্ণাঙ্ক parse_parts_মান_with_tenths_place(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *প্রতীক) {
        ধ্রুব বর্ণ *pc, *dot, *n;
        পূর্ণাঙ্ক r, q, v;

        pc = endswith(p, প্রতীক);
        যদি (!pc)
                প্রত্যাগম -EINVAL;

        dot = memchr(p, '.', pc - p);
        যদি (dot) {
                যদি (dot + 2 != pc)
                        প্রত্যাগম -EINVAL;
                যদি (dot[1] < '0' || dot[1] > '9')
                        প্রত্যাগম -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } অন্যথা {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (v < 0)
                প্রত্যাগম -ERANGE;
        যদি (v > (INT_MAX - q) / 10)
                প্রত্যাগম -ERANGE;

        v = v * 10 + q;
        প্রত্যাগম v;
}

স্থির পূর্ণাঙ্ক parse_parts_মান_with_hundredths_place(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *প্রতীক) {
        ধ্রুব বর্ণ *pc, *dot, *n;
        পূর্ণাঙ্ক r, q, v;

        pc = endswith(p, প্রতীক);
        যদি (!pc)
                প্রত্যাগম -EINVAL;

        dot = memchr(p, '.', pc - p);
        যদি (dot) {
                যদি (dot + 3 == pc) {
                        /* Support two places after the dot */

                        যদি (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                প্রত্যাগম -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } অন্যথা যদি (dot + 2 == pc) {
                        /* Support one place after the dot */

                        যদি (dot[1] < '0' || dot[1] > '9')
                                প্রত্যাগম -EINVAL;
                        q = (dot[1] - '0') * 10;
                } অন্যথা
                        /* We do not support zero or more than two places */
                        প্রত্যাগম -EINVAL;

                n = strndupa_safe(p, dot - p);
        } অন্যথা {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (v < 0)
                প্রত্যাগম -ERANGE;
        যদি (v > (INT_MAX - q) / 100)
                প্রত্যাগম -ERANGE;

        v = v * 100 + q;
        প্রত্যাগম v;
}

পূর্ণাঙ্ক parse_percent_unbounded(ধ্রুব বর্ণ *p) {
        প্রত্যাগম parse_parts_মান_whole(p, "%");
}

পূর্ণাঙ্ক parse_percent(ধ্রুব বর্ণ *p) {
        পূর্ণাঙ্ক v;

        v = parse_percent_unbounded(p);
        যদি (v > 100)
                প্রত্যাগম -ERANGE;

        প্রত্যাগম v;
}

পূর্ণাঙ্ক parse_permille_unbounded(ধ্রুব বর্ণ *p) {
        ধ্রুব বর্ণ *pm;

        pm = endswith(p, "‰");
        যদি (pm)
                প্রত্যাগম parse_parts_মান_whole(p, "‰");

        প্রত্যাগম parse_parts_মান_with_tenths_place(p, "%");
}

পূর্ণাঙ্ক parse_permille(ধ্রুব বর্ণ *p) {
        পূর্ণাঙ্ক v;

        v = parse_permille_unbounded(p);
        যদি (v > 1000)
                প্রত্যাগম -ERANGE;

        প্রত্যাগম v;
}

পূর্ণাঙ্ক parse_permyriad_unbounded(ধ্রুব বর্ণ *p) {
        ধ্রুব বর্ণ *pm;

        pm = endswith(p, "‱");
        যদি (pm)
                প্রত্যাগম parse_parts_মান_whole(p, "‱");

        pm = endswith(p, "‰");
        যদি (pm)
                প্রত্যাগম parse_parts_মান_with_tenths_place(p, "‰");

        প্রত্যাগম parse_parts_মান_with_hundredths_place(p, "%");
}

পূর্ণাঙ্ক parse_permyriad(ধ্রুব বর্ণ *p) {
        পূর্ণাঙ্ক v;

        v = parse_permyriad_unbounded(p);
        যদি (v > 10000)
                প্রত্যাগম -ERANGE;

        প্রত্যাগম v;
}
