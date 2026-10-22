/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const struct af_name* lookup_af(register const char *str, register GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const char* af_to_name(целое id) {

        если (id <= 0)
                вернуть NULL;

        если ((size_t) id >= ELEMENTSOF(af_names))
                вернуть NULL;

        вернуть af_names[id];
}

const char* af_to_name_short(целое id) {
        const char *f;

        если (id == AF_UNSPEC)
                вернуть "*";

        f = af_to_name(id);
        если (!f)
                вернуть "unknown";

        assert(startswith(f, "AF_"));
        вернуть f + 3;
}

целое af_from_name(const char *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const struct af_name *sc = lookup_af(name, len);
        если (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const char *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        если (!sc)
                вернуть -EINVAL;

        вернуть sc->id;
}

целое af_max(void) {
        вернуть ELEMENTSOF(af_names);
}

const char* af_to_ipv4_ipv6(целое id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        вернуть id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

целое af_from_ipv4_ipv6(const char *af) {
        вернуть streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
