/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "login-util.h"
#অন্তর্ভাব "string-util.h"

bool session_id_valid(ধ্রুব বর্ণ *id) {

        যদি (isempty(id))
                প্রত্যাগম false;

        প্রত্যাগম in_charset(id, ALPHANUMERICAL);
}

bool logind_running(শূন্য) {
        প্রত্যাগম access("/run/systemd/seats/", F_OK) >= 0;
}
