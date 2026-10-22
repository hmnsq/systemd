/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const struct errno_name* lookup_errno(register const char *str,
                                             register GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

целое errno_from_name(const char *name) {
        const struct errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        если (!sc)
                вернуть -EINVAL;

        assert(sc->id > 0);
        вернуть sc->id;
}

#ifdef __GLIBC__
const char* errno_name_no_fallback(целое id) {
        если (id == 0) /* To stay in line with our implementation below.  */
                вернуть NULL;

        вернуть strerrorname_np(ABS(id));
}
#иначе
#  include "errno-to-name.inc"

const char* errno_name_no_fallback(целое id) {
        если (id < 0)
                id = -id;

        если ((size_t) id >= ELEMENTSOF(errno_names))
                вернуть NULL;

        вернуть errno_names[id];
}
#endif

const char* errno_name(целое id, char buf[static ERRNO_NAME_BUF_LEN]) {
        const char *a = errno_name_no_fallback(id);
        если (a)
                вернуть a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        вернуть buf;
}
