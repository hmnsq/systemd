/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "login-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

bool session_id_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id) {

        ಆದರೆ (isempty(id))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ಒಳ_charset(id, ALPHANUMERICAL);
}

bool logind_running(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ access("/run/systemd/seats/", F_OK) >= 0;
}
