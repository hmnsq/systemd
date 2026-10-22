/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "stdio-util.h"

అక్షరం* asprintf_సురక్షిత(స్థిరరాశి అక్షరం *పరిమిత fmt, ...) {
        _cleanup_free_ అక్షరం *buf = ఖాళీ;
        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి ఖాళీ;
        తిరిగి TAKE_PTR(buf);
}
