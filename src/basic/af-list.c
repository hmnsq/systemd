/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <string.h>
#شامل <sys/socket.h>

#شامل "af-list.h"
#شامل "string-util.h"

ساکن ثابت ساخت af_name* lookup_af(اندراج ثابت حرف *str, اندراج GPERF_LEN_TYPE len);

#شامل "af-from-name.inc"
#شامل "af-to-name.inc"

ثابت حرف* af_مقصد_name(صحیح id) {

        اگر (id <= 0)
                واپس تہی;

        اگر ((size_t) id >= ELEMENTSOF(af_نام))
                واپس تہی;

        واپس af_نام[id];
}

ثابت حرف* af_مقصد_name_short(صحیح id) {
        ثابت حرف *f;

        اگر (id == AF_UNSPEC)
                واپس "*";

        f = af_مقصد_name(id);
        اگر (!f)
                واپس "unknown";

        أكد(startswith(f, "AF_"));
        واپس f + 3;
}

صحیح af_from_name(ثابت حرف *name) {
        size_t len = لڑی_لمبائی(ASSERT_PTR(name));

        ثابت ساخت af_name *sc = lookup_af(name, len);
        اگر (!sc && len + لڑی_لمبائی("AF_") <= MAX_لفظ_لمبائی) {
                ثابت حرف *t = strjoina("AF_", name);
                sc = lookup_af(t, لڑی_لمبائی("AF_") + len);
        }
        اگر (!sc)
                واپس -EINVAL;

        واپس sc->id;
}

صحیح af_max(خالی) {
        واپس ELEMENTSOF(af_نام);
}

ثابت حرف* af_مقصد_ipv4_ipv6(صحیح id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        واپس id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : تہی;
}

صحیح af_from_ipv4_ipv6(ثابت حرف *af) {
        واپس streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
