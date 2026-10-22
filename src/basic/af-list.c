/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/socket.h>

#अन्तर्भाव "af-list.h"
#अन्तर्भाव "string-util.h"

स्थिर अचर संरचना af_name* lookup_af(पञ्जिका अचर वर्ण *str, पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "af-from-name.inc"
#अन्तर्भाव "af-to-name.inc"

अचर वर्ण* af_to_name(पूर्णाङ्क id) {

        यदि (id <= 0)
                फर्काऊ रिक्त;

        यदि ((माप_प्रकार) id >= ELEMENTSOF(af_names))
                फर्काऊ रिक्त;

        फर्काऊ af_names[id];
}

अचर वर्ण* af_to_name_short(पूर्णाङ्क id) {
        अचर वर्ण *f;

        यदि (id == AF_UNSPEC)
                फर्काऊ "*";

        f = af_to_name(id);
        यदि (!f)
                फर्काऊ "unknown";

        प्रतिज्ञा(startswith(f, "AF_"));
        फर्काऊ f + 3;
}

पूर्णाङ्क af_from_name(अचर वर्ण *name) {
        माप_प्रकार len = म_माप(ASSERT_PTR(name));

        अचर संरचना af_name *sc = lookup_af(name, len);
        यदि (!sc && len + म_माप("AF_") <= MAX_WORD_LENGTH) {
                अचर वर्ण *t = strjoina("AF_", name);
                sc = lookup_af(t, म_माप("AF_") + len);
        }
        यदि (!sc)
                फर्काऊ -EINVAL;

        फर्काऊ sc->id;
}

पूर्णाङ्क af_max(शून्य) {
        फर्काऊ ELEMENTSOF(af_names);
}

अचर वर्ण* af_to_ipv4_ipv6(पूर्णाङ्क id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        फर्काऊ id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : रिक्त;
}

पूर्णाङ्क af_from_ipv4_ipv6(अचर वर्ण *af) {
        फर्काऊ streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
