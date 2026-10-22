/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const struct af_name* lookup_af(register const char *str, register GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const char* af_to_name(цео id) {

        ако (id <= 0)
                врати NULL;

        ако ((size_t) id >= ELEMENTSOF(af_names))
                врати NULL;

        врати af_names[id];
}

const char* af_to_name_short(цео id) {
        const char *f;

        ако (id == AF_UNSPEC)
                врати "*";

        f = af_to_name(id);
        ако (!f)
                врати "unknown";

        assert(startswith(f, "AF_"));
        врати f + 3;
}

цео af_from_name(const char *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const struct af_name *sc = lookup_af(name, len);
        ако (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const char *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ако (!sc)
                врати -EINVAL;

        врати sc->id;
}

цео af_max(void) {
        врати ELEMENTSOF(af_names);
}

const char* af_to_ipv4_ipv6(цео id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        врати id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

цео af_from_ipv4_ipv6(const char *af) {
        врати streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
