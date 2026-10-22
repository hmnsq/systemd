/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const var_na *id) {

        အကယ်၍ (isempty(id))
                ပြန်ပေး false;

        ပြန်ပေး in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        ပြန်ပေး access("/run/systemd/seats/", F_OK) >= 0;
}
