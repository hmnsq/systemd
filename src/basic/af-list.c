/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/socket.h>

#அந்தர்பாவ "af-list.h"
#அந்தர்பாவ "string-util.h"

நிலையான மாறிலி கட்டமைப்பு af_name* lookup_af(பதிவு மாறிலி எழுத்து *str, பதிவு GPERF_LEN_TYPE len);

#அந்தர்பாவ "af-from-name.inc"
#அந்தர்பாவ "af-to-name.inc"

மாறிலி எழுத்து* af_to_name(முழு id) {

        எனில் (id <= 0)
                திரும்பு வெற்று;

        எனில் ((size_t) id >= ELEMENTSOF(af_names))
                திரும்பு வெற்று;

        திரும்பு af_names[id];
}

மாறிலி எழுத்து* af_to_name_short(முழு id) {
        மாறிலி எழுத்து *f;

        எனில் (id == AF_UNSPEC)
                திரும்பு "*";

        f = af_to_name(id);
        எனில் (!f)
                திரும்பு "unknown";

        ப்ரதிஜ்ஞா(startswith(f, "AF_"));
        திரும்பு f + 3;
}

முழு af_from_name(மாறிலி எழுத்து *name) {
        size_t len = strlen(ASSERT_PTR(name));

        மாறிலி கட்டமைப்பு af_name *sc = lookup_af(name, len);
        எனில் (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                மாறிலி எழுத்து *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        எனில் (!sc)
                திரும்பு -EINVAL;

        திரும்பு sc->id;
}

முழு af_max(வெறுமை) {
        திரும்பு ELEMENTSOF(af_names);
}

மாறிலி எழுத்து* af_to_ipv4_ipv6(முழு id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        திரும்பு id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : வெற்று;
}

முழு af_from_ipv4_ipv6(மாறிலி எழுத்து *af) {
        திரும்பு streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
