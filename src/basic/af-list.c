/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>

#ಅನ್ತರ್ಭಾವ "af-list.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ af_name* lookup_af(ನೋಂದಣಿ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ನೋಂದಣಿ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "af-from-name.inc"
#ಅನ್ತರ್ಭಾವ "af-to-name.inc"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* af_ಗಮ್ಯ_name(ಪೂರ್ಣಾಂಕ id) {

        ಆದರೆ (id <= 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ ((size_t) id >= ELEMENTSOF(af_ಹೆಸರುಗಳು))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ af_ಹೆಸರುಗಳು[id];
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* af_ಗಮ್ಯ_name_short(ಪೂರ್ಣಾಂಕ id) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f;

        ಆದರೆ (id == AF_UNSPEC)
                ಹಿಂತಿರುಗಿ "*";

        f = af_ಗಮ್ಯ_name(id);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ "unknown";

        ಪ್ರತಿಜ್ಞಾ(startswith(f, "AF_"));
        ಹಿಂತಿರುಗಿ f + 3;
}

ಪೂರ್ಣಾಂಕ af_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ASSERT_PTR(name));

        ಸ್ಥಿರಾಂಕ ರಚನೆ af_name *sc = lookup_af(name, len);
        ಆದರೆ (!sc && len + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("AF_") <= MAX_ಶಬ್ದ_ಉದ್ದಳತೆ) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t = strjoina("AF_", name);
                sc = lookup_af(t, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("AF_") + len);
        }
        ಆದರೆ (!sc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ sc->id;
}

ಪೂರ್ಣಾಂಕ af_max(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ ELEMENTSOF(af_ಹೆಸರುಗಳು);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* af_ಗಮ್ಯ_ipv4_ipv6(ಪೂರ್ಣಾಂಕ id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ಹಿಂತಿರುಗಿ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ af_from_ipv4_ipv6(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *af) {
        ಹಿಂತಿರುಗಿ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
