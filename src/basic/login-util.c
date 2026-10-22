/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "login-util.h"
#অন্তর্ভাৱ "string-util.h"

bool session_id_valid(ধ্রুৱ ৱর্ণ *id) {

        যদি (isempty(id))
                ঘূৰাই false;

        ঘূৰাই in_charset(id, ALPHANUMERICAL);
}

bool logind_running(শূন্য) {
        ঘূৰাই access("/run/systemd/seats/", F_OK) >= 0;
}
