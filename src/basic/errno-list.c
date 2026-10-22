/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const sa_nracanaa errno_name* lookup_errno(pa_njikaa const var_na *str,
                                             pa_njikaa GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

පූර්ණ_සංඛ්‍යා errno_from_name(const var_na *name) {
        const sa_nracanaa errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        නම් (!sc)
                ආපසු -EINVAL;

        assert(sc->id > 0);
        ආපසු sc->id;
}

#ifdef __GLIBC__
const var_na* errno_name_no_fallback(පූර්ණ_සංඛ්‍යා id) {
        නම් (id == 0) /* To stay in line with our implementation below.  */
                ආපසු NULL;

        ආපසු strerrorname_np(ABS(id));
}
#නැතහොත්
#  include "errno-to-name.inc"

const var_na* errno_name_no_fallback(පූර්ණ_සංඛ්‍යා id) {
        නම් (id < 0)
                id = -id;

        නම් ((size_t) id >= ELEMENTSOF(errno_names))
                ආපසු NULL;

        ආපසු errno_names[id];
}
#endif

const var_na* errno_name(පූර්ණ_සංඛ්‍යා id, var_na buf[static ERRNO_NAME_BUF_LEN]) {
        const var_na *a = errno_name_no_fallback(id);
        නම් (a)
                ආපසු a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ආපසු buf;
}
