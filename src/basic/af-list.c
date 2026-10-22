/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/socket.h>

#অন্তর্ভাব "af-list.h"
#অন্তর্ভাব "string-util.h"

স্থির ধ্রুব সংরচনা af_name* lookup_af(পঞ্জিকা ধ্রুব বর্ণ *str, পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাব "af-from-name.inc"
#অন্তর্ভাব "af-to-name.inc"

ধ্রুব বর্ণ* af_to_name(পূর্ণাঙ্ক id) {

        যদি (id <= 0)
                প্রত্যাগম রিক্ত;

        যদি ((size_t) id >= ELEMENTSOF(af_names))
                প্রত্যাগম রিক্ত;

        প্রত্যাগম af_names[id];
}

ধ্রুব বর্ণ* af_to_name_short(পূর্ণাঙ্ক id) {
        ধ্রুব বর্ণ *f;

        যদি (id == AF_UNSPEC)
                প্রত্যাগম "*";

        f = af_to_name(id);
        যদি (!f)
                প্রত্যাগম "unknown";

        প্রতিজ্ঞা(startswith(f, "AF_"));
        প্রত্যাগম f + 3;
}

পূর্ণাঙ্ক af_from_name(ধ্রুব বর্ণ *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ধ্রুব সংরচনা af_name *sc = lookup_af(name, len);
        যদি (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ধ্রুব বর্ণ *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        যদি (!sc)
                প্রত্যাগম -EINVAL;

        প্রত্যাগম sc->id;
}

পূর্ণাঙ্ক af_max(শূন্য) {
        প্রত্যাগম ELEMENTSOF(af_names);
}

ধ্রুব বর্ণ* af_to_ipv4_ipv6(পূর্ণাঙ্ক id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        প্রত্যাগম id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : রিক্ত;
}

পূর্ণাঙ্ক af_from_ipv4_ipv6(ধ্রুব বর্ণ *af) {
        প্রত্যাগম streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
