/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "errno-list.h"

స్థిర స్థిరరాశి నిర్మాణం errno_name* lookup_errno(నమోదు స్థిరరాశి అక్షరం *str,
                                             నమోదు GPERF_LEN_TYPE len);

#అన్తర్భావ "errno-from-name.inc"

పూర్ణాంకం errno_from_name(స్థిరరాశి అక్షరం *name) {
        స్థిరరాశి నిర్మాణం errno_name *sc;

        ప్రతిజ్ఞా(name);

        sc = lookup_errno(name, అక్షరశ్రేణి_పొడవు(name));
        ఒకవేళ (!sc)
                తిరిగి -EINVAL;

        ప్రతిజ్ఞా(sc->id > 0);
        తిరిగి sc->id;
}

#నిర్వచితమైతే __GLIBC__
స్థిరరాశి అక్షరం* errno_name_no_fallback(పూర్ణాంకం id) {
        ఒకవేళ (id == 0) /* To stay in line with our implementation below.  */
                తిరిగి ఖాళీ;

        తిరిగి strerrorname_np(ABS(id));
}
#లేకపోతే
#  include "errno-to-name.inc"

స్థిరరాశి అక్షరం* errno_name_no_fallback(పూర్ణాంకం id) {
        ఒకవేళ (id < 0)
                id = -id;

        ఒకవేళ ((size_t) id >= ELEMENTSOF(errno_పేర్లు))
                తిరిగి ఖాళీ;

        తిరిగి errno_పేర్లు[id];
}
#అయితే_ముగింపు

స్థిరరాశి అక్షరం* errno_name(పూర్ణాంకం id, అక్షరం buf[స్థిర ERRNO_NAME_BUF_LEN]) {
        స్థిరరాశి అక్షరం *a = errno_name_no_fallback(id);
        ఒకవేళ (a)
                తిరిగి a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", నిరపేక్ష(id));
        తిరిగి buf;
}
