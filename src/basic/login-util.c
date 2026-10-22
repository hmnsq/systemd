/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const var_na *id) {

        නම් (isempty(id))
                ආපසු false;

        ආපසු in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        ආපසු access("/run/systemd/seats/", F_OK) >= 0;
}
