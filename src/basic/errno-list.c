/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "errno-list.h"

स्थिर ध्रुव संरचना errno_name* lookup_errno(पञ्जिका ध्रुव वर्ण *str,
                                             पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "errno-from-name.inc"

पूर्णाङ्क errno_from_name(ध्रुव वर्ण *name) {
        ध्रुव संरचना errno_name *sc;

        प्रतिज्ञा(name);

        sc = lookup_errno(name, strlen(name));
        जइ (!sc)
                प्रत्यागम -EINVAL;

        प्रतिज्ञा(sc->id > 0);
        प्रत्यागम sc->id;
}

#ifdef __GLIBC__
ध्रुव वर्ण* errno_name_no_fallback(पूर्णाङ्क id) {
        जइ (id == 0) /* To stay in line with our implementation below.  */
                प्रत्यागम रिक्त;

        प्रत्यागम strerrorname_np(ABS(id));
}
#अण्णहा
#  include "errno-to-name.inc"

ध्रुव वर्ण* errno_name_no_fallback(पूर्णाङ्क id) {
        जइ (id < 0)
                id = -id;

        जइ ((size_t) id >= ELEMENTSOF(errno_names))
                प्रत्यागम रिक्त;

        प्रत्यागम errno_names[id];
}
#endif

ध्रुव वर्ण* errno_name(पूर्णाङ्क id, वर्ण buf[स्थिर ERRNO_NAME_BUF_LEN]) {
        ध्रुव वर्ण *a = errno_name_no_fallback(id);
        जइ (a)
                प्रत्यागम a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        प्रत्यागम buf;
}
