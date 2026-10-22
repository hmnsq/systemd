/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if_arp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if_infiniband.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "arphrd-util.h"

ਸਥਿਰ ਅਚਲ ਬਣਤਰ arphrd_name* lookup_arphrd(ਰਜਿਸਟਰ ਅਚਲ ਅੱਖਰ *str, ਰਜਿਸਟਰ GPERF_LEN_TYPE len);

#ਅਨ੍ਤਰ੍ਭਾਵ "arphrd-from-name.inc"
#ਅਨ੍ਤਰ੍ਭਾਵ "arphrd-to-name.inc"

ਪੂਰਨ arphrd_from_name(ਅਚਲ ਅੱਖਰ *name) {
        ਅਚਲ ਬਣਤਰ arphrd_name *sc;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        sc = lookup_arphrd(name, ਲੜੀ_ਲੰਬਾਈ(name));
        ਜੇ (!sc)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ sc->id;
}

size_t arphrd_ਮੰਜ਼ਿਲ_hw_addr_len(uint16_t arphrd) {
        ਚੋਣ (arphrd) {
        ਹਾਲਤ ARPHRD_ETHER:
                ਵਾਪਸ ETH_ALEN;
        ਹਾਲਤ ARPHRD_INFINIBAND:
                ਵਾਪਸ INFINIBAND_ALEN;
        ਹਾਲਤ ARPHRD_TUNNEL:
        ਹਾਲਤ ARPHRD_SIT:
        ਹਾਲਤ ARPHRD_IPGRE:
                ਵਾਪਸ ਆਕਾਰ(ਬਣਤਰ in_addr);
        ਹਾਲਤ ARPHRD_TUNNEL6:
        ਹਾਲਤ ARPHRD_IP6GRE:
                ਵਾਪਸ ਆਕਾਰ(ਬਣਤਰ in6_addr);
        ਮੂਲ:
                ਵਾਪਸ 0;
        }
}
