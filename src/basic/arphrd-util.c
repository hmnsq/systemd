/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/if_arp.h>
#شامل <linux/if_infiniband.h>
#شامل <netinet/in.h>
#شامل <string.h>

#شامل "arphrd-util.h"

ساکن ثابت ساخت arphrd_name* lookup_arphrd(اندراج ثابت حرف *str, اندراج GPERF_LEN_TYPE len);

#شامل "arphrd-from-name.inc"
#شامل "arphrd-to-name.inc"

صحیح arphrd_from_name(ثابت حرف *name) {
        ثابت ساخت arphrd_name *sc;

        أكد(name);

        sc = lookup_arphrd(name, لڑی_لمبائی(name));
        اگر (!sc)
                واپس -EINVAL;

        واپس sc->id;
}

size_t arphrd_مقصد_hw_addr_len(uint16_t arphrd) {
        انتخاب (arphrd) {
        حالت ARPHRD_ETHER:
                واپس ETH_ALEN;
        حالت ARPHRD_INFINIBAND:
                واپس INFINIBAND_ALEN;
        حالت ARPHRD_TUNNEL:
        حالت ARPHRD_SIT:
        حالت ARPHRD_IPGRE:
                واپس حجم(ساخت in_addr);
        حالت ARPHRD_TUNNEL6:
        حالت ARPHRD_IP6GRE:
                واپس حجم(ساخت in6_addr);
        طےشدہ:
                واپس 0;
        }
}
