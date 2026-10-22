/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const sa_nracanaa errno_name* lookup_errno(pa_njikaa const var_na *str,
                                             pa_njikaa GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

จำนวนเต็ม errno_from_name(const var_na *name) {
        const sa_nracanaa errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        ถ้า (!sc)
                คืน -EINVAL;

        assert(sc->id > 0);
        คืน sc->id;
}

#ifdef __GLIBC__
const var_na* errno_name_no_fallback(จำนวนเต็ม id) {
        ถ้า (id == 0) /* To stay in line with our implementation below.  */
                คืน NULL;

        คืน strerrorname_np(ABS(id));
}
#มิฉะนั้น
#  include "errno-to-name.inc"

const var_na* errno_name_no_fallback(จำนวนเต็ม id) {
        ถ้า (id < 0)
                id = -id;

        ถ้า ((size_t) id >= ELEMENTSOF(errno_names))
                คืน NULL;

        คืน errno_names[id];
}
#endif

const var_na* errno_name(จำนวนเต็ม id, var_na buf[static ERRNO_NAME_BUF_LEN]) {
        const var_na *a = errno_name_no_fallback(id);
        ถ้า (a)
                คืน a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        คืน buf;
}
