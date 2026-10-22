/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/if_arp.h>
#அந்தர்பாவ <linux/if_infiniband.h>
#அந்தர்பாவ <netinet/in.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "arphrd-util.h"

நிலையான மாறிலி கட்டமைப்பு arphrd_name* lookup_arphrd(பதிவு மாறிலி எழுத்து *str, பதிவு GPERF_LEN_TYPE len);

#அந்தர்பாவ "arphrd-from-name.inc"
#அந்தர்பாவ "arphrd-to-name.inc"

முழு arphrd_from_name(மாறிலி எழுத்து *name) {
        மாறிலி கட்டமைப்பு arphrd_name *sc;

        ப்ரதிஜ்ஞா(name);

        sc = lookup_arphrd(name, strlen(name));
        எனில் (!sc)
                திரும்பு -EINVAL;

        திரும்பு sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        தேர்வு (arphrd) {
        நிலை ARPHRD_ETHER:
                திரும்பு ETH_ALEN;
        நிலை ARPHRD_INFINIBAND:
                திரும்பு INFINIBAND_ALEN;
        நிலை ARPHRD_TUNNEL:
        நிலை ARPHRD_SIT:
        நிலை ARPHRD_IPGRE:
                திரும்பு அளவு(கட்டமைப்பு in_addr);
        நிலை ARPHRD_TUNNEL6:
        நிலை ARPHRD_IP6GRE:
                திரும்பு அளவு(கட்டமைப்பு in6_addr);
        இயல்பு:
                திரும்பு 0;
        }
}
