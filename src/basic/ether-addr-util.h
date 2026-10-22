/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <linux/if_infiniband.h>
#అన్తర్భావ <netinet/in.h>
#అన్తర్భావ <net/ethernet.h>

#అన్తర్భావ "forward.h"

నిర్మాణం hw_addr_దత్తాంశం {
        size_t length;
        సంఘం {
                నిర్మాణం ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                నిర్మాణం in_addr లోపల;
                నిర్మాణం in6_addr in6;
                uint8_t బైట్లు[HW_ADDR_MAX_కొలత];
        };
};

పూర్ణాంకం parse_hw_addr_నిండిన(స్థిరరాశి అక్షరం *s, size_t expected_len, నిర్మాణం hw_addr_దత్తాంశం *ret);
స్థిర అంతఃవరుస పూర్ణాంకం parse_hw_addr(స్థిరరాశి అక్షరం *s, నిర్మాణం hw_addr_దత్తాంశం *ret) {
        తిరిగి parse_hw_addr_నిండిన(s, 0, ret);
}
పూర్ణాంకం parse_ether_addr(స్థిరరాశి అక్షరం *s, నిర్మాణం ether_addr *ret);

రకనిర్వచనం గణన Hardware_చిరునామా_గమ్యం_String_Flags {
        HW_ADDR_గమ్యం_STRING_NO_COLON = 1 << 0,
} Hardware_చిరునామా_గమ్యం_String_Flags;

#నిర్వచించు HW_ADDR_గమ్యం_STRING_MAX (3*HW_ADDR_MAX_కొలత)
అక్షరం* hw_addr_గమ్యం_string_నిండిన(
                స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *addr,
                Hardware_చిరునామా_గమ్యం_String_Flags flags,
                అక్షరం బఫరు[స్థిర HW_ADDR_గమ్యం_STRING_MAX]);
స్థిర అంతఃవరుస అక్షరం* hw_addr_గమ్యం_string(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *addr, అక్షరం బఫరు[స్థిర HW_ADDR_గమ్యం_STRING_MAX]) {
        తిరిగి hw_addr_గమ్యం_string_నిండిన(addr, 0, బఫరు);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#నిర్వచించు HW_ADDR_గమ్యం_STR_నిండిన(hw_addr, flags) hw_addr_గమ్యం_string_నిండిన((hw_addr), flags, (అక్షరం[HW_ADDR_గమ్యం_STRING_MAX]){})
#నిర్వచించు HW_ADDR_గమ్యం_STR(hw_addr) HW_ADDR_గమ్యం_STR_నిండిన(hw_addr, 0)

#నిర్వచించు HW_ADDR_NULL ((స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం){})

నిర్మాణం hw_addr_దత్తాంశం *hw_addr_సమితి(నిర్మాణం hw_addr_దత్తాంశం *addr, స్థిరరాశి uint8_t *బైట్లు, size_t length);

శూన్య hw_addr_hash_func(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *p, నిర్మాణం siphash *state);
పూర్ణాంకం hw_addr_compare(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *a, స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *b);
స్థిర అంతఃవరుస bool hw_addr_equal(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *a, స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *b) {
        తిరిగి hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *addr) శుద్ధ;
bool hw_addr_is_valid(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *addr, uint16_t iftype);

బాహ్య స్థిరరాశి నిర్మాణం hash_ops hw_addr_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops hw_addr_hash_ops_free;

#నిర్వచించు ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#నిర్వచించు ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#నిర్వచించు ETHER_ADDR_గమ్యం_STRING_MAX (3*6)
అక్షరం* ether_addr_గమ్యం_string(స్థిరరాశి నిర్మాణం ether_addr *addr, అక్షరం బఫరు[ETHER_ADDR_గమ్యం_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#నిర్వచించు ETHER_ADDR_గమ్యం_STR(addr) ether_addr_గమ్యం_string((addr), (అక్షరం[ETHER_ADDR_గమ్యం_STRING_MAX]){})

పూర్ణాంకం ether_addr_compare(స్థిరరాశి నిర్మాణం ether_addr *a, స్థిరరాశి నిర్మాణం ether_addr *b);
స్థిర అంతఃవరుస bool ether_addr_equal(స్థిరరాశి నిర్మాణం ether_addr *a, స్థిరరాశి నిర్మాణం ether_addr *b) {
        తిరిగి ether_addr_compare(a, b) == 0;
}

#నిర్వచించు ETHER_ADDR_NULL ((స్థిరరాశి నిర్మాణం ether_addr){})

స్థిర అంతఃవరుస bool ether_addr_is_null(స్థిరరాశి నిర్మాణం ether_addr *addr) {
        తిరిగి ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_ప్రసారం(స్థిరరాశి నిర్మాణం ether_addr *addr) శుద్ధ;

స్థిర అంతఃవరుస bool ether_addr_is_multicast(స్థిరరాశి నిర్మాణం ether_addr *addr) {
        ప్రతిజ్ఞా(addr);
        తిరిగి FLAGS_సమితి(addr->ether_addr_octet[0], 0x01);
}

స్థిర అంతఃవరుస bool ether_addr_is_unicast(స్థిరరాశి నిర్మాణం ether_addr *addr) {
        తిరిగి !ether_addr_is_multicast(addr);
}

స్థిర అంతఃవరుస bool ether_addr_is_local(స్థిరరాశి నిర్మాణం ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        ప్రతిజ్ఞా(addr);
        తిరిగి FLAGS_సమితి(addr->ether_addr_octet[0], 0x02);
}

స్థిర అంతఃవరుస bool ether_addr_is_global(స్థిరరాశి నిర్మాణం ether_addr *addr) {
        తిరిగి !ether_addr_is_local(addr);
}

బాహ్య స్థిరరాశి నిర్మాణం hash_ops ether_addr_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops ether_addr_hash_ops_free;

శూన్య ether_addr_mark_random(నిర్మాణం ether_addr *addr);

పూర్ణాంకం hw_addr_ensure_ప్రసారం(నిర్మాణం hw_addr_దత్తాంశం *bcast_addr, uint16_t arp_type);
