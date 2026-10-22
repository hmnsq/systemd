/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const struct errno_name* lookup_errno(register const char *str,
                                             register GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

số_nguyên errno_from_name(const char *name) {
        const struct errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        nếu (!sc)
                trả_về -EINVAL;

        assert(sc->id > 0);
        trả_về sc->id;
}

#ifdef __GLIBC__
const char* errno_name_no_fallback(số_nguyên id) {
        nếu (id == 0) /* To stay in line with our implementation below.  */
                trả_về NULL;

        trả_về strerrorname_np(ABS(id));
}
#không_thì
#  include "errno-to-name.inc"

const char* errno_name_no_fallback(số_nguyên id) {
        nếu (id < 0)
                id = -id;

        nếu ((size_t) id >= ELEMENTSOF(errno_names))
                trả_về NULL;

        trả_về errno_names[id];
}
#endif

const char* errno_name(số_nguyên id, char buf[static ERRNO_NAME_BUF_LEN]) {
        const char *a = errno_name_no_fallback(id);
        nếu (a)
                trả_về a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        trả_về buf;
}
