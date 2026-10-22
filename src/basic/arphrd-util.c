/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/if_arp.h>
#अन्तर्भाव <linux/if_infiniband.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "arphrd-util.h"

स्थिर ध्रुव संरचना arphrd_name* lookup_arphrd(पञ्जिका ध्रुव वर्ण *str, पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "arphrd-from-name.inc"
#अन्तर्भाव "arphrd-to-name.inc"

पूर्णाङ्क arphrd_from_name(ध्रुव वर्ण *name) {
        ध्रुव संरचना arphrd_name *sc;

        प्रतिज्ञा(name);

        sc = lookup_arphrd(name, strlen(name));
        सचे (!sc)
                पटिनिवत्त -EINVAL;

        पटिनिवत्त sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        विकल्प (arphrd) {
        पक्ष ARPHRD_ETHER:
                पटिनिवत्त ETH_ALEN;
        पक्ष ARPHRD_INFINIBAND:
                पटिनिवत्त INFINIBAND_ALEN;
        पक्ष ARPHRD_TUNNEL:
        पक्ष ARPHRD_SIT:
        पक्ष ARPHRD_IPGRE:
                पटिनिवत्त परिमाण(संरचना in_addr);
        पक्ष ARPHRD_TUNNEL6:
        पक्ष ARPHRD_IP6GRE:
                पटिनिवत्त परिमाण(संरचना in6_addr);
        स्वभाव:
                पटिनिवत्त 0;
        }
}
