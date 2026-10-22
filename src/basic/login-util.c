/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "login-util.h"
#شامل "string-util.h"

bool session_id_valid(ثابت حرف *id) {

        اگر (isempty(id))
                واپس false;

        واپس اندر_charset(id, ALPHANUMERICAL);
}

bool logind_running(خالی) {
        واپس access("/run/systemd/seats/", F_OK) >= 0;
}
