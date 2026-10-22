/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <unistd.h>

#समावेश "login-util.h"
#समावेश "string-util.h"

bool session_id_valid(स्थिर अक्षर *id) {

        अगर (isempty(id))
                वापस false;

        वापस in_charset(id, ALPHANUMERICAL);
}

bool logind_running(व्योम) {
        वापस access("/run/systemd/seats/", F_OK) >= 0;
}
