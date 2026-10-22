/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "percent-util.h"
#અન્તર્ભાવ "string-util.h"

સ્થિર પૂર્ણાંક parse_parts_કિંમત_whole(અચલ અક્ષર *p, અચલ અક્ષર *પ્રતીક) {
        અચલ અક્ષર *pc, *n;
        પૂર્ણાંક r, v;

        pc = endswith(p, પ્રતીક);
        જો (!pc)
                પાછા -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        જો (r < 0)
                પાછા r;
        જો (v < 0)
                પાછા -ERANGE;

        પાછા v;
}

સ્થિર પૂર્ણાંક parse_parts_કિંમત_with_tenths_place(અચલ અક્ષર *p, અચલ અક્ષર *પ્રતીક) {
        અચલ અક્ષર *pc, *dot, *n;
        પૂર્ણાંક r, q, v;

        pc = endswith(p, પ્રતીક);
        જો (!pc)
                પાછા -EINVAL;

        dot = memchr(p, '.', pc - p);
        જો (dot) {
                જો (dot + 2 != pc)
                        પાછા -EINVAL;
                જો (dot[1] < '0' || dot[1] > '9')
                        પાછા -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } નહીંતર {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        જો (r < 0)
                પાછા r;
        જો (v < 0)
                પાછા -ERANGE;
        જો (v > (INT_MAX - q) / 10)
                પાછા -ERANGE;

        v = v * 10 + q;
        પાછા v;
}

સ્થિર પૂર્ણાંક parse_parts_કિંમત_with_hundredths_place(અચલ અક્ષર *p, અચલ અક્ષર *પ્રતીક) {
        અચલ અક્ષર *pc, *dot, *n;
        પૂર્ણાંક r, q, v;

        pc = endswith(p, પ્રતીક);
        જો (!pc)
                પાછા -EINVAL;

        dot = memchr(p, '.', pc - p);
        જો (dot) {
                જો (dot + 3 == pc) {
                        /* Support two places after the dot */

                        જો (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                પાછા -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } નહીંતર જો (dot + 2 == pc) {
                        /* Support one place after the dot */

                        જો (dot[1] < '0' || dot[1] > '9')
                                પાછા -EINVAL;
                        q = (dot[1] - '0') * 10;
                } નહીંતર
                        /* We do not support zero or more than two places */
                        પાછા -EINVAL;

                n = strndupa_safe(p, dot - p);
        } નહીંતર {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        જો (r < 0)
                પાછા r;
        જો (v < 0)
                પાછા -ERANGE;
        જો (v > (INT_MAX - q) / 100)
                પાછા -ERANGE;

        v = v * 100 + q;
        પાછા v;
}

પૂર્ણાંક parse_percent_unbounded(અચલ અક્ષર *p) {
        પાછા parse_parts_કિંમત_whole(p, "%");
}

પૂર્ણાંક parse_percent(અચલ અક્ષર *p) {
        પૂર્ણાંક v;

        v = parse_percent_unbounded(p);
        જો (v > 100)
                પાછા -ERANGE;

        પાછા v;
}

પૂર્ણાંક parse_permille_unbounded(અચલ અક્ષર *p) {
        અચલ અક્ષર *pm;

        pm = endswith(p, "‰");
        જો (pm)
                પાછા parse_parts_કિંમત_whole(p, "‰");

        પાછા parse_parts_કિંમત_with_tenths_place(p, "%");
}

પૂર્ણાંક parse_permille(અચલ અક્ષર *p) {
        પૂર્ણાંક v;

        v = parse_permille_unbounded(p);
        જો (v > 1000)
                પાછા -ERANGE;

        પાછા v;
}

પૂર્ણાંક parse_permyriad_unbounded(અચલ અક્ષર *p) {
        અચલ અક્ષર *pm;

        pm = endswith(p, "‱");
        જો (pm)
                પાછા parse_parts_કિંમત_whole(p, "‱");

        pm = endswith(p, "‰");
        જો (pm)
                પાછા parse_parts_કિંમત_with_tenths_place(p, "‰");

        પાછા parse_parts_કિંમત_with_hundredths_place(p, "%");
}

પૂર્ણાંક parse_permyriad(અચલ અક્ષર *p) {
        પૂર્ણાંક v;

        v = parse_permyriad_unbounded(p);
        જો (v > 10000)
                પાછા -ERANGE;

        પાછા v;
}
