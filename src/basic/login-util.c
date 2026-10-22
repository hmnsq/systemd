/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "login-util.h"
#અન્તર્ભાવ "string-util.h"

bool session_id_valid(અચલ અક્ષર *id) {

        જો (isempty(id))
                પાછા false;

        પાછા in_charset(id, ALPHANUMERICAL);
}

bool logind_running(શૂન્ય) {
        પાછા access("/run/systemd/seats/", F_OK) >= 0;
}
