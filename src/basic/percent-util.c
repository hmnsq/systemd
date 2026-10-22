/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "percent-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_parts_ಮೌಲ್ಯ_ಸಂಪೂರ್ಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಚಿಹ್ನೆ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pc, *n;
        ಪೂರ್ಣಾಂಕ r, v;

        pc = endswith(p, ಚಿಹ್ನೆ);
        ಆದರೆ (!pc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        n = strndupa_ಸುರಕ್ಷಿತ(p, pc - p);
        r = ಸುರಕ್ಷಿತ_atoi(n, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (v < 0)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ v;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_parts_ಮೌಲ್ಯ_with_tenths_ಇರಿಸು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಚಿಹ್ನೆ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pc, *dot, *n;
        ಪೂರ್ಣಾಂಕ r, q, v;

        pc = endswith(p, ಚಿಹ್ನೆ);
        ಆದರೆ (!pc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ಆದರೆ (dot) {
                ಆದರೆ (dot + 2 != pc)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ಆದರೆ (dot[1] < '0' || dot[1] > '9')
                        ಹಿಂತಿರುಗಿ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_ಸುರಕ್ಷಿತ(p, dot - p);
        } ಇಲ್ಲದಿದ್ದರೆ {
                q = 0;
                n = strndupa_ಸುರಕ್ಷಿತ(p, pc - p);
        }
        r = ಸುರಕ್ಷಿತ_atoi(n, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (v < 0)
                ಹಿಂತಿರುಗಿ -ERANGE;
        ಆದರೆ (v > (ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - q) / 10)
                ಹಿಂತಿರುಗಿ -ERANGE;

        v = v * 10 + q;
        ಹಿಂತಿರುಗಿ v;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_parts_ಮೌಲ್ಯ_with_hundredths_ಇರಿಸು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಚಿಹ್ನೆ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pc, *dot, *n;
        ಪೂರ್ಣಾಂಕ r, q, v;

        pc = endswith(p, ಚಿಹ್ನೆ);
        ಆದರೆ (!pc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ಆದರೆ (dot) {
                ಆದರೆ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ಆದರೆ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ಹಿಂತಿರುಗಿ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ಆದರೆ (dot[1] < '0' || dot[1] > '9')
                                ಹಿಂತಿರುಗಿ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ಇಲ್ಲದಿದ್ದರೆ
                        /* We do not support zero or more than two places */
                        ಹಿಂತಿರುಗಿ -EINVAL;

                n = strndupa_ಸುರಕ್ಷಿತ(p, dot - p);
        } ಇಲ್ಲದಿದ್ದರೆ {
                q = 0;
                n = strndupa_ಸುರಕ್ಷಿತ(p, pc - p);
        }
        r = ಸುರಕ್ಷಿತ_atoi(n, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (v < 0)
                ಹಿಂತಿರುಗಿ -ERANGE;
        ಆದರೆ (v > (ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - q) / 100)
                ಹಿಂತಿರುಗಿ -ERANGE;

        v = v * 100 + q;
        ಹಿಂತಿರುಗಿ v;
}

ಪೂರ್ಣಾಂಕ parse_percent_unbounded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ parse_parts_ಮೌಲ್ಯ_ಸಂಪೂರ್ಣ(p, "%");
}

ಪೂರ್ಣಾಂಕ parse_percent(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪೂರ್ಣಾಂಕ v;

        v = parse_percent_unbounded(p);
        ಆದರೆ (v > 100)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ v;
}

ಪೂರ್ಣಾಂಕ parse_permille_unbounded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pm;

        pm = endswith(p, "‰");
        ಆದರೆ (pm)
                ಹಿಂತಿರುಗಿ parse_parts_ಮೌಲ್ಯ_ಸಂಪೂರ್ಣ(p, "‰");

        ಹಿಂತಿರುಗಿ parse_parts_ಮೌಲ್ಯ_with_tenths_ಇರಿಸು(p, "%");
}

ಪೂರ್ಣಾಂಕ parse_permille(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪೂರ್ಣಾಂಕ v;

        v = parse_permille_unbounded(p);
        ಆದರೆ (v > 1000)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ v;
}

ಪೂರ್ಣಾಂಕ parse_permyriad_unbounded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pm;

        pm = endswith(p, "‱");
        ಆದರೆ (pm)
                ಹಿಂತಿರುಗಿ parse_parts_ಮೌಲ್ಯ_ಸಂಪೂರ್ಣ(p, "‱");

        pm = endswith(p, "‰");
        ಆದರೆ (pm)
                ಹಿಂತಿರುಗಿ parse_parts_ಮೌಲ್ಯ_with_tenths_ಇರಿಸು(p, "‰");

        ಹಿಂತಿರುಗಿ parse_parts_ಮೌಲ್ಯ_with_hundredths_ಇರಿಸು(p, "%");
}

ಪೂರ್ಣಾಂಕ parse_permyriad(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪೂರ್ಣಾಂಕ v;

        v = parse_permyriad_unbounded(p);
        ಆದರೆ (v > 10000)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ v;
}
