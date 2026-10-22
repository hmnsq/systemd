/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/if_arp.h>
#شامل <linux/if_infiniband.h>
#شامل <netinet/in.h>
#شامل <string.h>

#شامل "arphrd-util.h"

ایستا ثابت ساختار arphrd_name* lookup_arphrd(ثبات ثابت نویسه *str, ثبات GPERF_LEN_TYPE len);

#شامل "arphrd-from-name.inc"
#شامل "arphrd-to-name.inc"

صحیح arphrd_from_name(ثابت نویسه *name) {
        ثابت ساختار arphrd_name *sc;

        أكد(name);

        sc = lookup_arphrd(name, strlen(name));
        اگر (!sc)
                بازگشت -EINVAL;

        بازگشت sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        انتخاب (arphrd) {
        حالت ARPHRD_ETHER:
                بازگشت ETH_ALEN;
        حالت ARPHRD_INFINIBAND:
                بازگشت INFINIBAND_ALEN;
        حالت ARPHRD_TUNNEL:
        حالت ARPHRD_SIT:
        حالت ARPHRD_IPGRE:
                بازگشت اندازه(ساختار in_addr);
        حالت ARPHRD_TUNNEL6:
        حالت ARPHRD_IP6GRE:
                بازگشت اندازه(ساختار in6_addr);
        پیشفرض:
                بازگشت 0;
        }
}
