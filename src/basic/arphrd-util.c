/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <linux/if_arp.h>
#অন্তর্ভাব <linux/if_infiniband.h>
#অন্তর্ভাব <netinet/in.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "arphrd-util.h"

স্থির ধ্রুবক কাঠামো arphrd_name* lookup_arphrd(নিবন্ধ ধ্রুবক অক্ষর *str, নিবন্ধ GPERF_LEN_TYPE len);

#অন্তর্ভাব "arphrd-from-name.inc"
#অন্তর্ভাব "arphrd-to-name.inc"

পূর্ণসংখ্যা arphrd_from_name(ধ্রুবক অক্ষর *name) {
        ধ্রুবক কাঠামো arphrd_name *sc;

        প্রতিজ্ঞা(name);

        sc = lookup_arphrd(name, strlen(name));
        যদি (!sc)
                ফেরত -EINVAL;

        ফেরত sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        নির্বাচন (arphrd) {
        ক্ষেত্র ARPHRD_ETHER:
                ফেরত ETH_ALEN;
        ক্ষেত্র ARPHRD_INFINIBAND:
                ফেরত INFINIBAND_ALEN;
        ক্ষেত্র ARPHRD_TUNNEL:
        ক্ষেত্র ARPHRD_SIT:
        ক্ষেত্র ARPHRD_IPGRE:
                ফেরত আকার(কাঠামো in_addr);
        ক্ষেত্র ARPHRD_TUNNEL6:
        ক্ষেত্র ARPHRD_IP6GRE:
                ফেরত আকার(কাঠামো in6_addr);
        পূর্বনির্ধারিত:
                ফেরত 0;
        }
}
