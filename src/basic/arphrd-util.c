/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/if_arp.h>
#ಅನ್ತರ್ಭಾವ <linux/if_infiniband.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "arphrd-util.h"

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ arphrd_name* lookup_arphrd(ಪಞ್ಜಿಕಾ ಧ್ರುವ ವರ್ಣ *str, ಪಞ್ಜಿಕಾ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "arphrd-from-name.inc"
#ಅನ್ತರ್ಭಾವ "arphrd-to-name.inc"

ಪೂರ್ಣಾಙ್ಕ arphrd_from_name(ಧ್ರುವ ವರ್ಣ *name) {
        ಧ್ರುವ ಸಂರಚನಾ arphrd_name *sc;

        ಪ್ರತಿಜ್ಞಾ(name);

        sc = lookup_arphrd(name, strlen(name));
        ಯದಿ (!sc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        ವಿಕಲ್ಪ (arphrd) {
        ಪಕ್ಷ ARPHRD_ETHER:
                ಪ್ರತ್ಯಾಗಮ ETH_ALEN;
        ಪಕ್ಷ ARPHRD_INFINIBAND:
                ಪ್ರತ್ಯಾಗಮ INFINIBAND_ALEN;
        ಪಕ್ಷ ARPHRD_TUNNEL:
        ಪಕ್ಷ ARPHRD_SIT:
        ಪಕ್ಷ ARPHRD_IPGRE:
                ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr);
        ಪಕ್ಷ ARPHRD_TUNNEL6:
        ಪಕ್ಷ ARPHRD_IP6GRE:
                ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ 0;
        }
}
