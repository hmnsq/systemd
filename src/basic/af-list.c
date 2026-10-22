/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>

#ಅನ್ತರ್ಭಾವ "af-list.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ af_name* lookup_af(ಪಞ್ಜಿಕಾ ಧ್ರುವ ವರ್ಣ *str, ಪಞ್ಜಿಕಾ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "af-from-name.inc"
#ಅನ್ತರ್ಭಾವ "af-to-name.inc"

ಧ್ರುವ ವರ್ಣ* af_to_name(ಪೂರ್ಣಾಙ್ಕ id) {

        ಯದಿ (id <= 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ ((size_t) id >= ELEMENTSOF(af_names))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ af_names[id];
}

ಧ್ರುವ ವರ್ಣ* af_to_name_short(ಪೂರ್ಣಾಙ್ಕ id) {
        ಧ್ರುವ ವರ್ಣ *f;

        ಯದಿ (id == AF_UNSPEC)
                ಪ್ರತ್ಯಾಗಮ "*";

        f = af_to_name(id);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ "unknown";

        ಪ್ರತಿಜ್ಞಾ(startswith(f, "AF_"));
        ಪ್ರತ್ಯಾಗಮ f + 3;
}

ಪೂರ್ಣಾಙ್ಕ af_from_name(ಧ್ರುವ ವರ್ಣ *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ಧ್ರುವ ಸಂರಚನಾ af_name *sc = lookup_af(name, len);
        ಯದಿ (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ಧ್ರುವ ವರ್ಣ *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ಯದಿ (!sc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ sc->id;
}

ಪೂರ್ಣಾಙ್ಕ af_max(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ ELEMENTSOF(af_names);
}

ಧ್ರುವ ವರ್ಣ* af_to_ipv4_ipv6(ಪೂರ್ಣಾಙ್ಕ id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ಪ್ರತ್ಯಾಗಮ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ af_from_ipv4_ipv6(ಧ್ರುವ ವರ್ಣ *af) {
        ಪ್ರತ್ಯಾಗಮ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
