/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "login-util.h"
#ضمن "string-util.h"

bool session_id_valid(ثابت حرف *id) {

        ܐܢ (isempty(id))
                ܦܢܐ false;

        ܦܢܐ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(فراغ) {
        ܦܢܐ access("/run/systemd/seats/", F_OK) >= 0;
}
