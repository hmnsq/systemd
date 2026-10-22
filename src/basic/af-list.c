/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>
#ضمن <sys/socket.h>

#ضمن "af-list.h"
#ضمن "string-util.h"

ساكن ثابت بنية af_name* lookup_af(سجل ثابت حرف *str, سجل GPERF_LEN_TYPE len);

#ضمن "af-from-name.inc"
#ضمن "af-to-name.inc"

ثابت حرف* af_to_name(صحيح id) {

        إذا (id <= 0)
                إرجاع عدم;

        إذا ((size_t) id >= ELEMENTSOF(af_names))
                إرجاع عدم;

        إرجاع af_names[id];
}

ثابت حرف* af_to_name_short(صحيح id) {
        ثابت حرف *f;

        إذا (id == AF_UNSPEC)
                إرجاع "*";

        f = af_to_name(id);
        إذا (!f)
                إرجاع "unknown";

        أكد(startswith(f, "AF_"));
        إرجاع f + 3;
}

صحيح af_from_name(ثابت حرف *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ثابت بنية af_name *sc = lookup_af(name, len);
        إذا (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ثابت حرف *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        إذا (!sc)
                إرجاع -EINVAL;

        إرجاع sc->id;
}

صحيح af_max(فراغ) {
        إرجاع ELEMENTSOF(af_names);
}

ثابت حرف* af_to_ipv4_ipv6(صحيح id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        إرجاع id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : عدم;
}

صحيح af_from_ipv4_ipv6(ثابت حرف *af) {
        إرجاع streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
