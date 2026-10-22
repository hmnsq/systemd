/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "percent-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_parts_ମୂଲ୍ୟ_whole(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *ପ୍ରତୀକ) {
        ଅଚଳ ଅକ୍ଷର *pc, *n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, v;

        pc = endswith(p, ପ୍ରତୀକ);
        ଯଦି (!pc)
                ଫେରନ୍ତୁ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (v < 0)
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ v;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_parts_ମୂଲ୍ୟ_with_tenths_place(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *ପ୍ରତୀକ) {
        ଅଚଳ ଅକ୍ଷର *pc, *dot, *n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, q, v;

        pc = endswith(p, ପ୍ରତୀକ);
        ଯଦି (!pc)
                ଫେରନ୍ତୁ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ଯଦି (dot) {
                ଯଦି (dot + 2 != pc)
                        ଫେରନ୍ତୁ -EINVAL;
                ଯଦି (dot[1] < '0' || dot[1] > '9')
                        ଫେରନ୍ତୁ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ଅନ୍ୟଥା {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (v < 0)
                ଫେରନ୍ତୁ -ERANGE;
        ଯଦି (v > (INT_MAX - q) / 10)
                ଫେରନ୍ତୁ -ERANGE;

        v = v * 10 + q;
        ଫେରନ୍ତୁ v;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_parts_ମୂଲ୍ୟ_with_hundredths_place(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *ପ୍ରତୀକ) {
        ଅଚଳ ଅକ୍ଷର *pc, *dot, *n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, q, v;

        pc = endswith(p, ପ୍ରତୀକ);
        ଯଦି (!pc)
                ଫେରନ୍ତୁ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ଯଦି (dot) {
                ଯଦି (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ଯଦି (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ଫେରନ୍ତୁ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ଅନ୍ୟଥା ଯଦି (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ଯଦି (dot[1] < '0' || dot[1] > '9')
                                ଫେରନ୍ତୁ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ଅନ୍ୟଥା
                        /* We do not support zero or more than two places */
                        ଫେରନ୍ତୁ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ଅନ୍ୟଥା {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (v < 0)
                ଫେରନ୍ତୁ -ERANGE;
        ଯଦି (v > (INT_MAX - q) / 100)
                ଫେରନ୍ତୁ -ERANGE;

        v = v * 100 + q;
        ଫେରନ୍ତୁ v;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_percent_unbounded(ଅଚଳ ଅକ୍ଷର *p) {
        ଫେରନ୍ତୁ parse_parts_ମୂଲ୍ୟ_whole(p, "%");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_percent(ଅଚଳ ଅକ୍ଷର *p) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;

        v = parse_percent_unbounded(p);
        ଯଦି (v > 100)
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ v;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_permille_unbounded(ଅଚଳ ଅକ୍ଷର *p) {
        ଅଚଳ ଅକ୍ଷର *pm;

        pm = endswith(p, "‰");
        ଯଦି (pm)
                ଫେରନ୍ତୁ parse_parts_ମୂଲ୍ୟ_whole(p, "‰");

        ଫେରନ୍ତୁ parse_parts_ମୂଲ୍ୟ_with_tenths_place(p, "%");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_permille(ଅଚଳ ଅକ୍ଷର *p) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;

        v = parse_permille_unbounded(p);
        ଯଦି (v > 1000)
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ v;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_permyriad_unbounded(ଅଚଳ ଅକ୍ଷର *p) {
        ଅଚଳ ଅକ୍ଷର *pm;

        pm = endswith(p, "‱");
        ଯଦି (pm)
                ଫେରନ୍ତୁ parse_parts_ମୂଲ୍ୟ_whole(p, "‱");

        pm = endswith(p, "‰");
        ଯଦି (pm)
                ଫେରନ୍ତୁ parse_parts_ମୂଲ୍ୟ_with_tenths_place(p, "‰");

        ଫେରନ୍ତୁ parse_parts_ମୂଲ୍ୟ_with_hundredths_place(p, "%");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_permyriad(ଅଚଳ ଅକ୍ଷର *p) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;

        v = parse_permyriad_unbounded(p);
        ଯଦି (v > 10000)
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ v;
}
