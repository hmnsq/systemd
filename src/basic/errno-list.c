/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "errno-list.h"

स्थिर अचर संरचना errno_name* lookup_errno(पञ्जिका अचर वर्ण *str,
                                             पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "errno-from-name.inc"

पूर्णाङ्क errno_from_name(अचर वर्ण *name) {
        अचर संरचना errno_name *sc;

        प्रतिज्ञा(name);

        sc = lookup_errno(name, म_माप(name));
        यदि (!sc)
                फर्काऊ -EINVAL;

        प्रतिज्ञा(sc->id > 0);
        फर्काऊ sc->id;
}

#ifdef __GLIBC__
अचर वर्ण* errno_name_no_fallback(पूर्णाङ्क id) {
        यदि (id == 0) /* To stay in line with our implementation below.  */
                फर्काऊ रिक्त;

        फर्काऊ strerrorname_np(ABS(id));
}
#अन्यथा
#  include "errno-to-name.inc"

अचर वर्ण* errno_name_no_fallback(पूर्णाङ्क id) {
        यदि (id < 0)
                id = -id;

        यदि ((माप_प्रकार) id >= ELEMENTSOF(errno_names))
                फर्काऊ रिक्त;

        फर्काऊ errno_names[id];
}
#endif

अचर वर्ण* errno_name(पूर्णाङ्क id, वर्ण buf[स्थिर ERRNO_NAME_BUF_LEN]) {
        अचर वर्ण *a = errno_name_no_fallback(id);
        यदि (a)
                फर्काऊ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", असल(id));
        फर्काऊ buf;
}
