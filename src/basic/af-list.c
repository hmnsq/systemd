/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/socket.h>

#अन्तर्भाव "af-list.h"
#अन्तर्भाव "string-util.h"

स्थिर ध्रुव संरचना af_name* lookup_af(पञ्जिका ध्रुव वर्ण *str, पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "af-from-name.inc"
#अन्तर्भाव "af-to-name.inc"

ध्रुव वर्ण* af_to_name(पूर्णाङ्क id) {

        सचे (id <= 0)
                पटिनिवत्त रिक्त;

        सचे ((size_t) id >= ELEMENTSOF(af_names))
                पटिनिवत्त रिक्त;

        पटिनिवत्त af_names[id];
}

ध्रुव वर्ण* af_to_name_short(पूर्णाङ्क id) {
        ध्रुव वर्ण *f;

        सचे (id == AF_UNSPEC)
                पटिनिवत्त "*";

        f = af_to_name(id);
        सचे (!f)
                पटिनिवत्त "unknown";

        प्रतिज्ञा(startswith(f, "AF_"));
        पटिनिवत्त f + 3;
}

पूर्णाङ्क af_from_name(ध्रुव वर्ण *name) {
        size_t len = strlen(ASSERT_PTR(name));

        ध्रुव संरचना af_name *sc = lookup_af(name, len);
        सचे (!sc && len + strlen("AF_") <= MAX_WORD_LENGTH) {
                ध्रुव वर्ण *t = strjoina("AF_", name);
                sc = lookup_af(t, strlen("AF_") + len);
        }
        सचे (!sc)
                पटिनिवत्त -EINVAL;

        पटिनिवत्त sc->id;
}

पूर्णाङ्क af_max(शून्य) {
        पटिनिवत्त ELEMENTSOF(af_names);
}

ध्रुव वर्ण* af_to_ipv4_ipv6(पूर्णाङ्क id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        पटिनिवत्त id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : रिक्त;
}

पूर्णाङ्क af_from_ipv4_ipv6(ध्रुव वर्ण *af) {
        पटिनिवत्त streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
