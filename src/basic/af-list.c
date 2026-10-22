/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/socket.h>

#અન્તર્ભાવ "af-list.h"
#અન્તર્ભાવ "string-util.h"

સ્થિર અચલ સંરચના af_name* lookup_af(નોંધણી અચલ અક્ષર *str, નોંધણી GPERF_LEN_TYPE len);

#અન્તર્ભાવ "af-from-name.inc"
#અન્તર્ભાવ "af-to-name.inc"

અચલ અક્ષર* af_to_name(પૂર્ણાંક id) {

        જો (id <= 0)
                પાછા ખાલી;

        જો ((size_t) id >= ELEMENTSOF(af_names))
                પાછા ખાલી;

        પાછા af_names[id];
}

અચલ અક્ષર* af_to_name_short(પૂર્ણાંક id) {
        અચલ અક્ષર *f;

        જો (id == AF_UNSPEC)
                પાછા "*";

        f = af_to_name(id);
        જો (!f)
                પાછા "unknown";

        પ્રતિજ્ઞા(startswith(f, "AF_"));
        પાછા f + 3;
}

પૂર્ણાંક af_from_name(અચલ અક્ષર *name) {
        size_t len = strlen(ASSERT_PTR(name));

        અચલ સંરચના af_name *sc = lookup_af(name, len);
        જો (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                અચલ અક્ષર *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        જો (!sc)
                પાછા -EINVAL;

        પાછા sc->id;
}

પૂર્ણાંક af_max(શૂન્ય) {
        પાછા ELEMENTSOF(af_names);
}

અચલ અક્ષર* af_to_ipv4_ipv6(પૂર્ણાંક id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        પાછા id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ખાલી;
}

પૂર્ણાંક af_from_ipv4_ipv6(અચલ અક્ષર *af) {
        પાછા streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
