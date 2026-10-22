/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const char *id) {

        если (isempty(id))
                вернуть false;

        вернуть in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        вернуть access("/run/systemd/seats/", F_OK) >= 0;
}
