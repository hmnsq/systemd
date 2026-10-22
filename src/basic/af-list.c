/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/socket.h>

#ଅନ୍ତର୍ଭାଵ "af-list.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ସ୍ଥିର ଅଚଳ ସଂରଚନା af_name* lookup_af(ପଞ୍ଜିକରଣ ଅଚଳ ଅକ୍ଷର *str, ପଞ୍ଜିକରଣ GPERF_LEN_TYPE len);

#ଅନ୍ତର୍ଭାଵ "af-from-name.inc"
#ଅନ୍ତର୍ଭାଵ "af-to-name.inc"

ଅଚଳ ଅକ୍ଷର* af_to_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) {

        ଯଦି (id <= 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି ((size_t) id >= ELEMENTSOF(af_names))
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ af_names[id];
}

ଅଚଳ ଅକ୍ଷର* af_to_name_short(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) {
        ଅଚଳ ଅକ୍ଷର *f;

        ଯଦି (id == AF_UNSPEC)
                ଫେରନ୍ତୁ "*";

        f = af_to_name(id);
        ଯଦି (!f)
                ଫେରନ୍ତୁ "unknown";

        ପ୍ରତିଜ୍ଞା(startswith(f, "AF_"));
        ଫେରନ୍ତୁ f + 3;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା af_from_name(ଅଚଳ ଅକ୍ଷର *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ଅଚଳ ସଂରଚନା af_name *sc = lookup_af(name, len);
        ଯଦି (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ଅଚଳ ଅକ୍ଷର *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        ଯଦି (!sc)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ sc->id;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା af_max(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ ELEMENTSOF(af_names);
}

ଅଚଳ ଅକ୍ଷର* af_to_ipv4_ipv6(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ଫେରନ୍ତୁ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା af_from_ipv4_ipv6(ଅଚଳ ଅକ୍ଷର *af) {
        ଫେରନ୍ତୁ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
