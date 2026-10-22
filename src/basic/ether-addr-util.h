/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <linux/if_infiniband.h>
#અન્તર્ભાવ <netinet/in.h>
#અન્તર્ભાવ <net/ethernet.h>

#અન્તર્ભાવ "forward.h"

સંરચના hw_addr_data {
        size_t length;
        સંઘ {
                સંરચના ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                સંરચના in_addr in;
                સંરચના in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

પૂર્ણાંક parse_hw_addr_full(અચલ અક્ષર *s, size_t expected_len, સંરચના hw_addr_data *ret);
સ્થિર અંતઃરેખ પૂર્ણાંક parse_hw_addr(અચલ અક્ષર *s, સંરચના hw_addr_data *ret) {
        પાછા parse_hw_addr_full(s, 0, ret);
}
પૂર્ણાંક parse_ether_addr(અચલ અક્ષર *s, સંરચના ether_addr *ret);

પ્રકારવ્યાખ્યા ગણના HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
અક્ષર* hw_addr_to_string_full(
                અચલ સંરચના hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                અક્ષર buffer[સ્થિર HW_ADDR_TO_STRING_MAX]);
સ્થિર અંતઃરેખ અક્ષર* hw_addr_to_string(અચલ સંરચના hw_addr_data *addr, અક્ષર buffer[સ્થિર HW_ADDR_TO_STRING_MAX]) {
        પાછા hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (અક્ષર[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((અચલ સંરચના hw_addr_data){})

સંરચના hw_addr_data *hw_addr_set(સંરચના hw_addr_data *addr, અચલ uint8_t *bytes, size_t length);

શૂન્ય hw_addr_hash_func(અચલ સંરચના hw_addr_data *p, સંરચના siphash *state);
પૂર્ણાંક hw_addr_compare(અચલ સંરચના hw_addr_data *a, અચલ સંરચના hw_addr_data *b);
સ્થિર અંતઃરેખ bool hw_addr_equal(અચલ સંરચના hw_addr_data *a, અચલ સંરચના hw_addr_data *b) {
        પાછા hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(અચલ સંરચના hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(અચલ સંરચના hw_addr_data *addr, uint16_t iftype);

બાહ્ય અચલ સંરચના hash_ops hw_addr_hash_ops;
બાહ્ય અચલ સંરચના hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
અક્ષર* ether_addr_to_string(અચલ સંરચના ether_addr *addr, અક્ષર buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (અક્ષર[ETHER_ADDR_TO_STRING_MAX]){})

પૂર્ણાંક ether_addr_compare(અચલ સંરચના ether_addr *a, અચલ સંરચના ether_addr *b);
સ્થિર અંતઃરેખ bool ether_addr_equal(અચલ સંરચના ether_addr *a, અચલ સંરચના ether_addr *b) {
        પાછા ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((અચલ સંરચના ether_addr){})

સ્થિર અંતઃરેખ bool ether_addr_is_null(અચલ સંરચના ether_addr *addr) {
        પાછા ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(અચલ સંરચના ether_addr *addr) _pure_;

સ્થિર અંતઃરેખ bool ether_addr_is_multicast(અચલ સંરચના ether_addr *addr) {
        પ્રતિજ્ઞા(addr);
        પાછા FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

સ્થિર અંતઃરેખ bool ether_addr_is_unicast(અચલ સંરચના ether_addr *addr) {
        પાછા !ether_addr_is_multicast(addr);
}

સ્થિર અંતઃરેખ bool ether_addr_is_local(અચલ સંરચના ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        પ્રતિજ્ઞા(addr);
        પાછા FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

સ્થિર અંતઃરેખ bool ether_addr_is_global(અચલ સંરચના ether_addr *addr) {
        પાછા !ether_addr_is_local(addr);
}

બાહ્ય અચલ સંરચના hash_ops ether_addr_hash_ops;
બાહ્ય અચલ સંરચના hash_ops ether_addr_hash_ops_free;

શૂન્ય ether_addr_mark_random(સંરચના ether_addr *addr);

પૂર્ણાંક hw_addr_ensure_broadcast(સંરચના hw_addr_data *bcast_addr, uint16_t arp_type);
