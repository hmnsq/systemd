/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/if_arp.h>
#അന്തര്ഭാവ <linux/if_infiniband.h>
#അന്തര്ഭാവ <netinet/in.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "arphrd-util.h"

സ്ഥിതം സ്ഥിരം ഘടന arphrd_name* lookup_arphrd(രജിസ്റ്റർ സ്ഥിരം അക്ഷരം *str, രജിസ്റ്റർ GPERF_LEN_TYPE len);

#അന്തര്ഭാവ "arphrd-from-name.inc"
#അന്തര്ഭാവ "arphrd-to-name.inc"

പൂർണ്ണസംഖ്യ arphrd_from_name(സ്ഥിരം അക്ഷരം *name) {
        സ്ഥിരം ഘടന arphrd_name *sc;

        പ്രതിജ്ഞാ(name);

        sc = lookup_arphrd(name, strlen(name));
        എങ്കിൽ (!sc)
                തിരികെ -EINVAL;

        തിരികെ sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        തിരഞ്ഞെടുപ്പ് (arphrd) {
        അവസ്ഥ ARPHRD_ETHER:
                തിരികെ ETH_ALEN;
        അവസ്ഥ ARPHRD_INFINIBAND:
                തിരികെ INFINIBAND_ALEN;
        അവസ്ഥ ARPHRD_TUNNEL:
        അവസ്ഥ ARPHRD_SIT:
        അവസ്ഥ ARPHRD_IPGRE:
                തിരികെ വലുപ്പം(ഘടന in_addr);
        അവസ്ഥ ARPHRD_TUNNEL6:
        അവസ്ഥ ARPHRD_IP6GRE:
                തിരികെ വലുപ്പം(ഘടന in6_addr);
        സ്ഥിരസ്ഥിതി:
                തിരികെ 0;
        }
}
