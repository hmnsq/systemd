/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>
#ضمن <sys/socket.h>

#ضمن "af-list.h"
#ضمن "string-util.h"

ساكن ثابت بنية af_name* lookup_af(سجل ثابت حرف *str, سجل GPERF_LEN_TYPE len);

#ضمن "af-from-name.inc"
#ضمن "af-to-name.inc"

ثابت حرف* af_منزل_name(صحيح id) {

        إذا (id <= 0)
                أرجع عدم;

        إذا ((size_t) id >= ELEMENTSOF(af_ناں))
                أرجع عدم;

        أرجع af_ناں[id];
}

ثابت حرف* af_منزل_name_short(صحيح id) {
        ثابت حرف *f;

        إذا (id == AF_UNSPEC)
                أرجع "*";

        f = af_منزل_name(id);
        إذا (!f)
                أرجع "unknown";

        أكد(startswith(f, "AF_"));
        أرجع f + 3;
}

صحيح af_from_name(ثابت حرف *name) {
        size_t len = لڑی_لمبائی(ASSERT_PTR(name));

        ثابت بنية af_name *sc = lookup_af(name, len);
        إذا (!sc && len + لڑی_لمبائی("AF_") <= MAX_لفظ_لمائی) {
                ثابت حرف *t = strjoina("AF_", name);
                sc = lookup_af(t, لڑی_لمبائی("AF_") + len);
        }
        إذا (!sc)
                أرجع -EINVAL;

        أرجع sc->id;
}

صحيح af_max(فراغ) {
        أرجع ELEMENTSOF(af_ناں);
}

ثابت حرف* af_منزل_ipv4_ipv6(صحيح id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        أرجع id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : عدم;
}

صحيح af_from_ipv4_ipv6(ثابت حرف *af) {
        أرجع streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
