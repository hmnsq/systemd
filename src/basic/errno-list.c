/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const struct errno_name* lookup_errno(register const char *str,
                                             register GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

ціле errno_from_name(const char *name) {
        const struct errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        якщо (!sc)
                повернути -EINVAL;

        assert(sc->id > 0);
        повернути sc->id;
}

#ifdef __GLIBC__
const char* errno_name_no_fallback(ціле id) {
        якщо (id == 0) /* To stay in line with our implementation below.  */
                повернути NULL;

        повернути strerrorname_np(ABS(id));
}
#інакше
#  include "errno-to-name.inc"

const char* errno_name_no_fallback(ціле id) {
        якщо (id < 0)
                id = -id;

        якщо ((size_t) id >= ELEMENTSOF(errno_names))
                повернути NULL;

        повернути errno_names[id];
}
#endif

const char* errno_name(ціле id, char buf[static ERRNO_NAME_BUF_LEN]) {
        const char *a = errno_name_no_fallback(id);
        якщо (a)
                повернути a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        повернути buf;
}
