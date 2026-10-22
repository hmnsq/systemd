/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "login-util.h"
#அந்தர்பாவ "string-util.h"

bool session_id_valid(மாறிலி எழுத்து *id) {

        எனில் (isempty(id))
                திரும்பு false;

        திரும்பு in_charset(id, ALPHANUMERICAL);
}

bool logind_running(வெறுமை) {
        திரும்பு access("/run/systemd/seats/", F_OK) >= 0;
}
