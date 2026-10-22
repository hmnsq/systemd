/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "percent-util.h"
#అన్తర్భావ "string-util.h"

స్థిర పూర్ణాంకం parse_parts_విలువ_సంపూర్ణ(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *చిహ్నం) {
        స్థిరరాశి అక్షరం *pc, *n;
        పూర్ణాంకం r, v;

        pc = endswith(p, చిహ్నం);
        ఒకవేళ (!pc)
                తిరిగి -EINVAL;

        n = strndupa_సురక్షిత(p, pc - p);
        r = సురక్షిత_atoi(n, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (v < 0)
                తిరిగి -ERANGE;

        తిరిగి v;
}

స్థిర పూర్ణాంకం parse_parts_విలువ_with_tenths_ఉంచు(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *చిహ్నం) {
        స్థిరరాశి అక్షరం *pc, *dot, *n;
        పూర్ణాంకం r, q, v;

        pc = endswith(p, చిహ్నం);
        ఒకవేళ (!pc)
                తిరిగి -EINVAL;

        dot = memchr(p, '.', pc - p);
        ఒకవేళ (dot) {
                ఒకవేళ (dot + 2 != pc)
                        తిరిగి -EINVAL;
                ఒకవేళ (dot[1] < '0' || dot[1] > '9')
                        తిరిగి -EINVAL;
                q = dot[1] - '0';
                n = strndupa_సురక్షిత(p, dot - p);
        } లేకపోతే {
                q = 0;
                n = strndupa_సురక్షిత(p, pc - p);
        }
        r = సురక్షిత_atoi(n, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (v < 0)
                తిరిగి -ERANGE;
        ఒకవేళ (v > (గరిష్ఠ_పూర్ణాంకం - q) / 10)
                తిరిగి -ERANGE;

        v = v * 10 + q;
        తిరిగి v;
}

స్థిర పూర్ణాంకం parse_parts_విలువ_with_hundredths_ఉంచు(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *చిహ్నం) {
        స్థిరరాశి అక్షరం *pc, *dot, *n;
        పూర్ణాంకం r, q, v;

        pc = endswith(p, చిహ్నం);
        ఒకవేళ (!pc)
                తిరిగి -EINVAL;

        dot = memchr(p, '.', pc - p);
        ఒకవేళ (dot) {
                ఒకవేళ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ఒకవేళ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                తిరిగి -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } లేకపోతే ఒకవేళ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ఒకవేళ (dot[1] < '0' || dot[1] > '9')
                                తిరిగి -EINVAL;
                        q = (dot[1] - '0') * 10;
                } లేకపోతే
                        /* We do not support zero or more than two places */
                        తిరిగి -EINVAL;

                n = strndupa_సురక్షిత(p, dot - p);
        } లేకపోతే {
                q = 0;
                n = strndupa_సురక్షిత(p, pc - p);
        }
        r = సురక్షిత_atoi(n, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (v < 0)
                తిరిగి -ERANGE;
        ఒకవేళ (v > (గరిష్ఠ_పూర్ణాంకం - q) / 100)
                తిరిగి -ERANGE;

        v = v * 100 + q;
        తిరిగి v;
}

పూర్ణాంకం parse_percent_unbounded(స్థిరరాశి అక్షరం *p) {
        తిరిగి parse_parts_విలువ_సంపూర్ణ(p, "%");
}

పూర్ణాంకం parse_percent(స్థిరరాశి అక్షరం *p) {
        పూర్ణాంకం v;

        v = parse_percent_unbounded(p);
        ఒకవేళ (v > 100)
                తిరిగి -ERANGE;

        తిరిగి v;
}

పూర్ణాంకం parse_permille_unbounded(స్థిరరాశి అక్షరం *p) {
        స్థిరరాశి అక్షరం *pm;

        pm = endswith(p, "‰");
        ఒకవేళ (pm)
                తిరిగి parse_parts_విలువ_సంపూర్ణ(p, "‰");

        తిరిగి parse_parts_విలువ_with_tenths_ఉంచు(p, "%");
}

పూర్ణాంకం parse_permille(స్థిరరాశి అక్షరం *p) {
        పూర్ణాంకం v;

        v = parse_permille_unbounded(p);
        ఒకవేళ (v > 1000)
                తిరిగి -ERANGE;

        తిరిగి v;
}

పూర్ణాంకం parse_permyriad_unbounded(స్థిరరాశి అక్షరం *p) {
        స్థిరరాశి అక్షరం *pm;

        pm = endswith(p, "‱");
        ఒకవేళ (pm)
                తిరిగి parse_parts_విలువ_సంపూర్ణ(p, "‱");

        pm = endswith(p, "‰");
        ఒకవేళ (pm)
                తిరిగి parse_parts_విలువ_with_tenths_ఉంచు(p, "‰");

        తిరిగి parse_parts_విలువ_with_hundredths_ఉంచు(p, "%");
}

పూర్ణాంకం parse_permyriad(స్థిరరాశి అక్షరం *p) {
        పూర్ణాంకం v;

        v = parse_permyriad_unbounded(p);
        ఒకవేళ (v > 10000)
                తిరిగి -ERANGE;

        తిరిగి v;
}
