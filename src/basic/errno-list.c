/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const sa_nracanaa errno_name* lookup_errno(pa_njikaa const var_na *str,
                                             pa_njikaa GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

ကိန်းပြည့် errno_from_name(const var_na *name) {
        const sa_nracanaa errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        အကယ်၍ (!sc)
                ပြန်ပေး -EINVAL;

        assert(sc->id > 0);
        ပြန်ပေး sc->id;
}

#ifdef __GLIBC__
const var_na* errno_name_no_fallback(ကိန်းပြည့် id) {
        အကယ်၍ (id == 0) /* To stay in line with our implementation below.  */
                ပြန်ပေး NULL;

        ပြန်ပေး strerrorname_np(ABS(id));
}
#မဟုတ်ပါက
#  include "errno-to-name.inc"

const var_na* errno_name_no_fallback(ကိန်းပြည့် id) {
        အကယ်၍ (id < 0)
                id = -id;

        အကယ်၍ ((size_t) id >= ELEMENTSOF(errno_names))
                ပြန်ပေး NULL;

        ပြန်ပေး errno_names[id];
}
#endif

const var_na* errno_name(ကိန်းပြည့် id, var_na buf[static ERRNO_NAME_BUF_LEN]) {
        const var_na *a = errno_name_no_fallback(id);
        အကယ်၍ (a)
                ပြန်ပေး a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ပြန်ပေး buf;
}
