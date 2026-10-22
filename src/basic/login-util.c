/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const var_na *id) {

        ប្រសិនបើ (isempty(id))
                ត្រឡប់ false;

        ត្រឡប់ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        ត្រឡប់ access("/run/systemd/seats/", F_OK) >= 0;
}
