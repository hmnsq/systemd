/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/if_arp.h>
#अन्तर्भाव <linux/if_infiniband.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "arphrd-util.h"

स्थिर अचर संरचना arphrd_name* lookup_arphrd(पञ्जिका अचर वर्ण *str, पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "arphrd-from-name.inc"
#अन्तर्भाव "arphrd-to-name.inc"

पूर्णाङ्क arphrd_from_name(अचर वर्ण *name) {
        अचर संरचना arphrd_name *sc;

        प्रतिज्ञा(name);

        sc = lookup_arphrd(name, म_माप(name));
        यदि (!sc)
                फर्काऊ -EINVAL;

        फर्काऊ sc->id;
}

माप_प्रकार arphrd_to_hw_addr_len(uint16_t arphrd) {
        विकल्प (arphrd) {
        पक्ष ARPHRD_ETHER:
                फर्काऊ ETH_ALEN;
        पक्ष ARPHRD_INFINIBAND:
                फर्काऊ INFINIBAND_ALEN;
        पक्ष ARPHRD_TUNNEL:
        पक्ष ARPHRD_SIT:
        पक्ष ARPHRD_IPGRE:
                फर्काऊ परिमाण(संरचना in_addr);
        पक्ष ARPHRD_TUNNEL6:
        पक्ष ARPHRD_IP6GRE:
                फर्काऊ परिमाण(संरचना in6_addr);
        स्वभाव:
                फर्काऊ 0;
        }
}
