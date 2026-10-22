/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "login-util.h"
#অন্তর্ভাব "string-util.h"

bool session_id_valid(ধ্রুবক অক্ষর *id) {

        যদি (isempty(id))
                ফেরত false;

        ফেরত in_charset(id, ALPHANUMERICAL);
}

bool logind_running(শূন্য) {
        ফেরত access("/run/systemd/seats/", F_OK) >= 0;
}
