/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const sa_nracanaa af_name* lookup_af(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const var_na* af_to_name(ཧྲིལ id) {

        གལ་ཏེ (id <= 0)
                ཕྱིར NULL;

        གལ་ཏེ ((size_t) id >= ELEMENTSOF(af_names))
                ཕྱིར NULL;

        ཕྱིར af_names[id];
}

const var_na* af_to_name_short(ཧྲིལ id) {
        const var_na *f;

        གལ་ཏེ (id == AF_UNSPEC)
                ཕྱིར "*";

        f = af_to_name(id);
        གལ་ཏེ (!f)
                ཕྱིར "unknown";

        assert(startswith(f, "AF_"));
        ཕྱིར f + 3;
}

ཧྲིལ af_from_name(const var_na *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const sa_nracanaa af_name *sc = lookup_af(name, len);
        གལ་ཏེ (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const var_na *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        གལ་ཏེ (!sc)
                ཕྱིར -EINVAL;

        ཕྱིར sc->id;
}

ཧྲིལ af_max(void) {
        ཕྱིར ELEMENTSOF(af_names);
}

const var_na* af_to_ipv4_ipv6(ཧྲིལ id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ཕྱིར id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

ཧྲིལ af_from_ipv4_ipv6(const var_na *af) {
        ཕྱིར streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
