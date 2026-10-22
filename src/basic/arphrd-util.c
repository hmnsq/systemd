/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/if_arp.h>
#include <linux/if_infiniband.h>
#include <netinet/in.h>
#include <string.h>

#include "arphrd-util.h"

static const struct arphrd_name* lookup_arphrd(register const char *str, register GPERF_LEN_TYPE len);

#include "arphrd-from-name.inc"
#include "arphrd-to-name.inc"

整数 arphrd_from_name(const char *name) {
        const struct arphrd_name *sc;

        assert(name);

        sc = lookup_arphrd(name, strlen(name));
        もし (!sc)
                戻る -EINVAL;

        戻る sc->id;
}

size_t arphrd_to_hw_addr_len(uint16_t arphrd) {
        switch (arphrd) {
        case ARPHRD_ETHER:
                戻る ETH_ALEN;
        case ARPHRD_INFINIBAND:
                戻る INFINIBAND_ALEN;
        case ARPHRD_TUNNEL:
        case ARPHRD_SIT:
        case ARPHRD_IPGRE:
                戻る sizeof(struct in_addr);
        case ARPHRD_TUNNEL6:
        case ARPHRD_IP6GRE:
                戻る sizeof(struct in6_addr);
        default:
                戻る 0;
        }
}
