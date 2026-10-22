/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <linux/if_infiniband.h>
#ضمن <netinet/in.h>
#ضمن <net/ethernet.h>

#ضمن "forward.h"

بنية hw_addr_data {
        size_t length;
        اتحاد {
                بنية ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                بنية in_addr in;
                بنية in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

صحيح parse_hw_addr_full(ثابت حرف *s, size_t expected_len, بنية hw_addr_data *ret);
ساكن مضمن صحيح parse_hw_addr(ثابت حرف *s, بنية hw_addr_data *ret) {
        أرجع parse_hw_addr_full(s, 0, ret);
}
صحيح parse_ether_addr(ثابت حرف *s, بنية ether_addr *ret);

تعريفنوع تعداد HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
حرف* hw_addr_to_string_full(
                ثابت بنية hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                حرف buffer[ساكن HW_ADDR_TO_STRING_MAX]);
ساكن مضمن حرف* hw_addr_to_string(ثابت بنية hw_addr_data *addr, حرف buffer[ساكن HW_ADDR_TO_STRING_MAX]) {
        أرجع hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (حرف[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((ثابت بنية hw_addr_data){})

بنية hw_addr_data *hw_addr_set(بنية hw_addr_data *addr, ثابت uint8_t *bytes, size_t length);

فراغ hw_addr_hash_func(ثابت بنية hw_addr_data *p, بنية siphash *state);
صحيح hw_addr_compare(ثابت بنية hw_addr_data *a, ثابت بنية hw_addr_data *b);
ساكن مضمن bool hw_addr_equal(ثابت بنية hw_addr_data *a, ثابت بنية hw_addr_data *b) {
        أرجع hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ثابت بنية hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(ثابت بنية hw_addr_data *addr, uint16_t iftype);

خارجي ثابت بنية hash_ops hw_addr_hash_ops;
خارجي ثابت بنية hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
حرف* ether_addr_to_string(ثابت بنية ether_addr *addr, حرف buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (حرف[ETHER_ADDR_TO_STRING_MAX]){})

صحيح ether_addr_compare(ثابت بنية ether_addr *a, ثابت بنية ether_addr *b);
ساكن مضمن bool ether_addr_equal(ثابت بنية ether_addr *a, ثابت بنية ether_addr *b) {
        أرجع ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((ثابت بنية ether_addr){})

ساكن مضمن bool ether_addr_is_null(ثابت بنية ether_addr *addr) {
        أرجع ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(ثابت بنية ether_addr *addr) _pure_;

ساكن مضمن bool ether_addr_is_multicast(ثابت بنية ether_addr *addr) {
        أكد(addr);
        أرجع FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

ساكن مضمن bool ether_addr_is_unicast(ثابت بنية ether_addr *addr) {
        أرجع !ether_addr_is_multicast(addr);
}

ساكن مضمن bool ether_addr_is_local(ثابت بنية ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        أكد(addr);
        أرجع FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

ساكن مضمن bool ether_addr_is_global(ثابت بنية ether_addr *addr) {
        أرجع !ether_addr_is_local(addr);
}

خارجي ثابت بنية hash_ops ether_addr_hash_ops;
خارجي ثابت بنية hash_ops ether_addr_hash_ops_free;

فراغ ether_addr_mark_random(بنية ether_addr *addr);

صحيح hw_addr_ensure_broadcast(بنية hw_addr_data *bcast_addr, uint16_t arp_type);
