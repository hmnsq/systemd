/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "format-util.h"

అక్షరం* format_బైట్లు_నిండిన(అక్షరం *buf, size_t l, uint64_t t, Format_బైట్లు_Flag flag) {
        రకనిర్వచనం నిర్మాణం {
                స్థిరరాశి అక్షరం *suffix;
                uint64_t గుణకం;
        } suffix_పట్టిక;
        స్థిర స్థిరరాశి suffix_పట్టిక పట్టిక_iec[] = {
                { "E", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "P", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "T", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "G", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "M", UINT64_C(1024)*UINT64_C(1024) },
                { "K", UINT64_C(1024) },
        }, పట్టిక_si[] = {
                { "E", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "P", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "T", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "G", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "M", UINT64_C(1000)*UINT64_C(1000) },
                { "K", UINT64_C(1000) },
        };
        స్థిరరాశి suffix_పట్టిక *పట్టిక;
        size_t n;

        ప్రతిజ్ఞా(buf);
        assert_cc(ELEMENTSOF(పట్టిక_iec) == ELEMENTSOF(పట్టిక_si));

        ఒకవేళ (t == UINT64_MAX)
                తిరిగి ఖాళీ;

        పట్టిక = flag & FORMAT_బైట్లు_USE_IEC ? పట్టిక_iec : పట్టిక_si;
        n = ELEMENTSOF(పట్టిక_iec);

        కొరకు (size_t i = 0; i < n; i++)
                ఒకవేళ (t >= పట్టిక[i].గుణకం) {
                        uint64_t remainder = i != n - 1 ?
                                (t / పట్టిక[i + 1].గుణకం * 10 / పట్టిక[n - 1].గుణకం) % 10 :
                                (t * 10 / పట్టిక[i].గుణకం) % 10;

                        ఒకవేళ (FLAGS_సమితి(flag, FORMAT_బైట్లు_ALWAYS_బిందువు) ||
                            (FLAGS_సమితి(flag, FORMAT_బైట్లు_క్రింద_బిందువు) && remainder > 0))
                                (శూన్య) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / పట్టిక[i].గుణకం,
                                                remainder,
                                                పట్టిక[i].suffix);
                        లేకపోతే
                                (శూన్య) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / పట్టిక[i].గుణకం,
                                                పట్టిక[i].suffix);

                        వెళ్ళు ముగింపు;
                }

        (శూన్య) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_బైట్లు_TRAILING_B ? "B" : "");

ముగింపు:
        buf[l-1] = 0;
        తిరిగి buf;
}
