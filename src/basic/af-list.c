/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const sa_nracanaa af_name* lookup_af(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const var_na* af_to_name(ຈຳນວນເຕັມ id) {

        ຖ້າ (id <= 0)
                ສົ່ງຄືນ NULL;

        ຖ້າ ((size_t) id >= ELEMENTSOF(af_names))
                ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ af_names[id];
}

const var_na* af_to_name_short(ຈຳນວນເຕັມ id) {
        const var_na *f;

        ຖ້າ (id == AF_UNSPEC)
                ສົ່ງຄືນ "*";

        f = af_to_name(id);
        ຖ້າ (!f)
                ສົ່ງຄືນ "unknown";

        assert(startswith(f, "AF_"));
        ສົ່ງຄືນ f + 3;
}

ຈຳນວນເຕັມ af_from_name(const var_na *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const sa_nracanaa af_name *sc = lookup_af(name, len);
        ຖ້າ (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const var_na *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ຖ້າ (!sc)
                ສົ່ງຄືນ -EINVAL;

        ສົ່ງຄືນ sc->id;
}

ຈຳນວນເຕັມ af_max(void) {
        ສົ່ງຄືນ ELEMENTSOF(af_names);
}

const var_na* af_to_ipv4_ipv6(ຈຳນວນເຕັມ id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ສົ່ງຄືນ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

ຈຳນວນເຕັມ af_from_ipv4_ipv6(const var_na *af) {
        ສົ່ງຄືນ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
