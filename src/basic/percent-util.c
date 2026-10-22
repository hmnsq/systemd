/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "percent-util.h"
#अन्तर्भाव "string-util.h"

स्थिर पूर्णाङ्क parse_parts_मानम्_whole(ध्रुव वर्ण *p, ध्रुव वर्ण *चिह्नम्) {
        ध्रुव वर्ण *pc, *n;
        पूर्णाङ्क r, v;

        pc = endswith(p, चिह्नम्);
        सचे (!pc)
                पटिनिवत्त -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (v < 0)
                पटिनिवत्त -ERANGE;

        पटिनिवत्त v;
}

स्थिर पूर्णाङ्क parse_parts_मानम्_with_tenths_place(ध्रुव वर्ण *p, ध्रुव वर्ण *चिह्नम्) {
        ध्रुव वर्ण *pc, *dot, *n;
        पूर्णाङ्क r, q, v;

        pc = endswith(p, चिह्नम्);
        सचे (!pc)
                पटिनिवत्त -EINVAL;

        dot = memchr(p, '.', pc - p);
        सचे (dot) {
                सचे (dot + 2 != pc)
                        पटिनिवत्त -EINVAL;
                सचे (dot[1] < '0' || dot[1] > '9')
                        पटिनिवत्त -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } अञ्ञथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (v < 0)
                पटिनिवत्त -ERANGE;
        सचे (v > (INT_MAX - q) / 10)
                पटिनिवत्त -ERANGE;

        v = v * 10 + q;
        पटिनिवत्त v;
}

स्थिर पूर्णाङ्क parse_parts_मानम्_with_hundredths_place(ध्रुव वर्ण *p, ध्रुव वर्ण *चिह्नम्) {
        ध्रुव वर्ण *pc, *dot, *n;
        पूर्णाङ्क r, q, v;

        pc = endswith(p, चिह्नम्);
        सचे (!pc)
                पटिनिवत्त -EINVAL;

        dot = memchr(p, '.', pc - p);
        सचे (dot) {
                सचे (dot + 3 == pc) {
                        /* Support two places after the dot */

                        सचे (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                पटिनिवत्त -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } अञ्ञथा सचे (dot + 2 == pc) {
                        /* Support one place after the dot */

                        सचे (dot[1] < '0' || dot[1] > '9')
                                पटिनिवत्त -EINVAL;
                        q = (dot[1] - '0') * 10;
                } अञ्ञथा
                        /* We do not support zero or more than two places */
                        पटिनिवत्त -EINVAL;

                n = strndupa_safe(p, dot - p);
        } अञ्ञथा {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (v < 0)
                पटिनिवत्त -ERANGE;
        सचे (v > (INT_MAX - q) / 100)
                पटिनिवत्त -ERANGE;

        v = v * 100 + q;
        पटिनिवत्त v;
}

पूर्णाङ्क parse_percent_unbounded(ध्रुव वर्ण *p) {
        पटिनिवत्त parse_parts_मानम्_whole(p, "%");
}

पूर्णाङ्क parse_percent(ध्रुव वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_percent_unbounded(p);
        सचे (v > 100)
                पटिनिवत्त -ERANGE;

        पटिनिवत्त v;
}

पूर्णाङ्क parse_permille_unbounded(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *pm;

        pm = endswith(p, "‰");
        सचे (pm)
                पटिनिवत्त parse_parts_मानम्_whole(p, "‰");

        पटिनिवत्त parse_parts_मानम्_with_tenths_place(p, "%");
}

पूर्णाङ्क parse_permille(ध्रुव वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_permille_unbounded(p);
        सचे (v > 1000)
                पटिनिवत्त -ERANGE;

        पटिनिवत्त v;
}

पूर्णाङ्क parse_permyriad_unbounded(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *pm;

        pm = endswith(p, "‱");
        सचे (pm)
                पटिनिवत्त parse_parts_मानम्_whole(p, "‱");

        pm = endswith(p, "‰");
        सचे (pm)
                पटिनिवत्त parse_parts_मानम्_with_tenths_place(p, "‰");

        पटिनिवत्त parse_parts_मानम्_with_hundredths_place(p, "%");
}

पूर्णाङ्क parse_permyriad(ध्रुव वर्ण *p) {
        पूर्णाङ्क v;

        v = parse_permyriad_unbounded(p);
        सचे (v > 10000)
                पटिनिवत्त -ERANGE;

        पटिनिवत्त v;
}
