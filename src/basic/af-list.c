/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const struct af_name* lookup_af(register const char *str, register GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const char* af_to_name(цяло id) {

        ако (id <= 0)
                върни NULL;

        ако ((size_t) id >= ELEMENTSOF(af_names))
                върни NULL;

        върни af_names[id];
}

const char* af_to_name_short(цяло id) {
        const char *f;

        ако (id == AF_UNSPEC)
                върни "*";

        f = af_to_name(id);
        ако (!f)
                върни "unknown";

        assert(startswith(f, "AF_"));
        върни f + 3;
}

цяло af_from_name(const char *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const struct af_name *sc = lookup_af(name, len);
        ако (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const char *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ако (!sc)
                върни -EINVAL;

        върни sc->id;
}

цяло af_max(void) {
        върни ELEMENTSOF(af_names);
}

const char* af_to_ipv4_ipv6(цяло id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        върни id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

цяло af_from_ipv4_ipv6(const char *af) {
        върни streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
