/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const struct af_name* lookup_af(register const char *str, register GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const char* af_to_name(정수 id) {

        만약 (id <= 0)
                반환 NULL;

        만약 ((size_t) id >= ELEMENTSOF(af_names))
                반환 NULL;

        반환 af_names[id];
}

const char* af_to_name_short(정수 id) {
        const char *f;

        만약 (id == AF_UNSPEC)
                반환 "*";

        f = af_to_name(id);
        만약 (!f)
                반환 "unknown";

        assert(startswith(f, "AF_"));
        반환 f + 3;
}

정수 af_from_name(const char *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const struct af_name *sc = lookup_af(name, len);
        만약 (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const char *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        만약 (!sc)
                반환 -EINVAL;

        반환 sc->id;
}

정수 af_max(void) {
        반환 ELEMENTSOF(af_names);
}

const char* af_to_ipv4_ipv6(정수 id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        반환 id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

정수 af_from_ipv4_ipv6(const char *af) {
        반환 streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
