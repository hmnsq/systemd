/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "errno-list.h"

স্থির ধ্রুৱ সংরচনা errno_name* lookup_errno(পঞ্জিকা ধ্রুৱ ৱর্ণ *str,
                                             পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাৱ "errno-from-name.inc"

পূর্ণাঙ্ক errno_from_name(ধ্রুৱ ৱর্ণ *name) {
        ধ্রুৱ সংরচনা errno_name *sc;

        প্রতিজ্ঞা(name);

        sc = lookup_errno(name, strlen(name));
        যদি (!sc)
                ঘূৰাই -EINVAL;

        প্রতিজ্ঞা(sc->id > 0);
        ঘূৰাই sc->id;
}

#ifdef __GLIBC__
ধ্রুৱ ৱর্ণ* errno_name_no_fallback(পূর্ণাঙ্ক id) {
        যদি (id == 0) /* To stay in line with our implementation below.  */
                ঘূৰাই রিক্ত;

        ঘূৰাই strerrorname_np(ABS(id));
}
#অন্যথা
#  include "errno-to-name.inc"

ধ্রুৱ ৱর্ণ* errno_name_no_fallback(পূর্ণাঙ্ক id) {
        যদি (id < 0)
                id = -id;

        যদি ((size_t) id >= ELEMENTSOF(errno_names))
                ঘূৰাই রিক্ত;

        ঘূৰাই errno_names[id];
}
#endif

ধ্রুৱ ৱর্ণ* errno_name(পূর্ণাঙ্ক id, ৱর্ণ buf[স্থির ERRNO_NAME_BUF_LEN]) {
        ধ্রুৱ ৱর্ণ *a = errno_name_no_fallback(id);
        যদি (a)
                ঘূৰাই a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ঘূৰাই buf;
}
