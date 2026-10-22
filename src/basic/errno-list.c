/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const sa_nracanaa errno_name* lookup_errno(pa_njikaa const var_na *str,
                                             pa_njikaa GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

ཧྲིལ errno_from_name(const var_na *name) {
        const sa_nracanaa errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        གལ་ཏེ (!sc)
                ཕྱིར -EINVAL;

        assert(sc->id > 0);
        ཕྱིར sc->id;
}

#ifdef __GLIBC__
const var_na* errno_name_no_fallback(ཧྲིལ id) {
        གལ་ཏེ (id == 0) /* To stay in line with our implementation below.  */
                ཕྱིར NULL;

        ཕྱིར strerrorname_np(ABS(id));
}
#དེ་མིན
#  include "errno-to-name.inc"

const var_na* errno_name_no_fallback(ཧྲིལ id) {
        གལ་ཏེ (id < 0)
                id = -id;

        གལ་ཏེ ((size_t) id >= ELEMENTSOF(errno_names))
                ཕྱིར NULL;

        ཕྱིར errno_names[id];
}
#endif

const var_na* errno_name(ཧྲིལ id, var_na buf[static ERRNO_NAME_BUF_LEN]) {
        const var_na *a = errno_name_no_fallback(id);
        གལ་ཏེ (a)
                ཕྱིར a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ཕྱིར buf;
}
