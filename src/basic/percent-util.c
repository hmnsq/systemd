/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "percent-util.h"
#अन्तर्भाव "string-util.h"

स्थिर पूर्णाङ्क parse_parts_मूल्य_whole(ध्रुव वर्ण *p, ध्रुव वर्ण *प्रतीक) {
        ध्रुव वर्ण *pc, *n;
        पूर्णाङ्क r, v;

        pc = endswith(p, प्रतीक);
        यदि (!pc)
                प्रत्यागम -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (v < 0)
                प्रत्यागम -दुस्फल;

        प्रत्यागम v;
}

स्थिर पूर्णाङ्क parse_parts_मूल्य_with_tenths_place(ध्रुव वर्ण *p, ध्रुव वर्ण *प्रतीक) {
        ध्रुव वर्ण *pc, *dot, *n;
        पूर्णाङ्क r, q, v;

        pc = endswith(p, प्रतीक);
        यदि (!pc)
                प्रत्यागम -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        यदि (dot) {
                यदि (dot + 2 != pc)
                        प्रत्यागम -EINVAL;
                यदि (dot[1] < '0' || dot[1] > '9')
                        प्रत्यागम -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (v < 0)
                प्रत्यागम -दुस्फल;
        यदि (v > (पूर्णांक_उच्च - q) / 10)
                प्रत्यागम -दुस्फल;

        v = v * 10 + q;
        प्रत्यागम v;
}

स्थिर पूर्णाङ्क parse_parts_मूल्य_with_hundredths_place(ध्रुव वर्ण *p, ध्रुव वर्ण *प्रतीक) {
        ध्रुव वर्ण *pc, *dot, *n;
        पूर्णाङ्क r, q, v;

        pc = endswith(p, प्रतीक);
        यदि (!pc)
                प्रत्यागम -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        यदि (dot) {
                यदि (dot + 3 == pc) {
                        /* Support two places after the dot */

                        यदि (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                प्रत्यागम -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } अन्यथा यदि (dot + 2 == pc) {
                        /* Support one place after the dot */

                        यदि (dot[1] < '0' || dot[1] > '9')
                                प्रत्यागम -EINVAL;
                        q = (dot[1] - '0') * 10;
                } अन्यथा
                        /* We do not support zero or more than two places */
                        प्रत्यागम -EINVAL;

                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (v < 0)
                प्रत्यागम -दुस्फल;
        यदि (v > (पूर्णांक_उच्च - q) / 100)
                प्रत्यागम -दुस्फल;

        v = v * 100 + q;
        प्रत्यागम v;
}

पूर्णाङ्क parse_percent_unbounded(ध्रुव वर्ण *p) {
        प्रत्यागम parse_parts_मूल्य_whole(p, "%");
}

पूर्णाङ्क parse_percent(ध्रुव वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_percent_unbounded(p);
        यदि (v > 100)
                प्रत्यागम -दुस्फल;

        प्रत्यागम v;
}

पूर्णाङ्क parse_permille_unbounded(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *pm;

        pm = endswith(p, "‰");
        यदि (pm)
                प्रत्यागम parse_parts_मूल्य_whole(p, "‰");

        प्रत्यागम parse_parts_मूल्य_with_tenths_place(p, "%");
}

पूर्णाङ्क parse_permille(ध्रुव वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_permille_unbounded(p);
        यदि (v > 1000)
                प्रत्यागम -दुस्फल;

        प्रत्यागम v;
}

पूर्णाङ्क parse_permyriad_unbounded(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *pm;

        pm = endswith(p, "‱");
        यदि (pm)
                प्रत्यागम parse_parts_मूल्य_whole(p, "‱");

        pm = endswith(p, "‰");
        यदि (pm)
                प्रत्यागम parse_parts_मूल्य_with_tenths_place(p, "‰");

        प्रत्यागम parse_parts_मूल्य_with_hundredths_place(p, "%");
}

पूर्णाङ्क parse_permyriad(ध्रुव वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_permyriad_unbounded(p);
        यदि (v > 10000)
                प्रत्यागम -दुस्फल;

        प्रत्यागम v;
}
