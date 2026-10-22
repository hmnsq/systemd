/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <linux/if_infiniband.h>
#அந்தர்பாவ <netinet/in.h>
#அந்தர்பாவ <net/ethernet.h>

#அந்தர்பாவ "forward.h"

கட்டமைப்பு hw_addr_data {
        size_t length;
        ஒன்றிப்பு {
                கட்டமைப்பு ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                கட்டமைப்பு in_addr in;
                கட்டமைப்பு in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

முழு parse_hw_addr_full(மாறிலி எழுத்து *s, size_t expected_len, கட்டமைப்பு hw_addr_data *ret);
நிலையான உள்வரி முழு parse_hw_addr(மாறிலி எழுத்து *s, கட்டமைப்பு hw_addr_data *ret) {
        திரும்பு parse_hw_addr_full(s, 0, ret);
}
முழு parse_ether_addr(மாறிலி எழுத்து *s, கட்டமைப்பு ether_addr *ret);

வகைவரை எண்ணிக்கை HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
எழுத்து* hw_addr_to_string_full(
                மாறிலி கட்டமைப்பு hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                எழுத்து buffer[நிலையான HW_ADDR_TO_STRING_MAX]);
நிலையான உள்வரி எழுத்து* hw_addr_to_string(மாறிலி கட்டமைப்பு hw_addr_data *addr, எழுத்து buffer[நிலையான HW_ADDR_TO_STRING_MAX]) {
        திரும்பு hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (எழுத்து[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((மாறிலி கட்டமைப்பு hw_addr_data){})

கட்டமைப்பு hw_addr_data *hw_addr_set(கட்டமைப்பு hw_addr_data *addr, மாறிலி uint8_t *bytes, size_t length);

வெறுமை hw_addr_hash_func(மாறிலி கட்டமைப்பு hw_addr_data *p, கட்டமைப்பு siphash *state);
முழு hw_addr_compare(மாறிலி கட்டமைப்பு hw_addr_data *a, மாறிலி கட்டமைப்பு hw_addr_data *b);
நிலையான உள்வரி bool hw_addr_equal(மாறிலி கட்டமைப்பு hw_addr_data *a, மாறிலி கட்டமைப்பு hw_addr_data *b) {
        திரும்பு hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(மாறிலி கட்டமைப்பு hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(மாறிலி கட்டமைப்பு hw_addr_data *addr, uint16_t iftype);

வெளி மாறிலி கட்டமைப்பு hash_ops hw_addr_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
எழுத்து* ether_addr_to_string(மாறிலி கட்டமைப்பு ether_addr *addr, எழுத்து buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (எழுத்து[ETHER_ADDR_TO_STRING_MAX]){})

முழு ether_addr_compare(மாறிலி கட்டமைப்பு ether_addr *a, மாறிலி கட்டமைப்பு ether_addr *b);
நிலையான உள்வரி bool ether_addr_equal(மாறிலி கட்டமைப்பு ether_addr *a, மாறிலி கட்டமைப்பு ether_addr *b) {
        திரும்பு ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((மாறிலி கட்டமைப்பு ether_addr){})

நிலையான உள்வரி bool ether_addr_is_null(மாறிலி கட்டமைப்பு ether_addr *addr) {
        திரும்பு ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(மாறிலி கட்டமைப்பு ether_addr *addr) _pure_;

நிலையான உள்வரி bool ether_addr_is_multicast(மாறிலி கட்டமைப்பு ether_addr *addr) {
        ப்ரதிஜ்ஞா(addr);
        திரும்பு FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

நிலையான உள்வரி bool ether_addr_is_unicast(மாறிலி கட்டமைப்பு ether_addr *addr) {
        திரும்பு !ether_addr_is_multicast(addr);
}

நிலையான உள்வரி bool ether_addr_is_local(மாறிலி கட்டமைப்பு ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        ப்ரதிஜ்ஞா(addr);
        திரும்பு FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

நிலையான உள்வரி bool ether_addr_is_global(மாறிலி கட்டமைப்பு ether_addr *addr) {
        திரும்பு !ether_addr_is_local(addr);
}

வெளி மாறிலி கட்டமைப்பு hash_ops ether_addr_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops ether_addr_hash_ops_free;

வெறுமை ether_addr_mark_random(கட்டமைப்பு ether_addr *addr);

முழு hw_addr_ensure_broadcast(கட்டமைப்பு hw_addr_data *bcast_addr, uint16_t arp_type);
