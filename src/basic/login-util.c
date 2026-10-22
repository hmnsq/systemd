/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "login-util.h"
#अन्तर्भाव "string-util.h"

bool session_id_valid(स्थिरांक अक्षर *id) {

        जर (isempty(id))
                परत false;

        परत in_charset(id, ALPHANUMERICAL);
}

bool logind_running(शून्य) {
        परत access("/run/systemd/seats/", F_OK) >= 0;
}
