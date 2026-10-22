/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <string.h>
#شامل <sys/socket.h>

#شامل "af-list.h"
#شامل "string-util.h"

ایستا ثابت ساختار af_name* lookup_af(ثبات ثابت نویسه *str, ثبات GPERF_LEN_TYPE len);

#شامل "af-from-name.inc"
#شامل "af-to-name.inc"

ثابت نویسه* af_to_name(صحیح id) {

        اگر (id <= 0)
                بازگشت پوچ;

        اگر ((size_t) id >= ELEMENTSOF(af_names))
                بازگشت پوچ;

        بازگشت af_names[id];
}

ثابت نویسه* af_to_name_short(صحیح id) {
        ثابت نویسه *f;

        اگر (id == AF_UNSPEC)
                بازگشت "*";

        f = af_to_name(id);
        اگر (!f)
                بازگشت "unknown";

        أكد(startswith(f, "AF_"));
        بازگشت f + 3;
}

صحیح af_from_name(ثابت نویسه *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ثابت ساختار af_name *sc = lookup_af(name, len);
        اگر (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ثابت نویسه *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        اگر (!sc)
                بازگشت -EINVAL;

        بازگشت sc->id;
}

صحیح af_max(تهی) {
        بازگشت ELEMENTSOF(af_names);
}

ثابت نویسه* af_to_ipv4_ipv6(صحیح id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        بازگشت id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : پوچ;
}

صحیح af_from_ipv4_ipv6(ثابت نویسه *af) {
        بازگشت streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
