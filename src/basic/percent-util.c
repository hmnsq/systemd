/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "percent-util.h"
#அந்தர்பாவ "string-util.h"

நிலையான முழு parse_parts_மதிப்பு_whole(மாறிலி எழுத்து *p, மாறிலி எழுத்து *குறியீடு) {
        மாறிலி எழுத்து *pc, *n;
        முழு r, v;

        pc = endswith(p, குறியீடு);
        எனில் (!pc)
                திரும்பு -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (v < 0)
                திரும்பு -ERANGE;

        திரும்பு v;
}

நிலையான முழு parse_parts_மதிப்பு_with_tenths_place(மாறிலி எழுத்து *p, மாறிலி எழுத்து *குறியீடு) {
        மாறிலி எழுத்து *pc, *dot, *n;
        முழு r, q, v;

        pc = endswith(p, குறியீடு);
        எனில் (!pc)
                திரும்பு -EINVAL;

        dot = memchr(p, '.', pc - p);
        எனில் (dot) {
                எனில் (dot + 2 != pc)
                        திரும்பு -EINVAL;
                எனில் (dot[1] < '0' || dot[1] > '9')
                        திரும்பு -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } இல்லையெனில் {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (v < 0)
                திரும்பு -ERANGE;
        எனில் (v > (INT_MAX - q) / 10)
                திரும்பு -ERANGE;

        v = v * 10 + q;
        திரும்பு v;
}

நிலையான முழு parse_parts_மதிப்பு_with_hundredths_place(மாறிலி எழுத்து *p, மாறிலி எழுத்து *குறியீடு) {
        மாறிலி எழுத்து *pc, *dot, *n;
        முழு r, q, v;

        pc = endswith(p, குறியீடு);
        எனில் (!pc)
                திரும்பு -EINVAL;

        dot = memchr(p, '.', pc - p);
        எனில் (dot) {
                எனில் (dot + 3 == pc) {
                        /* Support two places after the dot */

                        எனில் (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                திரும்பு -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } இல்லையெனில் எனில் (dot + 2 == pc) {
                        /* Support one place after the dot */

                        எனில் (dot[1] < '0' || dot[1] > '9')
                                திரும்பு -EINVAL;
                        q = (dot[1] - '0') * 10;
                } இல்லையெனில்
                        /* We do not support zero or more than two places */
                        திரும்பு -EINVAL;

                n = strndupa_safe(p, dot - p);
        } இல்லையெனில் {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (v < 0)
                திரும்பு -ERANGE;
        எனில் (v > (INT_MAX - q) / 100)
                திரும்பு -ERANGE;

        v = v * 100 + q;
        திரும்பு v;
}

முழு parse_percent_unbounded(மாறிலி எழுத்து *p) {
        திரும்பு parse_parts_மதிப்பு_whole(p, "%");
}

முழு parse_percent(மாறிலி எழுத்து *p) {
        முழு v;

        v = parse_percent_unbounded(p);
        எனில் (v > 100)
                திரும்பு -ERANGE;

        திரும்பு v;
}

முழு parse_permille_unbounded(மாறிலி எழுத்து *p) {
        மாறிலி எழுத்து *pm;

        pm = endswith(p, "‰");
        எனில் (pm)
                திரும்பு parse_parts_மதிப்பு_whole(p, "‰");

        திரும்பு parse_parts_மதிப்பு_with_tenths_place(p, "%");
}

முழு parse_permille(மாறிலி எழுத்து *p) {
        முழு v;

        v = parse_permille_unbounded(p);
        எனில் (v > 1000)
                திரும்பு -ERANGE;

        திரும்பு v;
}

முழு parse_permyriad_unbounded(மாறிலி எழுத்து *p) {
        மாறிலி எழுத்து *pm;

        pm = endswith(p, "‱");
        எனில் (pm)
                திரும்பு parse_parts_மதிப்பு_whole(p, "‱");

        pm = endswith(p, "‰");
        எனில் (pm)
                திரும்பு parse_parts_மதிப்பு_with_tenths_place(p, "‰");

        திரும்பு parse_parts_மதிப்பு_with_hundredths_place(p, "%");
}

முழு parse_permyriad(மாறிலி எழுத்து *p) {
        முழு v;

        v = parse_permyriad_unbounded(p);
        எனில் (v > 10000)
                திரும்பு -ERANGE;

        திரும்பு v;
}
