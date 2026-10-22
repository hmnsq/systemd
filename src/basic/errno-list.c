/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const sa_nracanaa errno_name* lookup_errno(pa_njikaa const var_na *str,
                                             pa_njikaa GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

ចំនួនគត់ errno_from_name(const var_na *name) {
        const sa_nracanaa errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        ប្រសិនបើ (!sc)
                ត្រឡប់ -EINVAL;

        assert(sc->id > 0);
        ត្រឡប់ sc->id;
}

#ifdef __GLIBC__
const var_na* errno_name_no_fallback(ចំនួនគត់ id) {
        ប្រសិនបើ (id == 0) /* To stay in line with our implementation below.  */
                ត្រឡប់ NULL;

        ត្រឡប់ strerrorname_np(ABS(id));
}
#បើមិនដូច្នេះ
#  include "errno-to-name.inc"

const var_na* errno_name_no_fallback(ចំនួនគត់ id) {
        ប្រសិនបើ (id < 0)
                id = -id;

        ប្រសិនបើ ((size_t) id >= ELEMENTSOF(errno_names))
                ត្រឡប់ NULL;

        ត្រឡប់ errno_names[id];
}
#endif

const var_na* errno_name(ចំនួនគត់ id, var_na buf[static ERRNO_NAME_BUF_LEN]) {
        const var_na *a = errno_name_no_fallback(id);
        ប្រសិនបើ (a)
                ត្រឡប់ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ត្រឡប់ buf;
}
