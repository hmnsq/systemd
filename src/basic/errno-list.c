/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const struct errno_name* lookup_errno(register const char *str,
                                             register GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

цео errno_from_name(const char *name) {
        const struct errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        ако (!sc)
                врати -EINVAL;

        assert(sc->id > 0);
        врати sc->id;
}

#ifdef __GLIBC__
const char* errno_name_no_fallback(цео id) {
        ако (id == 0) /* To stay in line with our implementation below.  */
                врати NULL;

        врати strerrorname_np(ABS(id));
}
#инако
#  include "errno-to-name.inc"

const char* errno_name_no_fallback(цео id) {
        ако (id < 0)
                id = -id;

        ако ((size_t) id >= ELEMENTSOF(errno_names))
                врати NULL;

        врати errno_names[id];
}
#endif

const char* errno_name(цео id, char buf[static ERRNO_NAME_BUF_LEN]) {
        const char *a = errno_name_no_fallback(id);
        ако (a)
                врати a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        врати buf;
}
