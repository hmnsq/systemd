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

        sc = lookup_arphrd(name, वाक्यदैर्घ्यम्(name));
        यदि (!sc)
                प्रत्यागम -EINVAL;

        प्रत्यागम sc->id;
}

size_t arphrd_गन्तव्यम्_hw_addr_len(uint16_t arphrd) {
        विकल्प (arphrd) {
        पक्ष ARPHRD_ETHER:
                प्रत्यागम ETH_ALEN;
        पक्ष ARPHRD_INFINIBAND:
                प्रत्यागम INFINIBAND_ALEN;
        पक्ष ARPHRD_TUNNEL:
        पक्ष ARPHRD_SIT:
        पक्ष ARPHRD_IPGRE:
                प्रत्यागम परिमाण(संरचना in_addr);
        पक्ष ARPHRD_TUNNEL6:
        पक्ष ARPHRD_IP6GRE:
                प्रत्यागम परिमाण(संरचना in6_addr);
        स्वभाव:
                प्रत्यागम 0;
        }
}
