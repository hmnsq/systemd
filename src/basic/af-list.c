/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/socket.h>

#অন্তর্ভাব "af-list.h"
#অন্তর্ভাব "string-util.h"

স্থির ধ্রুবক কাঠামো af_name* lookup_af(নিবন্ধ ধ্রুবক অক্ষর *str, নিবন্ধ GPERF_LEN_TYPE len);

#অন্তর্ভাব "af-from-name.inc"
#অন্তর্ভাব "af-to-name.inc"

ধ্রুবক অক্ষর* af_to_name(পূর্ণসংখ্যা id) {

        যদি (id <= 0)
                ফেরত ফাঁকা;

        যদি ((size_t) id >= ELEMENTSOF(af_names))
                ফেরত ফাঁকা;

        ফেরত af_names[id];
}

ধ্রুবক অক্ষর* af_to_name_short(পূর্ণসংখ্যা id) {
        ধ্রুবক অক্ষর *f;

        যদি (id == AF_UNSPEC)
                ফেরত "*";

        f = af_to_name(id);
        যদি (!f)
                ফেরত "unknown";

        প্রতিজ্ঞা(startswith(f, "AF_"));
        ফেরত f + 3;
}

পূর্ণসংখ্যা af_from_name(ধ্রুবক অক্ষর *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ধ্রুবক কাঠামো af_name *sc = lookup_af(name, len);
        যদি (!sc && len + strlen("AF_") <= MAX_WORD_দৈর্ঘ্য) {
                ধ্রুবক অক্ষর *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        যদি (!sc)
                ফেরত -EINVAL;

        ফেরত sc->id;
}

পূর্ণসংখ্যা af_max(শূন্য) {
        ফেরত ELEMENTSOF(af_names);
}

ধ্রুবক অক্ষর* af_to_ipv4_ipv6(পূর্ণসংখ্যা id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        ফেরত id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ফাঁকা;
}

পূর্ণসংখ্যা af_from_ipv4_ipv6(ধ্রুবক অক্ষর *af) {
        ফেরত streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
