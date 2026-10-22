/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/if_arp.h>
#अन्तर्भाव <linux/if_infiniband.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "arphrd-util.h"

स्थिर स्थिरांक संरचना arphrd_name* lookup_arphrd(नोंदणी स्थिरांक अक्षर *str, नोंदणी GPERF_LEN_TYPE len);

#अन्तर्भाव "arphrd-from-name.inc"
#अन्तर्भाव "arphrd-to-name.inc"

पूर्णांक arphrd_from_name(स्थिरांक अक्षर *name) {
        स्थिरांक संरचना arphrd_name *sc;

        प्रतिज्ञा(name);

        sc = lookup_arphrd(name, म_माप(name));
        जर (!sc)
                परत -EINVAL;

        परत sc->id;
}

माप_प्रकार arphrd_to_hw_addr_len(uint16_t arphrd) {
        निवड (arphrd) {
        स्थिती ARPHRD_ETHER:
                परत ETH_ALEN;
        स्थिती ARPHRD_INFINIBAND:
                परत INFINIBAND_ALEN;
        स्थिती ARPHRD_TUNNEL:
        स्थिती ARPHRD_SIT:
        स्थिती ARPHRD_IPGRE:
                परत आकार(संरचना in_addr);
        स्थिती ARPHRD_TUNNEL6:
        स्थिती ARPHRD_IP6GRE:
                परत आकार(संरचना in6_addr);
        पूर्वनिर्धारित:
                परत 0;
        }
}
