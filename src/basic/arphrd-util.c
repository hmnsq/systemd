/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/if_arp.h>
#समावेश <linux/if_infiniband.h>
#समावेश <netinet/in.h>
#समावेश <string.h>

#समावेश "arphrd-util.h"

अटल स्थिर काष्ठा arphrd_name* lookup_arphrd(रेजिस्टर स्थिर अक्षर *str, रेजिस्टर GPERF_LEN_TYPE len);

#समावेश "arphrd-from-name.inc"
#समावेश "arphrd-to-name.inc"

पूर्णांक arphrd_from_name(स्थिर अक्षर *name) {
        स्थिर काष्ठा arphrd_name *sc;

        निश्चित(name);

        sc = lookup_arphrd(name, म_माप(name));
        अगर (!sc)
                वापस -EINVAL;

        वापस sc->id;
}

माप_प्रकार arphrd_to_hw_addr_len(uint16_t arphrd) {
        चयन (arphrd) {
        हाल ARPHRD_ETHER:
                वापस ETH_ALEN;
        हाल ARPHRD_INFINIBAND:
                वापस INFINIBAND_ALEN;
        हाल ARPHRD_TUNNEL:
        हाल ARPHRD_SIT:
        हाल ARPHRD_IPGRE:
                वापस माप(काष्ठा in_addr);
        हाल ARPHRD_TUNNEL6:
        हाल ARPHRD_IP6GRE:
                वापस माप(काष्ठा in6_addr);
        शेष:
                वापस 0;
        }
}
