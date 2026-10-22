/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "errno-list.h"

स्थिर स्थिरांक संरचना errno_name* lookup_errno(नोंदणी स्थिरांक अक्षर *str,
                                             नोंदणी GPERF_LEN_TYPE len);

#अन्तर्भाव "errno-from-name.inc"

पूर्णांक errno_from_name(स्थिरांक अक्षर *name) {
        स्थिरांक संरचना errno_name *sc;

        प्रतिज्ञा(name);

        sc = lookup_errno(name, म_माप(name));
        जर (!sc)
                परत -EINVAL;

        प्रतिज्ञा(sc->id > 0);
        परत sc->id;
}

#ifdef __GLIBC__
स्थिरांक अक्षर* errno_name_no_fallback(पूर्णांक id) {
        जर (id == 0) /* To stay in line with our implementation below.  */
                परत रिक्त;

        परत strerrorname_np(ABS(id));
}
#अन्यथा
#  include "errno-to-name.inc"

स्थिरांक अक्षर* errno_name_no_fallback(पूर्णांक id) {
        जर (id < 0)
                id = -id;

        जर ((माप_प्रकार) id >= ELEMENTSOF(errno_names))
                परत रिक्त;

        परत errno_names[id];
}
#endif

स्थिरांक अक्षर* errno_name(पूर्णांक id, अक्षर buf[स्थिर ERRNO_NAME_BUF_LEN]) {
        स्थिरांक अक्षर *a = errno_name_no_fallback(id);
        जर (a)
                परत a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", असल(id));
        परत buf;
}
