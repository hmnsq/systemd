/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <sys/socket.h>

#অন্তর্ভাৱ "af-list.h"
#অন্তর্ভাৱ "string-util.h"

স্থির ধ্রুৱ সংরচনা af_name* lookup_af(পঞ্জিকা ধ্রুৱ ৱর্ণ *str, পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাৱ "af-from-name.inc"
#অন্তর্ভাৱ "af-to-name.inc"

ধ্রুৱ ৱর্ণ* af_to_name(পূর্ণাঙ্ক id) {

        যদি (id <= 0)
                ঘূৰাই রিক্ত;

        যদি ((size_t) id >= ELEMENTSOF(af_names))
                ঘূৰাই রিক্ত;

        ঘূৰাই af_names[id];
}

ধ্রুৱ ৱর্ণ* af_to_name_short(পূর্ণাঙ্ক id) {
        ধ্রুৱ ৱর্ণ *f;

        যদি (id == AF_UNSPEC)
                ঘূৰাই "*";

        f = af_to_name(id);
        যদি (!f)
                ঘূৰাই "unknown";

        প্রতিজ্ঞা(startswith(f, "AF_"));
        ঘূৰাই f + 3;
}

পূর্ণাঙ্ক af_from_name(ধ্রুৱ ৱর্ণ *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ধ্রুৱ সংরচনা af_name *sc = lookup_af(name, len);
        যদি (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ধ্রুৱ ৱর্ণ *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        যদি (!sc)
                ঘূৰাই -EINVAL;

        ঘূৰাই sc->id;
}

পূর্ণাঙ্ক af_max(শূন্য) {
        ঘূৰাই ELEMENTSOF(af_names);
}

ধ্রুৱ ৱর্ণ* af_to_ipv4_ipv6(পূর্ণাঙ্ক id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ঘূৰাই id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : রিক্ত;
}

পূর্ণাঙ্ক af_from_ipv4_ipv6(ধ্রুৱ ৱর্ণ *af) {
        ঘূৰাই streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
