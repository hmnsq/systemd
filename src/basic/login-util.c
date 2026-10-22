/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "login-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

bool session_id_valid(ਅਚਲ ਅੱਖਰ *id) {

        ਜੇ (isempty(id))
                ਵਾਪਸ false;

        ਵਾਪਸ ਵਿੱਚ_charset(id, ALPHANUMERICAL);
}

bool logind_running(ਖਾਲੀ) {
        ਵਾਪਸ access("/run/systemd/seats/", F_OK) >= 0;
}
