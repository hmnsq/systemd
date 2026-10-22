/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const sa_nracanaa af_name* lookup_af(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const var_na* af_to_name(puur_naa_nka id) {

        ꦪꦼꦤ꧀ (id <= 0)
                ꦮꦭꦶ NULL;

        ꦪꦼꦤ꧀ ((size_t) id >= ELEMENTSOF(af_names))
                ꦮꦭꦶ NULL;

        ꦮꦭꦶ af_names[id];
}

const var_na* af_to_name_short(puur_naa_nka id) {
        const var_na *f;

        ꦪꦼꦤ꧀ (id == AF_UNSPEC)
                ꦮꦭꦶ "*";

        f = af_to_name(id);
        ꦪꦼꦤ꧀ (!f)
                ꦮꦭꦶ "unknown";

        assert(startswith(f, "AF_"));
        ꦮꦭꦶ f + 3;
}

puur_naa_nka af_from_name(const var_na *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const sa_nracanaa af_name *sc = lookup_af(name, len);
        ꦪꦼꦤ꧀ (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const var_na *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ꦪꦼꦤ꧀ (!sc)
                ꦮꦭꦶ -EINVAL;

        ꦮꦭꦶ sc->id;
}

puur_naa_nka af_max(void) {
        ꦮꦭꦶ ELEMENTSOF(af_names);
}

const var_na* af_to_ipv4_ipv6(puur_naa_nka id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ꦮꦭꦶ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

puur_naa_nka af_from_ipv4_ipv6(const var_na *af) {
        ꦮꦭꦶ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
