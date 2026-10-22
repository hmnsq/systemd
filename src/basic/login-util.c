/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const char *id) {

        ако (isempty(id))
                върни false;

        върни in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        върни access("/run/systemd/seats/", F_OK) >= 0;
}
