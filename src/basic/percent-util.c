/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "percent-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_parts_ಮೌಲ್ಯ_whole(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *ಚಿಹ್ನೆ) {
        ಧ್ರುವ ವರ್ಣ *pc, *n;
        ಪೂರ್ಣಾಙ್ಕ r, v;

        pc = endswith(p, ಚಿಹ್ನೆ);
        ಯದಿ (!pc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (v < 0)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ v;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_parts_ಮೌಲ್ಯ_with_tenths_place(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *ಚಿಹ್ನೆ) {
        ಧ್ರುವ ವರ್ಣ *pc, *dot, *n;
        ಪೂರ್ಣಾಙ್ಕ r, q, v;

        pc = endswith(p, ಚಿಹ್ನೆ);
        ಯದಿ (!pc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ಯದಿ (dot) {
                ಯದಿ (dot + 2 != pc)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                ಯದಿ (dot[1] < '0' || dot[1] > '9')
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } ಅನ್ಯಥಾ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (v < 0)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;
        ಯದಿ (v > (INT_MAX - q) / 10)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        v = v * 10 + q;
        ಪ್ರತ್ಯಾಗಮ v;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_parts_ಮೌಲ್ಯ_with_hundredths_place(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *ಚಿಹ್ನೆ) {
        ಧ್ರುವ ವರ್ಣ *pc, *dot, *n;
        ಪೂರ್ಣಾಙ್ಕ r, q, v;

        pc = endswith(p, ಚಿಹ್ನೆ);
        ಯದಿ (!pc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ಯದಿ (dot) {
                ಯದಿ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ಯದಿ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ಅನ್ಯಥಾ ಯದಿ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ಯದಿ (dot[1] < '0' || dot[1] > '9')
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ಅನ್ಯಥಾ
                        /* We do not support zero or more than two places */
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } ಅನ್ಯಥಾ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (v < 0)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;
        ಯದಿ (v > (INT_MAX - q) / 100)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        v = v * 100 + q;
        ಪ್ರತ್ಯಾಗಮ v;
}

ಪೂರ್ಣಾಙ್ಕ parse_percent_unbounded(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತ್ಯಾಗಮ parse_parts_ಮೌಲ್ಯ_whole(p, "%");
}

ಪೂರ್ಣಾಙ್ಕ parse_percent(ಧ್ರುವ ವರ್ಣ *p) {
        ಪೂರ್ಣಾಙ್ಕ v;

        v = parse_percent_unbounded(p);
        ಯದಿ (v > 100)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ v;
}

ಪೂರ್ಣಾಙ್ಕ parse_permille_unbounded(ಧ್ರುವ ವರ್ಣ *p) {
        ಧ್ರುವ ವರ್ಣ *pm;

        pm = endswith(p, "‰");
        ಯದಿ (pm)
                ಪ್ರತ್ಯಾಗಮ parse_parts_ಮೌಲ್ಯ_whole(p, "‰");

        ಪ್ರತ್ಯಾಗಮ parse_parts_ಮೌಲ್ಯ_with_tenths_place(p, "%");
}

ಪೂರ್ಣಾಙ್ಕ parse_permille(ಧ್ರುವ ವರ್ಣ *p) {
        ಪೂರ್ಣಾಙ್ಕ v;

        v = parse_permille_unbounded(p);
        ಯದಿ (v > 1000)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ v;
}

ಪೂರ್ಣಾಙ್ಕ parse_permyriad_unbounded(ಧ್ರುವ ವರ್ಣ *p) {
        ಧ್ರುವ ವರ್ಣ *pm;

        pm = endswith(p, "‱");
        ಯದಿ (pm)
                ಪ್ರತ್ಯಾಗಮ parse_parts_ಮೌಲ್ಯ_whole(p, "‱");

        pm = endswith(p, "‰");
        ಯದಿ (pm)
                ಪ್ರತ್ಯಾಗಮ parse_parts_ಮೌಲ್ಯ_with_tenths_place(p, "‰");

        ಪ್ರತ್ಯಾಗಮ parse_parts_ಮೌಲ್ಯ_with_hundredths_place(p, "%");
}

ಪೂರ್ಣಾಙ್ಕ parse_permyriad(ಧ್ರುವ ವರ್ಣ *p) {
        ಪೂರ್ಣಾಙ್ಕ v;

        v = parse_permyriad_unbounded(p);
        ಯದಿ (v > 10000)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ v;
}
