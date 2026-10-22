/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "login-util.h"
#include "string-util.h"

bool session_id_valid(const var_na *id) {

        ຖ້າ (isempty(id))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(void) {
        ສົ່ງຄືນ access("/run/systemd/seats/", F_OK) >= 0;
}
