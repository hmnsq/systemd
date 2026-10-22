/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>
#ضمن <sys/socket.h>

#ضمن "af-list.h"
#ضمن "string-util.h"

ساكن ثابت بنية af_name* lookup_af(سجل ثابت حرف *str, سجل GPERF_LEN_TYPE len);

#ضمن "af-from-name.inc"
#ضمن "af-to-name.inc"

ثابت حرف* af_to_name(صحيح id) {

        אם (id <= 0)
                החזר عدم;

        אם ((size_t) id >= ELEMENTSOF(af_names))
                החזר عدم;

        החזר af_names[id];
}

ثابت حرف* af_to_name_short(صحيح id) {
        ثابت حرف *f;

        אם (id == AF_UNSPEC)
                החזר "*";

        f = af_to_name(id);
        אם (!f)
                החזר "unknown";

        أكد(startswith(f, "AF_"));
        החזר f + 3;
}

صحيح af_from_name(ثابت حرف *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ثابت بنية af_name *sc = lookup_af(name, len);
        אם (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ثابت حرف *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        אם (!sc)
                החזר -EINVAL;

        החזר sc->id;
}

صحيح af_max(فراغ) {
        החזר ELEMENTSOF(af_names);
}

ثابت حرف* af_to_ipv4_ipv6(صحيح id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        החזר id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : عدم;
}

صحيح af_from_ipv4_ipv6(ثابت حرف *af) {
        החזר streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
