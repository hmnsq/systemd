/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

అక్షరం* bus_label_escape(స్థిరరాశి అక్షరం *s);
అక్షరం* bus_label_unescape_n(స్థిరరాశి అక్షరం *f, size_t l);

స్థిర అంతఃవరుస అక్షరం* bus_label_unescape(స్థిరరాశి అక్షరం *f) {
        తిరిగి bus_label_unescape_n(f, SIZE_MAX);
}
