/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "parse-util.h"
#समावेश "percent-util.h"
#समावेश "string-util.h"

अटल पूर्णांक parse_parts_मूल्य_whole(स्थिर अक्षर *p, स्थिर अक्षर *प्रतीक) {
        स्थिर अक्षर *pc, *n;
        पूर्णांक r, v;

        pc = endswith(p, प्रतीक);
        अगर (!pc)
                वापस -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        अगर (r < 0)
                वापस r;
        अगर (v < 0)
                वापस -दुस्फल;

        वापस v;
}

अटल पूर्णांक parse_parts_मूल्य_with_tenths_place(स्थिर अक्षर *p, स्थिर अक्षर *प्रतीक) {
        स्थिर अक्षर *pc, *dot, *n;
        पूर्णांक r, q, v;

        pc = endswith(p, प्रतीक);
        अगर (!pc)
                वापस -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        अगर (dot) {
                अगर (dot + 2 != pc)
                        वापस -EINVAL;
                अगर (dot[1] < '0' || dot[1] > '9')
                        वापस -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        अगर (r < 0)
                वापस r;
        अगर (v < 0)
                वापस -दुस्फल;
        अगर (v > (पूर्णांक_उच्च - q) / 10)
                वापस -दुस्फल;

        v = v * 10 + q;
        वापस v;
}

अटल पूर्णांक parse_parts_मूल्य_with_hundredths_place(स्थिर अक्षर *p, स्थिर अक्षर *प्रतीक) {
        स्थिर अक्षर *pc, *dot, *n;
        पूर्णांक r, q, v;

        pc = endswith(p, प्रतीक);
        अगर (!pc)
                वापस -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        अगर (dot) {
                अगर (dot + 3 == pc) {
                        /* Support two places after the dot */

                        अगर (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                वापस -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } अन्यथा अगर (dot + 2 == pc) {
                        /* Support one place after the dot */

                        अगर (dot[1] < '0' || dot[1] > '9')
                                वापस -EINVAL;
                        q = (dot[1] - '0') * 10;
                } अन्यथा
                        /* We do not support zero or more than two places */
                        वापस -EINVAL;

                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        अगर (r < 0)
                वापस r;
        अगर (v < 0)
                वापस -दुस्फल;
        अगर (v > (पूर्णांक_उच्च - q) / 100)
                वापस -दुस्फल;

        v = v * 100 + q;
        वापस v;
}

पूर्णांक parse_percent_unbounded(स्थिर अक्षर *p) {
        वापस parse_parts_मूल्य_whole(p, "%");
}

पूर्णांक parse_percent(स्थिर अक्षर *p) {
        पूर्णांक v;

        v = parse_percent_unbounded(p);
        अगर (v > 100)
                वापस -दुस्फल;

        वापस v;
}

पूर्णांक parse_permille_unbounded(स्थिर अक्षर *p) {
        स्थिर अक्षर *pm;

        pm = endswith(p, "‰");
        अगर (pm)
                वापस parse_parts_मूल्य_whole(p, "‰");

        वापस parse_parts_मूल्य_with_tenths_place(p, "%");
}

पूर्णांक parse_permille(स्थिर अक्षर *p) {
        पूर्णांक v;

        v = parse_permille_unbounded(p);
        अगर (v > 1000)
                वापस -दुस्फल;

        वापस v;
}

पूर्णांक parse_permyriad_unbounded(स्थिर अक्षर *p) {
        स्थिर अक्षर *pm;

        pm = endswith(p, "‱");
        अगर (pm)
                वापस parse_parts_मूल्य_whole(p, "‱");

        pm = endswith(p, "‰");
        अगर (pm)
                वापस parse_parts_मूल्य_with_tenths_place(p, "‰");

        वापस parse_parts_मूल्य_with_hundredths_place(p, "%");
}

पूर्णांक parse_permyriad(स्थिर अक्षर *p) {
        पूर्णांक v;

        v = parse_permyriad_unbounded(p);
        अगर (v > 10000)
                वापस -दुस्फल;

        वापस v;
}
