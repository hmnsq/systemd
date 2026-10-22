/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "errno-list.h"

স্থির ধ্রুবক কাঠামো errno_name* lookup_errno(নিবন্ধ ধ্রুবক অক্ষর *str,
                                             নিবন্ধ GPERF_LEN_TYPE len);

#অন্তর্ভাব "errno-from-name.inc"

পূর্ণসংখ্যা errno_from_name(ধ্রুবক অক্ষর *name) {
        ধ্রুবক কাঠামো errno_name *sc;

        প্রতিজ্ঞা(name);

        sc = lookup_errno(name, strlen(name));
        যদি (!sc)
                ফেরত -EINVAL;

        প্রতিজ্ঞা(sc->id > 0);
        ফেরত sc->id;
}

#ifdef __GLIBC__
ধ্রুবক অক্ষর* errno_name_no_fallback(পূর্ণসংখ্যা id) {
        যদি (id == 0) /* To stay in line with our implementation below.  */
                ফেরত ফাঁকা;

        ফেরত strerrorname_np(ABS(id));
}
#অন্যথা
#  include "errno-to-name.inc"

ধ্রুবক অক্ষর* errno_name_no_fallback(পূর্ণসংখ্যা id) {
        যদি (id < 0)
                id = -id;

        যদি ((size_t) id >= ELEMENTSOF(errno_names))
                ফেরত ফাঁকা;

        ফেরত errno_names[id];
}
#endif

ধ্রুবক অক্ষর* errno_name(পূর্ণসংখ্যা id, অক্ষর buf[স্থির ERRNO_NAME_BUF_LEN]) {
        ধ্রুবক অক্ষর *a = errno_name_no_fallback(id);
        যদি (a)
                ফেরত a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ফেরত buf;
}
