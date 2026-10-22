/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const struct errno_name* lookup_errno(register const char *str,
                                             register GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

정수 errno_from_name(const char *name) {
        const struct errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        만약 (!sc)
                반환 -EINVAL;

        assert(sc->id > 0);
        반환 sc->id;
}

#ifdef __GLIBC__
const char* errno_name_no_fallback(정수 id) {
        만약 (id == 0) /* To stay in line with our implementation below.  */
                반환 NULL;

        반환 strerrorname_np(ABS(id));
}
#아니면
#  include "errno-to-name.inc"

const char* errno_name_no_fallback(정수 id) {
        만약 (id < 0)
                id = -id;

        만약 ((size_t) id >= ELEMENTSOF(errno_names))
                반환 NULL;

        반환 errno_names[id];
}
#endif

const char* errno_name(정수 id, char buf[static ERRNO_NAME_BUF_LEN]) {
        const char *a = errno_name_no_fallback(id);
        만약 (a)
                반환 a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        반환 buf;
}
