/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const char *id) {

        如果 (isempty(id))
                返回 false;

        返回 in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        返回 access("/run/systemd/seats/", F_OK) >= 0;
}
