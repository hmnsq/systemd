/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <linux/if_arp.h>
#અન્તર્ભાવ <linux/if_infiniband.h>
#અન્તર્ભાવ <netinet/in.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "arphrd-util.h"

સ્થિર અચલ સંરચના arphrd_name* lookup_arphrd(નોંધણી અચલ અક્ષર *str, નોંધણી GPERF_LEN_TYPE len);

#અન્તર્ભાવ "arphrd-from-name.inc"
#અન્તર્ભાવ "arphrd-to-name.inc"

પૂર્ણાંક arphrd_from_name(અચલ અક્ષર *name) {
        અચલ સંરચના arphrd_name *sc;

        પ્રતિજ્ઞા(name);

        sc = lookup_arphrd(name, strlen(name));
        જો (!sc)
                પાછા -EINVAL;

        પાછા sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        પસંદગી (arphrd) {
        સ્થિતિ ARPHRD_ETHER:
                પાછા ETH_ALEN;
        સ્થિતિ ARPHRD_INFINIBAND:
                પાછા INFINIBAND_ALEN;
        સ્થિતિ ARPHRD_TUNNEL:
        સ્થિતિ ARPHRD_SIT:
        સ્થિતિ ARPHRD_IPGRE:
                પાછા કદ(સંરચના in_addr);
        સ્થિતિ ARPHRD_TUNNEL6:
        સ્થિતિ ARPHRD_IP6GRE:
                પાછા કદ(સંરચના in6_addr);
        મૂળભૂત:
                પાછા 0;
        }
}
