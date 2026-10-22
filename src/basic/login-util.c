/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "login-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

bool session_id_valid(ଅଚଳ ଅକ୍ଷର *id) {

        ଯଦି (isempty(id))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ in_charset(id, ALPHANUMERICAL);
}

bool logind_running(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ access("/run/systemd/seats/", F_OK) >= 0;
}
