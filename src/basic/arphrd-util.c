/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <linux/if_arp.h>
#অন্তর্ভাৱ <linux/if_infiniband.h>
#অন্তর্ভাৱ <netinet/in.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "arphrd-util.h"

স্থির ধ্রুৱ সংরচনা arphrd_name* lookup_arphrd(পঞ্জিকা ধ্রুৱ ৱর্ণ *str, পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাৱ "arphrd-from-name.inc"
#অন্তর্ভাৱ "arphrd-to-name.inc"

পূর্ণাঙ্ক arphrd_from_name(ধ্রুৱ ৱর্ণ *name) {
        ধ্রুৱ সংরচনা arphrd_name *sc;

        প্রতিজ্ঞা(name);

        sc = lookup_arphrd(name, strlen(name));
        যদি (!sc)
                ঘূৰাই -EINVAL;

        ঘূৰাই sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        ৱিকল্প (arphrd) {
        পক্ষ ARPHRD_ETHER:
                ঘূৰাই ETH_ALEN;
        পক্ষ ARPHRD_INFINIBAND:
                ঘূৰাই INFINIBAND_ALEN;
        পক্ষ ARPHRD_TUNNEL:
        পক্ষ ARPHRD_SIT:
        পক্ষ ARPHRD_IPGRE:
                ঘূৰাই পরিমাণ(সংরচনা in_addr);
        পক্ষ ARPHRD_TUNNEL6:
        পক্ষ ARPHRD_IP6GRE:
                ঘূৰাই পরিমাণ(সংরচনা in6_addr);
        স্ৱভাৱ:
                ঘূৰাই 0;
        }
}
