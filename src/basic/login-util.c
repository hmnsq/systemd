/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "login-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

bool session_id_valid(ಧ್ರುವ ವರ್ಣ *id) {

        ಯದಿ (isempty(id))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ access("/run/systemd/seats/", F_OK) >= 0;
}
