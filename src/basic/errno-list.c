/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const sa_nracanaa errno_name* lookup_errno(pa_njikaa const var_na *str,
                                             pa_njikaa GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

ຈຳນວນເຕັມ errno_from_name(const var_na *name) {
        const sa_nracanaa errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        ຖ້າ (!sc)
                ສົ່ງຄືນ -EINVAL;

        assert(sc->id > 0);
        ສົ່ງຄືນ sc->id;
}

#ifdef __GLIBC__
const var_na* errno_name_no_fallback(ຈຳນວນເຕັມ id) {
        ຖ້າ (id == 0) /* To stay in line with our implementation below.  */
                ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ strerrorname_np(ABS(id));
}
#ບໍ່ດັ່ງນັ້ນ
#  include "errno-to-name.inc"

const var_na* errno_name_no_fallback(ຈຳນວນເຕັມ id) {
        ຖ້າ (id < 0)
                id = -id;

        ຖ້າ ((size_t) id >= ELEMENTSOF(errno_names))
                ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ errno_names[id];
}
#endif

const var_na* errno_name(ຈຳນວນເຕັມ id, var_na buf[static ERRNO_NAME_BUF_LEN]) {
        const var_na *a = errno_name_no_fallback(id);
        ຖ້າ (a)
                ສົ່ງຄືນ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ສົ່ງຄືນ buf;
}
