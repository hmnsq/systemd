/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "percent-util.h"
#अन्तर्भाव "string-util.h"

स्थिर पूर्णांक parse_parts_मूल्य_whole(स्थिरांक अक्षर *p, स्थिरांक अक्षर *चिन्ह) {
        स्थिरांक अक्षर *pc, *n;
        पूर्णांक r, v;

        pc = endswith(p, चिन्ह);
        जर (!pc)
                परत -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        जर (r < 0)
                परत r;
        जर (v < 0)
                परत -दुस्फल;

        परत v;
}

स्थिर पूर्णांक parse_parts_मूल्य_with_tenths_place(स्थिरांक अक्षर *p, स्थिरांक अक्षर *चिन्ह) {
        स्थिरांक अक्षर *pc, *dot, *n;
        पूर्णांक r, q, v;

        pc = endswith(p, चिन्ह);
        जर (!pc)
                परत -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        जर (dot) {
                जर (dot + 2 != pc)
                        परत -EINVAL;
                जर (dot[1] < '0' || dot[1] > '9')
                        परत -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        जर (r < 0)
                परत r;
        जर (v < 0)
                परत -दुस्फल;
        जर (v > (पूर्णांक_उच्च - q) / 10)
                परत -दुस्फल;

        v = v * 10 + q;
        परत v;
}

स्थिर पूर्णांक parse_parts_मूल्य_with_hundredths_place(स्थिरांक अक्षर *p, स्थिरांक अक्षर *चिन्ह) {
        स्थिरांक अक्षर *pc, *dot, *n;
        पूर्णांक r, q, v;

        pc = endswith(p, चिन्ह);
        जर (!pc)
                परत -EINVAL;

        dot = स_प्रथम(p, '.', pc - p);
        जर (dot) {
                जर (dot + 3 == pc) {
                        /* Support two places after the dot */

                        जर (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                परत -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } अन्यथा जर (dot + 2 == pc) {
                        /* Support one place after the dot */

                        जर (dot[1] < '0' || dot[1] > '9')
                                परत -EINVAL;
                        q = (dot[1] - '0') * 10;
                } अन्यथा
                        /* We do not support zero or more than two places */
                        परत -EINVAL;

                n = strndupa_safe(p, dot - p);
        } अन्यथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        जर (r < 0)
                परत r;
        जर (v < 0)
                परत -दुस्फल;
        जर (v > (पूर्णांक_उच्च - q) / 100)
                परत -दुस्फल;

        v = v * 100 + q;
        परत v;
}

पूर्णांक parse_percent_unbounded(स्थिरांक अक्षर *p) {
        परत parse_parts_मूल्य_whole(p, "%");
}

पूर्णांक parse_percent(स्थिरांक अक्षर *p) {
        पूर्णांक v;

        v = parse_percent_unbounded(p);
        जर (v > 100)
                परत -दुस्फल;

        परत v;
}

पूर्णांक parse_permille_unbounded(स्थिरांक अक्षर *p) {
        स्थिरांक अक्षर *pm;

        pm = endswith(p, "‰");
        जर (pm)
                परत parse_parts_मूल्य_whole(p, "‰");

        परत parse_parts_मूल्य_with_tenths_place(p, "%");
}

पूर्णांक parse_permille(स्थिरांक अक्षर *p) {
        पूर्णांक v;

        v = parse_permille_unbounded(p);
        जर (v > 1000)
                परत -दुस्फल;

        परत v;
}

पूर्णांक parse_permyriad_unbounded(स्थिरांक अक्षर *p) {
        स्थिरांक अक्षर *pm;

        pm = endswith(p, "‱");
        जर (pm)
                परत parse_parts_मूल्य_whole(p, "‱");

        pm = endswith(p, "‰");
        जर (pm)
                परत parse_parts_मूल्य_with_tenths_place(p, "‰");

        परत parse_parts_मूल्य_with_hundredths_place(p, "%");
}

पूर्णांक parse_permyriad(स्थिरांक अक्षर *p) {
        पूर्णांक v;

        v = parse_permyriad_unbounded(p);
        जर (v > 10000)
                परत -दुस्फल;

        परत v;
}
