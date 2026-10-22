/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <linux/if_arp.h>
#অন্তর্ভাব <linux/if_infiniband.h>
#অন্তর্ভাব <netinet/in.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "arphrd-util.h"

স্থির ধ্রুব সংরচনা arphrd_name* lookup_arphrd(পঞ্জিকা ধ্রুব বর্ণ *str, পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাব "arphrd-from-name.inc"
#অন্তর্ভাব "arphrd-to-name.inc"

পূর্ণাঙ্ক arphrd_from_name(ধ্রুব বর্ণ *name) {
        ধ্রুব সংরচনা arphrd_name *sc;

        প্রতিজ্ঞা(name);

        sc = lookup_arphrd(name, strlen(name));
        যদি (!sc)
                প্রত্যাগম -EINVAL;

        প্রত্যাগম sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        বিকল্প (arphrd) {
        পক্ষ ARPHRD_ETHER:
                প্রত্যাগম ETH_ALEN;
        পক্ষ ARPHRD_INFINIBAND:
                প্রত্যাগম INFINIBAND_ALEN;
        পক্ষ ARPHRD_TUNNEL:
        পক্ষ ARPHRD_SIT:
        পক্ষ ARPHRD_IPGRE:
                প্রত্যাগম পরিমাণ(সংরচনা in_addr);
        পক্ষ ARPHRD_TUNNEL6:
        পক্ষ ARPHRD_IP6GRE:
                প্রত্যাগম পরিমাণ(সংরচনা in6_addr);
        স্বভাব:
                প্রত্যাগম 0;
        }
}
