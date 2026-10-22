/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "errno-list.h"

স্থির ধ্রুব সংরচনা errno_name* lookup_errno(পঞ্জিকা ধ্রুব বর্ণ *str,
                                             পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাব "errno-from-name.inc"

পূর্ণাঙ্ক errno_from_name(ধ্রুব বর্ণ *name) {
        ধ্রুব সংরচনা errno_name *sc;

        প্রতিজ্ঞা(name);

        sc = lookup_errno(name, strlen(name));
        যদি (!sc)
                প্রত্যাগম -EINVAL;

        প্রতিজ্ঞা(sc->id > 0);
        প্রত্যাগম sc->id;
}

#ifdef __GLIBC__
ধ্রুব বর্ণ* errno_name_no_fallback(পূর্ণাঙ্ক id) {
        যদি (id == 0) /* To stay in line with our implementation below.  */
                প্রত্যাগম রিক্ত;

        প্রত্যাগম strerrorname_np(ABS(id));
}
#অন্যথা
#  include "errno-to-name.inc"

ধ্রুব বর্ণ* errno_name_no_fallback(পূর্ণাঙ্ক id) {
        যদি (id < 0)
                id = -id;

        যদি ((size_t) id >= ELEMENTSOF(errno_names))
                প্রত্যাগম রিক্ত;

        প্রত্যাগম errno_names[id];
}
#endif

ধ্রুব বর্ণ* errno_name(পূর্ণাঙ্ক id, বর্ণ buf[স্থির ERRNO_NAME_BUF_LEN]) {
        ধ্রুব বর্ণ *a = errno_name_no_fallback(id);
        যদি (a)
                প্রত্যাগম a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        প্রত্যাগম buf;
}
