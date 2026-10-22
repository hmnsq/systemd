/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "errno-list.h"

ସ୍ଥିର ଅଚଳ ସଂରଚନା errno_name* lookup_errno(ପଞ୍ଜିକରଣ ଅଚଳ ଅକ୍ଷର *str,
                                             ପଞ୍ଜିକରଣ GPERF_LEN_TYPE len);

#ଅନ୍ତର୍ଭାଵ "errno-from-name.inc"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_from_name(ଅଚଳ ଅକ୍ଷର *name) {
        ଅଚଳ ସଂରଚନା errno_name *sc;

        ପ୍ରତିଜ୍ଞା(name);

        sc = lookup_errno(name, strlen(name));
        ଯଦି (!sc)
                ଫେରନ୍ତୁ -EINVAL;

        ପ୍ରତିଜ୍ଞା(sc->id > 0);
        ଫେରନ୍ତୁ sc->id;
}

#ifdef __GLIBC__
ଅଚଳ ଅକ୍ଷର* errno_name_no_fallback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) {
        ଯଦି (id == 0) /* To stay in line with our implementation below.  */
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ strerrorname_np(ABS(id));
}
#ଅନ୍ୟଥା
#  include "errno-to-name.inc"

ଅଚଳ ଅକ୍ଷର* errno_name_no_fallback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) {
        ଯଦି (id < 0)
                id = -id;

        ଯଦି ((size_t) id >= ELEMENTSOF(errno_names))
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ errno_names[id];
}
#endif

ଅଚଳ ଅକ୍ଷର* errno_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ଅକ୍ଷର buf[ସ୍ଥିର ERRNO_NAME_BUF_LEN]) {
        ଅଚଳ ଅକ୍ଷର *a = errno_name_no_fallback(id);
        ଯଦି (a)
                ଫେରନ୍ତୁ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ଫେରନ୍ତୁ buf;
}
