/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/socket.h>

#अन्तर्भाव "af-list.h"
#अन्तर्भाव "string-util.h"

स्थिर स्थिरांक संरचना af_name* lookup_af(नोंदणी स्थिरांक अक्षर *str, नोंदणी GPERF_LEN_TYPE len);

#अन्तर्भाव "af-from-name.inc"
#अन्तर्भाव "af-to-name.inc"

स्थिरांक अक्षर* af_to_name(पूर्णांक id) {

        जर (id <= 0)
                परत रिक्त;

        जर ((माप_प्रकार) id >= ELEMENTSOF(af_names))
                परत रिक्त;

        परत af_names[id];
}

स्थिरांक अक्षर* af_to_name_short(पूर्णांक id) {
        स्थिरांक अक्षर *f;

        जर (id == AF_UNSPEC)
                परत "*";

        f = af_to_name(id);
        जर (!f)
                परत "unknown";

        प्रतिज्ञा(startswith(f, "AF_"));
        परत f + 3;
}

पूर्णांक af_from_name(स्थिरांक अक्षर *name) {
        माप_प्रकार len = म_माप(ASSERT_PTR(name));

        स्थिरांक संरचना af_name *sc = lookup_af(name, len);
        जर (!sc && len + म_माप("AF_") <= MAX_WORD_LENGTH) {
                स्थिरांक अक्षर *t = strjoina("AF_", name);
                sc = lookup_af(t, म_माप("AF_") + len);
        }
        जर (!sc)
                परत -EINVAL;

        परत sc->id;
}

पूर्णांक af_max(शून्य) {
        परत ELEMENTSOF(af_names);
}

स्थिरांक अक्षर* af_to_ipv4_ipv6(पूर्णांक id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        परत id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : रिक्त;
}

पूर्णांक af_from_ipv4_ipv6(स्थिरांक अक्षर *af) {
        परत streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
