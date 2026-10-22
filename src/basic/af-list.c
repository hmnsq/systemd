/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const struct af_name* lookup_af(register const char *str, register GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const char* af_to_name(ціле id) {

        якщо (id <= 0)
                повернути NULL;

        якщо ((size_t) id >= ELEMENTSOF(af_names))
                повернути NULL;

        повернути af_names[id];
}

const char* af_to_name_short(ціле id) {
        const char *f;

        якщо (id == AF_UNSPEC)
                повернути "*";

        f = af_to_name(id);
        якщо (!f)
                повернути "unknown";

        assert(startswith(f, "AF_"));
        повернути f + 3;
}

ціле af_from_name(const char *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const struct af_name *sc = lookup_af(name, len);
        якщо (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const char *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        якщо (!sc)
                повернути -EINVAL;

        повернути sc->id;
}

ціле af_max(void) {
        повернути ELEMENTSOF(af_names);
}

const char* af_to_ipv4_ipv6(ціле id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        повернути id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

ціле af_from_ipv4_ipv6(const char *af) {
        повернути streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
