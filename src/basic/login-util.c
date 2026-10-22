/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <unistd.h>

#అన్తర్భావ "login-util.h"
#అన్తర్భావ "string-util.h"

bool session_id_valid(స్థిరరాశి అక్షరం *id) {

        ఒకవేళ (isempty(id))
                తిరిగి false;

        తిరిగి లోపల_charset(id, ALPHANUMERICAL);
}

bool logind_running(శూన్య) {
        తిరిగి access("/run/systemd/seats/", F_OK) >= 0;
}
