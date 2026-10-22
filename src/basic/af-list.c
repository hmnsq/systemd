/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const sa_nracanaa af_name* lookup_af(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const var_na* af_to_name(ចំនួនគត់ id) {

        ប្រសិនបើ (id <= 0)
                ត្រឡប់ NULL;

        ប្រសិនបើ ((size_t) id >= ELEMENTSOF(af_names))
                ត្រឡប់ NULL;

        ត្រឡប់ af_names[id];
}

const var_na* af_to_name_short(ចំនួនគត់ id) {
        const var_na *f;

        ប្រសិនបើ (id == AF_UNSPEC)
                ត្រឡប់ "*";

        f = af_to_name(id);
        ប្រសិនបើ (!f)
                ត្រឡប់ "unknown";

        assert(startswith(f, "AF_"));
        ត្រឡប់ f + 3;
}

ចំនួនគត់ af_from_name(const var_na *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const sa_nracanaa af_name *sc = lookup_af(name, len);
        ប្រសិនបើ (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const var_na *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ប្រសិនបើ (!sc)
                ត្រឡប់ -EINVAL;

        ត្រឡប់ sc->id;
}

ចំនួនគត់ af_max(void) {
        ត្រឡប់ ELEMENTSOF(af_names);
}

const var_na* af_to_ipv4_ipv6(ចំនួនគត់ id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ត្រឡប់ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

ចំនួនគត់ af_from_ipv4_ipv6(const var_na *af) {
        ត្រឡប់ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
