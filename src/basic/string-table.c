/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"

స్థిరరాశి అక్షరం* string_పట్టిక_lookup_గమ్యం_string(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, ssize_t i) {
        ఒకవేళ (i < 0 || i >= (ssize_t) len)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(పట్టిక);

        తిరిగి పట్టిక[i];
}

ssize_t string_పట్టిక_lookup_from_string(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, స్థిరరాశి అక్షరం *కీలకం) {
        assert_return(పట్టిక, -EINVAL);

        ఒకవేళ (!కీలకం)
                తిరిగి -EINVAL;

        కొరకు (size_t i = 0; i < len; ++i)
                ఒకవేళ (streq_ptr(పట్టిక[i], కీలకం))
                        తిరిగి (ssize_t) i;

        తిరిగి -EINVAL;
}

ssize_t string_పట్టిక_lookup_from_string_with_boolean(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, స్థిరరాశి అక్షరం *కీలకం, ssize_t yes) {
        ఒకవేళ (!కీలకం)
                తిరిగి -EINVAL;

        పూర్ణాంకం b = parse_boolean(కీలకం);
        ఒకవేళ (b == 0)
                తిరిగి 0;
        ఒకవేళ (b > 0)
                తిరిగి yes;

        తిరిగి string_పట్టిక_lookup_from_string(పట్టిక, len, కీలకం);
}

పూర్ణాంకం string_పట్టిక_lookup_గమ్యం_string_fallback(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, ssize_t i, size_t max, అక్షరం **ret) {
        అక్షరం *s;

        ప్రతిజ్ఞా(పట్టిక);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (i < 0 || i > (ssize_t) max)
                తిరిగి -ERANGE;

        ఒకవేళ (i < (ssize_t) len && పట్టిక[i])
                s = strdup(పట్టిక[i]);
        లేకపోతే
                s = asprintf_సురక్షిత("%zd", i);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        *ret = s;
        తిరిగి 0;
}

ssize_t string_పట్టిక_lookup_from_string_fallback(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, స్థిరరాశి అక్షరం *s, size_t max) {
        ఒకవేళ (!s)
                తిరిగి -EINVAL;

        ssize_t i = string_పట్టిక_lookup_from_string(పట్టిక, len, s);
        ఒకవేళ (i >= 0)
                తిరిగి i;

        గుర్తులేని u;
        ఒకవేళ (సురక్షిత_atou(s, &u) < 0)
                తిరిగి -EINVAL;
        ఒకవేళ (u > max)
                తిరిగి -EINVAL;

        తిరిగి u;
}
