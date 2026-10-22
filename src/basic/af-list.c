/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/socket.h>

#include "af-list.h"
#include "string-util.h"

static const struct af_name* lookup_af(register const char *str, register GPERF_LEN_TYPE len);

#include "af-from-name.inc"
#include "af-to-name.inc"

const char* af_to_name(số_nguyên id) {

        nếu (id <= 0)
                trả_về NULL;

        nếu ((size_t) id >= ELEMENTSOF(af_names))
                trả_về NULL;

        trả_về af_names[id];
}

const char* af_to_name_short(số_nguyên id) {
        const char *f;

        nếu (id == AF_UNSPEC)
                trả_về "*";

        f = af_to_name(id);
        nếu (!f)
                trả_về "unknown";

        assert(startswith(f, "AF_"));
        trả_về f + 3;
}

số_nguyên af_from_name(const char *name) {
        size_t len = strlen(ASSERT_PTR(name));

        const struct af_name *sc = lookup_af(name, len);
        nếu (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                const char *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        nếu (!sc)
                trả_về -EINVAL;

        trả_về sc->id;
}

số_nguyên af_max(void) {
        trả_về ELEMENTSOF(af_names);
}

const char* af_to_ipv4_ipv6(số_nguyên id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        trả_về id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : NULL;
}

số_nguyên af_from_ipv4_ipv6(const char *af) {
        trả_về streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
