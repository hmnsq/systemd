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
        सचे (!sc)
                पटिनिवत्त -EINVAL;

        प्रतिज्ञा(sc->id > 0);
        पटिनिवत्त sc->id;
}

#ifdef __GLIBC__
ध्रुव वर्ण* errno_name_no_fallback(पूर्णाङ्क id) {
        सचे (id == 0) /* To stay in line with our implementation below.  */
                पटिनिवत्त रिक्त;

        पटिनिवत्त strerrorname_np(ABS(id));
}
#अञ्ञथा
#  include "errno-to-name.inc"

ध्रुव वर्ण* errno_name_no_fallback(पूर्णाङ्क id) {
        सचे (id < 0)
                id = -id;

        सचे ((size_t) id >= ELEMENTSOF(errno_names))
                पटिनिवत्त रिक्त;

        पटिनिवत्त errno_names[id];
}
#endif

ध्रुव वर्ण* errno_name(पूर्णाङ्क id, वर्ण buf[स्थिर ERRNO_NAME_BUF_LEN]) {
        ध्रुव वर्ण *a = errno_name_no_fallback(id);
        सचे (a)
                पटिनिवत्त a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        पटिनिवत्त buf;
}
