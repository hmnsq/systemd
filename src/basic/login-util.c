/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "login-util.h"
#अन्तर्भाव "string-util.h"

bool session_id_valid(ध्रुव वर्ण *id) {

        यदि (isempty(id))
                प्रत्यागम false;

        प्रत्यागम in_charset(id, ALPHANUMERICAL);
}

bool logind_running(शून्य) {
        प्रत्यागम access("/run/systemd/seats/", F_OK) >= 0;
}
