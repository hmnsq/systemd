/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const struct errno_name* lookup_errno(register const char *str,
                                             register GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

整数 errno_from_name(const char *name) {
        const struct errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        如果 (!sc)
                返回 -EINVAL;

        assert(sc->id > 0);
        返回 sc->id;
}

#ifdef __GLIBC__
const char* errno_name_no_fallback(整数 id) {
        如果 (id == 0) /* To stay in line with our implementation below.  */
                返回 NULL;

        返回 strerrorname_np(ABS(id));
}
#否则
#  include "errno-to-name.inc"

const char* errno_name_no_fallback(整数 id) {
        如果 (id < 0)
                id = -id;

        如果 ((size_t) id >= ELEMENTSOF(errno_names))
                返回 NULL;

        返回 errno_names[id];
}
#endif

const char* errno_name(整数 id, char buf[static ERRNO_NAME_BUF_LEN]) {
        const char *a = errno_name_no_fallback(id);
        如果 (a)
                返回 a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        返回 buf;
}
