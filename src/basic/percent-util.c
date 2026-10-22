/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "percent-util.h"
#अन्तर्भाव "string-util.h"

स्थिर पूर्णाङ्क parse_parts_मान_whole(अचर वर्ण *p, अचर वर्ण *चिह्न) {
        अचर वर्ण *pc, *n;
        पूर्णाङ्क r, v;

        pc = endswith(p, चिह्न);
        यदि (!pc)
                फर्काऊ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        यदि (r < 0)
                फर्काऊ r;
        यदि (v < 0)
                फर्काऊ -दुस्फल;

        फर्काऊ v;
}

स्थिर पूर्णाङ्क parse_parts_मान_with_tenths_place(अचर वर्ण *p, अचर वर्ण *चिह्न) {
        अचर वर्ण *pc, *dot, *n;
        पूर्णाङ्क r, q, v;

        pc = endswith(p, चिह्न);
        यदि (!pc)
                फर्काऊ -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        यदि (dot) {
                यदि (dot + 2 != pc)
                        फर्काऊ -EINVAL;
                यदि (dot[1] < '0' || dot[1] > '9')
                        फर्काऊ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        यदि (r < 0)
                फर्काऊ r;
        यदि (v < 0)
                फर्काऊ -दुस्फल;
        यदि (v > (पूर्णांक_उच्च - q) / 10)
                फर्काऊ -दुस्फल;

        v = v * 10 + q;
        फर्काऊ v;
}

स्थिर पूर्णाङ्क parse_parts_मान_with_hundredths_place(अचर वर्ण *p, अचर वर्ण *चिह्न) {
        अचर वर्ण *pc, *dot, *n;
        पूर्णाङ्क r, q, v;

        pc = endswith(p, चिह्न);
        यदि (!pc)
                फर्काऊ -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        यदि (dot) {
                यदि (dot + 3 == pc) {
                        /* Support two places after the dot */

                        यदि (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                फर्काऊ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } अन्यथा यदि (dot + 2 == pc) {
                        /* Support one place after the dot */

                        यदि (dot[1] < '0' || dot[1] > '9')
                                फर्काऊ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } अन्यथा
                        /* We do not support zero or more than two places */
                        फर्काऊ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        यदि (r < 0)
                फर्काऊ r;
        यदि (v < 0)
                फर्काऊ -दुस्फल;
        यदि (v > (पूर्णांक_उच्च - q) / 100)
                फर्काऊ -दुस्फल;

        v = v * 100 + q;
        फर्काऊ v;
}

पूर्णाङ्क parse_percent_unbounded(अचर वर्ण *p) {
        फर्काऊ parse_parts_मान_whole(p, "%");
}

पूर्णाङ्क parse_percent(अचर वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_percent_unbounded(p);
        यदि (v > 100)
                फर्काऊ -दुस्फल;

        फर्काऊ v;
}

पूर्णाङ्क parse_permille_unbounded(अचर वर्ण *p) {
        अचर वर्ण *pm;

        pm = endswith(p, "‰");
        यदि (pm)
                फर्काऊ parse_parts_मान_whole(p, "‰");

        फर्काऊ parse_parts_मान_with_tenths_place(p, "%");
}

पूर्णाङ्क parse_permille(अचर वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_permille_unbounded(p);
        यदि (v > 1000)
                फर्काऊ -दुस्फल;

        फर्काऊ v;
}

पूर्णाङ्क parse_permyriad_unbounded(अचर वर्ण *p) {
        अचर वर्ण *pm;

        pm = endswith(p, "‱");
        यदि (pm)
                फर्काऊ parse_parts_मान_whole(p, "‱");

        pm = endswith(p, "‰");
        यदि (pm)
                फर्काऊ parse_parts_मान_with_tenths_place(p, "‰");

        फर्काऊ parse_parts_मान_with_hundredths_place(p, "%");
}

पूर्णाङ्क parse_permyriad(अचर वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_permyriad_unbounded(p);
        यदि (v > 10000)
                फर्काऊ -दुस्फल;

        फर्काऊ v;
}
