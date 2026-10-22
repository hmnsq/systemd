/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/socket.h>

#അന്തര്ഭാവ "af-list.h"
#അന്തര്ഭാവ "string-util.h"

സ്ഥിതം സ്ഥിരം ഘടന af_name* lookup_af(രജിസ്റ്റർ സ്ഥിരം അക്ഷരം *str, രജിസ്റ്റർ GPERF_LEN_TYPE len);

#അന്തര്ഭാവ "af-from-name.inc"
#അന്തര്ഭാവ "af-to-name.inc"

സ്ഥിരം അക്ഷരം* af_to_name(പൂർണ്ണസംഖ്യ id) {

        എങ്കിൽ (id <= 0)
                തിരികെ ശൂന്യ;

        എങ്കിൽ ((size_t) id >= ELEMENTSOF(af_names))
                തിരികെ ശൂന്യ;

        തിരികെ af_names[id];
}

സ്ഥിരം അക്ഷരം* af_to_name_short(പൂർണ്ണസംഖ്യ id) {
        സ്ഥിരം അക്ഷരം *f;

        എങ്കിൽ (id == AF_UNSPEC)
                തിരികെ "*";

        f = af_to_name(id);
        എങ്കിൽ (!f)
                തിരികെ "unknown";

        പ്രതിജ്ഞാ(startswith(f, "AF_"));
        തിരികെ f + 3;
}

പൂർണ്ണസംഖ്യ af_from_name(സ്ഥിരം അക്ഷരം *name) {
        size_t len = strlen(ASSERT_PTR(name));

        സ്ഥിരം ഘടന af_name *sc = lookup_af(name, len);
        എങ്കിൽ (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                സ്ഥിരം അക്ഷരം *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        എങ്കിൽ (!sc)
                തിരികെ -EINVAL;

        തിരികെ sc->id;
}

പൂർണ്ണസംഖ്യ af_max(ശൂന്യം) {
        തിരികെ ELEMENTSOF(af_names);
}

സ്ഥിരം അക്ഷരം* af_to_ipv4_ipv6(പൂർണ്ണസംഖ്യ id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        തിരികെ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ശൂന്യ;
}

പൂർണ്ണസംഖ്യ af_from_ipv4_ipv6(സ്ഥിരം അക്ഷരം *af) {
        തിരികെ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
