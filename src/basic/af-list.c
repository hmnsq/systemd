/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <string.h>
#समावेश <sys/socket.h>

#समावेश "af-list.h"
#समावेश "string-util.h"

अटल स्थिर काष्ठा af_name* lookup_af(रेजिस्टर स्थिर अक्षर *str, रेजिस्टर GPERF_LEN_TYPE len);

#समावेश "af-from-name.inc"
#समावेश "af-to-name.inc"

स्थिर अक्षर* af_to_name(पूर्णांक id) {

        अगर (id <= 0)
                वापस शून्य;

        अगर ((माप_प्रकार) id >= ELEMENTSOF(af_names))
                वापस शून्य;

        वापस af_names[id];
}

स्थिर अक्षर* af_to_name_short(पूर्णांक id) {
        स्थिर अक्षर *f;

        अगर (id == AF_UNSPEC)
                वापस "*";

        f = af_to_name(id);
        अगर (!f)
                वापस "unknown";

        निश्चित(startswith(f, "AF_"));
        वापस f + 3;
}

पूर्णांक af_from_name(स्थिर अक्षर *name) {
        माप_प्रकार len = म_माप(ASSERT_PTR(name));

        स्थिर काष्ठा af_name *sc = lookup_af(name, len);
        अगर (!sc && len + म_माप("AF_") <= MAX_WORD_लंबाई) {
                स्थिर अक्षर *t = strjoina("AF_", name);
                sc = lookup_af(t, म_माप("AF_") + len);
        }
        अगर (!sc)
                वापस -EINVAL;

        वापस sc->id;
}

पूर्णांक af_max(व्योम) {
        वापस ELEMENTSOF(af_names);
}

स्थिर अक्षर* af_to_ipv4_ipv6(पूर्णांक id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        वापस id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : शून्य;
}

पूर्णांक af_from_ipv4_ipv6(स्थिर अक्षर *af) {
        वापस streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
