/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "login-util.h"
#ضمن "string-util.h"

bool session_id_valid(ثابت حرف *id) {

        إذا (isempty(id))
                إرجاع false;

        إرجاع in_charset(id, ALPHANUMERICAL);
}

bool logind_running(فراغ) {
        إرجاع access("/run/systemd/seats/", F_OK) >= 0;
}
