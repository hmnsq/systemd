/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const sa_nracanaa af_name* lookup_af(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const var_na* af_to_name(จำนวนเต็ม id) {

        ถ้า (id <= 0)
                คืน NULL;

        ถ้า ((size_t) id >= ELEMENTSOF(af_names))
                คืน NULL;

        คืน af_names[id];
}

const var_na* af_to_name_short(จำนวนเต็ม id) {
        const var_na *f;

        ถ้า (id == AF_UNSPEC)
                คืน "*";

        f = af_to_name(id);
        ถ้า (!f)
                คืน "unknown";

        assert(startswith(f, "AF_"));
        คืน f + 3;
}

จำนวนเต็ม af_from_name(const var_na *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const sa_nracanaa af_name *sc = lookup_af(name, len);
        ถ้า (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const var_na *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ถ้า (!sc)
                คืน -EINVAL;

        คืน sc->id;
}

จำนวนเต็ม af_max(void) {
        คืน ELEMENTSOF(af_names);
}

const var_na* af_to_ipv4_ipv6(จำนวนเต็ม id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        คืน id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

จำนวนเต็ม af_from_ipv4_ipv6(const var_na *af) {
        คืน streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
