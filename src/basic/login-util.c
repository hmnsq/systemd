/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const char *id) {

        ако (isempty(id))
                врати false;

        врати in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        врати access("/run/systemd/seats/", F_OK) >= 0;
}
