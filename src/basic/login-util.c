/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "login-util.h"
#അന്തര്ഭാവ "string-util.h"

bool session_id_valid(സ്ഥിരം അക്ഷരം *id) {

        എങ്കിൽ (isempty(id))
                തിരികെ false;

        തിരികെ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(ശൂന്യം) {
        തിരികെ access("/run/systemd/seats/", F_OK) >= 0;
}
