/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/if_arp.h>
#ଅନ୍ତର୍ଭାଵ <linux/if_infiniband.h>
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "arphrd-util.h"

ସ୍ଥିର ଅଚଳ ସଂରଚନା arphrd_name* lookup_arphrd(ପଞ୍ଜିକରଣ ଅଚଳ ଅକ୍ଷର *str, ପଞ୍ଜିକରଣ GPERF_LEN_TYPE len);

#ଅନ୍ତର୍ଭାଵ "arphrd-from-name.inc"
#ଅନ୍ତର୍ଭାଵ "arphrd-to-name.inc"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା arphrd_from_name(ଅଚଳ ଅକ୍ଷର *name) {
        ଅଚଳ ସଂରଚନା arphrd_name *sc;

        ପ୍ରତିଜ୍ଞା(name);

        sc = lookup_arphrd(name, strlen(name));
        ଯଦି (!sc)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        ବାଛ (arphrd) {
        ସ୍ଥିତି ARPHRD_ETHER:
                ଫେରନ୍ତୁ ETH_ALEN;
        ସ୍ଥିତି ARPHRD_INFINIBAND:
                ଫେରନ୍ତୁ INFINIBAND_ALEN;
        ସ୍ଥିତି ARPHRD_TUNNEL:
        ସ୍ଥିତି ARPHRD_SIT:
        ସ୍ଥିତି ARPHRD_IPGRE:
                ଫେରନ୍ତୁ ଆକାର(ସଂରଚନା in_addr);
        ସ୍ଥିତି ARPHRD_TUNNEL6:
        ସ୍ଥିତି ARPHRD_IP6GRE:
                ଫେରନ୍ତୁ ଆକାର(ସଂରଚନା in6_addr);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ 0;
        }
}
