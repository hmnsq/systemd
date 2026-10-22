/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const sa_nracanaa af_name* lookup_af(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const var_na* af_to_name(පූර්ණ_සංඛ්‍යා id) {

        නම් (id <= 0)
                ආපසු NULL;

        නම් ((size_t) id >= ELEMENTSOF(af_names))
                ආපසු NULL;

        ආපසු af_names[id];
}

const var_na* af_to_name_short(පූර්ණ_සංඛ්‍යා id) {
        const var_na *f;

        නම් (id == AF_UNSPEC)
                ආපසු "*";

        f = af_to_name(id);
        නම් (!f)
                ආපසු "unknown";

        assert(startswith(f, "AF_"));
        ආපසු f + 3;
}

පූර්ණ_සංඛ්‍යා af_from_name(const var_na *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const sa_nracanaa af_name *sc = lookup_af(name, len);
        නම් (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const var_na *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        නම් (!sc)
                ආපසු -EINVAL;

        ආපසු sc->id;
}

පූර්ණ_සංඛ්‍යා af_max(void) {
        ආපසු ELEMENTSOF(af_names);
}

const var_na* af_to_ipv4_ipv6(පූර්ණ_සංඛ්‍යා id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ආපසු id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

පූර්ණ_සංඛ්‍යා af_from_ipv4_ipv6(const var_na *af) {
        ආපසු streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
