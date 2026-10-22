/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/if_arp.h>
#ضمن <linux/if_infiniband.h>
#ضمن <netinet/in.h>
#ضمن <string.h>

#ضمن "arphrd-util.h"

ساكن ثابت بنية arphrd_name* lookup_arphrd(سجل ثابت حرف *str, سجل GPERF_LEN_TYPE len);

#ضمن "arphrd-from-name.inc"
#ضمن "arphrd-to-name.inc"

صحيح arphrd_from_name(ثابت حرف *name) {
        ثابت بنية arphrd_name *sc;

        أكد(name);

        sc = lookup_arphrd(name, strlen(name));
        إذا (!sc)
                إرجاع -EINVAL;

        إرجاع sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        بدل (arphrd) {
        حالة ARPHRD_ETHER:
                إرجاع ETH_ALEN;
        حالة ARPHRD_INFINIBAND:
                إرجاع INFINIBAND_ALEN;
        حالة ARPHRD_TUNNEL:
        حالة ARPHRD_SIT:
        حالة ARPHRD_IPGRE:
                إرجاع حجم(بنية in_addr);
        حالة ARPHRD_TUNNEL6:
        حالة ARPHRD_IP6GRE:
                إرجاع حجم(بنية in6_addr);
        افتراضي:
                إرجاع 0;
        }
}
