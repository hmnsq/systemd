/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "login-util.h"
#شامل "string-util.h"

bool session_id_valid(ثابت نویسه *id) {

        اگر (isempty(id))
                بازگشت false;

        بازگشت in_charset(id, ALPHANUMERICAL);
}

bool logind_running(تهی) {
        بازگشت access("/run/systemd/seats/", F_OK) >= 0;
}
