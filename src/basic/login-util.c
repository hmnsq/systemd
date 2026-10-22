/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "login-util.h"
#अन्तर्भाव "string-util.h"

bool session_id_valid(अचर वर्ण *id) {

        यदि (isempty(id))
                फर्काऊ false;

        फर्काऊ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(शून्य) {
        फर्काऊ access("/run/systemd/seats/", F_OK) >= 0;
}
