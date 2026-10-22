/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const sa_nracanaa af_name* lookup_af(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const var_na* af_to_name(ကိန်းပြည့် id) {

        အကယ်၍ (id <= 0)
                ပြန်ပေး NULL;

        အကယ်၍ ((size_t) id >= ELEMENTSOF(af_names))
                ပြန်ပေး NULL;

        ပြန်ပေး af_names[id];
}

const var_na* af_to_name_short(ကိန်းပြည့် id) {
        const var_na *f;

        အကယ်၍ (id == AF_UNSPEC)
                ပြန်ပေး "*";

        f = af_to_name(id);
        အကယ်၍ (!f)
                ပြန်ပေး "unknown";

        assert(startswith(f, "AF_"));
        ပြန်ပေး f + 3;
}

ကိန်းပြည့် af_from_name(const var_na *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const sa_nracanaa af_name *sc = lookup_af(name, len);
        အကယ်၍ (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const var_na *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        အကယ်၍ (!sc)
                ပြန်ပေး -EINVAL;

        ပြန်ပေး sc->id;
}

ကိန်းပြည့် af_max(void) {
        ပြန်ပေး ELEMENTSOF(af_names);
}

const var_na* af_to_ipv4_ipv6(ကိန်းပြည့် id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ပြန်ပေး id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

ကိန်းပြည့် af_from_ipv4_ipv6(const var_na *af) {
        ပြန်ပေး streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
