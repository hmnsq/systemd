/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "af-list.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਸਥਿਰ ਅਚਲ ਬਣਤਰ af_name* lookup_af(ਰਜਿਸਟਰ ਅਚਲ ਅੱਖਰ *str, ਰਜਿਸਟਰ GPERF_LEN_TYPE len);

#ਅਨ੍ਤਰ੍ਭਾਵ "af-from-name.inc"
#ਅਨ੍ਤਰ੍ਭਾਵ "af-to-name.inc"

ਅਚਲ ਅੱਖਰ* af_ਮੰਜ਼ਿਲ_name(ਪੂਰਨ id) {

        ਜੇ (id <= 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ ((size_t) id >= ELEMENTSOF(af_ਨਾਂ))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ af_ਨਾਂ[id];
}

ਅਚਲ ਅੱਖਰ* af_ਮੰਜ਼ਿਲ_name_short(ਪੂਰਨ id) {
        ਅਚਲ ਅੱਖਰ *f;

        ਜੇ (id == AF_UNSPEC)
                ਵਾਪਸ "*";

        f = af_ਮੰਜ਼ਿਲ_name(id);
        ਜੇ (!f)
                ਵਾਪਸ "unknown";

        ਪ੍ਰਤਿਜ੍ਞਾ(startswith(f, "AF_"));
        ਵਾਪਸ f + 3;
}

ਪੂਰਨ af_from_name(ਅਚਲ ਅੱਖਰ *name) {
        size_t len = ਲੜੀ_ਲੰਬਾਈ(ASSERT_PTR(name));

        ਅਚਲ ਬਣਤਰ af_name *sc = lookup_af(name, len);
        ਜੇ (!sc && len + ਲੜੀ_ਲੰਬਾਈ("AF_") <= MAX_ਸ਼ਬਦ_ਲੰਮਾਈ) {
                ਅਚਲ ਅੱਖਰ *t = strjoina("AF_", name);
                sc = lookup_af(t, ਲੜੀ_ਲੰਬਾਈ("AF_") + len);
        }
        ਜੇ (!sc)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ sc->id;
}

ਪੂਰਨ af_max(ਖਾਲੀ) {
        ਵਾਪਸ ELEMENTSOF(af_ਨਾਂ);
}

ਅਚਲ ਅੱਖਰ* af_ਮੰਜ਼ਿਲ_ipv4_ipv6(ਪੂਰਨ id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ਵਾਪਸ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ਨਿਰਾ;
}

ਪੂਰਨ af_from_ipv4_ipv6(ਅਚਲ ਅੱਖਰ *af) {
        ਵਾਪਸ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
