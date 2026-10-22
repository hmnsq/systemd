/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/if_arp.h>
#include <linux/if_infiniband.h>
#include <netinet/in.h>
#include <string.h>

#include "arphrd-util.h"

static const sa_nracanaa arphrd_name* lookup_arphrd(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "arphrd-from-name.inc"
#include "arphrd-to-name.inc"

ཧྲིལ arphrd_from_name(const var_na *name) {
        const sa_nracanaa arphrd_name *sc;

        assert(name);

        sc = lookup_arphrd(name, strlen(name));
        གལ་ཏེ (!sc)
                ཕྱིར -EINVAL;

        ཕྱིར sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        switch (arphrd) {
        pak_sa ARPHRD_ETHER:
                ཕྱིར ETH_ALEN;
        pak_sa ARPHRD_INFINIBAND:
                ཕྱིར INFINIBAND_ALEN;
        pak_sa ARPHRD_TUNNEL:
        pak_sa ARPHRD_SIT:
        pak_sa ARPHRD_IPGRE:
                ཕྱིར parimaa_na(sa_nracanaa in_addr);
        pak_sa ARPHRD_TUNNEL6:
        pak_sa ARPHRD_IP6GRE:
                ཕྱིར parimaa_na(sa_nracanaa in6_addr);
        default:
                ཕྱིར 0;
        }
}
