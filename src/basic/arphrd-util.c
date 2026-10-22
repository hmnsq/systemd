/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/if_arp.h>
#ಅನ್ತರ್ಭಾವ <linux/if_infiniband.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "arphrd-util.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ arphrd_name* lookup_arphrd(ನೋಂದಣಿ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ನೋಂದಣಿ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "arphrd-from-name.inc"
#ಅನ್ತರ್ಭಾವ "arphrd-to-name.inc"

ಪೂರ್ಣಾಂಕ arphrd_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ arphrd_name *sc;

        ಪ್ರತಿಜ್ಞಾ(name);

        sc = lookup_arphrd(name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
        ಆದರೆ (!sc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ sc->id;
}

size_t arphrd_ಗಮ್ಯ_hw_addr_len(uint16_t arphrd) {
        ಆಯ್ಕೆ (arphrd) {
        ಸ್ಥಿತಿ ARPHRD_ETHER:
                ಹಿಂತಿರುಗಿ ETH_ALEN;
        ಸ್ಥಿತಿ ARPHRD_INFINIBAND:
                ಹಿಂತಿರುಗಿ INFINIBAND_ALEN;
        ಸ್ಥಿತಿ ARPHRD_TUNNEL:
        ಸ್ಥಿತಿ ARPHRD_SIT:
        ಸ್ಥಿತಿ ARPHRD_IPGRE:
                ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ರಚನೆ in_addr);
        ಸ್ಥಿತಿ ARPHRD_TUNNEL6:
        ಸ್ಥಿತಿ ARPHRD_IP6GRE:
                ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ರಚನೆ in6_addr);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ 0;
        }
}
