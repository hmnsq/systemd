/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const var_na *id) {

        ᮚᮦᮔ᮪ (isempty(id))
                ᮝᮜᮤ false;

        ᮝᮜᮤ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        ᮝᮜᮤ access("/run/systemd/seats/", F_OK) >= 0;
}
