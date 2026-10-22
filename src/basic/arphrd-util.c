/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/if_arp.h>
#అన్తర్భావ <linux/if_infiniband.h>
#అన్తర్భావ <netinet/in.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "arphrd-util.h"

స్థిర స్థిరరాశి నిర్మాణం arphrd_name* lookup_arphrd(నమోదు స్థిరరాశి అక్షరం *str, నమోదు GPERF_LEN_TYPE len);

#అన్తర్భావ "arphrd-from-name.inc"
#అన్తర్భావ "arphrd-to-name.inc"

పూర్ణాంకం arphrd_from_name(స్థిరరాశి అక్షరం *name) {
        స్థిరరాశి నిర్మాణం arphrd_name *sc;

        ప్రతిజ్ఞా(name);

        sc = lookup_arphrd(name, అక్షరశ్రేణి_పొడవు(name));
        ఒకవేళ (!sc)
                తిరిగి -EINVAL;

        తిరిగి sc->id;
}

size_t arphrd_గమ్యం_hw_addr_len(uint16_t arphrd) {
        ఎంపిక (arphrd) {
        స్థితి ARPHRD_ETHER:
                తిరిగి ETH_ALEN;
        స్థితి ARPHRD_INFINIBAND:
                తిరిగి INFINIBAND_ALEN;
        స్థితి ARPHRD_TUNNEL:
        స్థితి ARPHRD_SIT:
        స్థితి ARPHRD_IPGRE:
                తిరిగి పరిమాణం(నిర్మాణం in_addr);
        స్థితి ARPHRD_TUNNEL6:
        స్థితి ARPHRD_IP6GRE:
                తిరిగి పరిమాణం(నిర్మాణం in6_addr);
        అప్రమేయ:
                తిరిగి 0;
        }
}
