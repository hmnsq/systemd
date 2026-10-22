/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "errno-util.h"

స్థిరరాశి అక్షరం* strerror_or_eof(పూర్ణాంకం errnum, అక్షరం *buf, size_t buflen) {
        ఒకవేళ (errnum != 0)
                తిరిగి strerror_r(ABS(errnum), buf, buflen);

        తిరిగి "Unexpected EOF";
}
