/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errno-list.h"

static const sa_nracanaa errno_name* lookup_errno(pa_njikaa const var_na *str,
                                             pa_njikaa GPERF_LEN_TYPE len);

#include "errno-from-name.inc"

puur_naa_nka errno_from_name(const var_na *name) {
        const sa_nracanaa errno_name *sc;

        assert(name);

        sc = lookup_errno(name, strlen(name));
        ᮚᮦᮔ᮪ (!sc)
                ᮝᮜᮤ -EINVAL;

        assert(sc->id > 0);
        ᮝᮜᮤ sc->id;
}

#ifdef __GLIBC__
const var_na* errno_name_no_fallback(puur_naa_nka id) {
        ᮚᮦᮔ᮪ (id == 0) /* To stay in line with our implementation below.  */
                ᮝᮜᮤ NULL;

        ᮝᮜᮤ strerrorname_np(ABS(id));
}
#ᮜᮤᮚ
#  include "errno-to-name.inc"

const var_na* errno_name_no_fallback(puur_naa_nka id) {
        ᮚᮦᮔ᮪ (id < 0)
                id = -id;

        ᮚᮦᮔ᮪ ((size_t) id >= ELEMENTSOF(errno_names))
                ᮝᮜᮤ NULL;

        ᮝᮜᮤ errno_names[id];
}
#endif

const var_na* errno_name(puur_naa_nka id, var_na buf[static ERRNO_NAME_BUF_LEN]) {
        const var_na *a = errno_name_no_fallback(id);
        ᮚᮦᮔ᮪ (a)
                ᮝᮜᮤ a;
        snprintf(buf, ERRNO_NAME_BUF_LEN, "%d", abs(id));
        ᮝᮜᮤ buf;
}
