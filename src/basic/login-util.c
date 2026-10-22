/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const char *id) {

        만약 (isempty(id))
                반환 false;

        반환 in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        반환 access("/run/systemd/seats/", F_OK) >= 0;
}
