/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <linux/if_infiniband.h>
#অন্তর্ভাৱ <netinet/in.h>
#অন্তর্ভাৱ <net/ethernet.h>

#অন্তর্ভাৱ "forward.h"

সংরচনা hw_addr_data {
        size_t length;
        সঙ্ঘ {
                সংরচনা ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                সংরচনা in_addr in;
                সংরচনা in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

পূর্ণাঙ্ক parse_hw_addr_full(ধ্রুৱ ৱর্ণ *s, size_t expected_len, সংরচনা hw_addr_data *ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক parse_hw_addr(ধ্রুৱ ৱর্ণ *s, সংরচনা hw_addr_data *ret) {
        ঘূৰাই parse_hw_addr_full(s, 0, ret);
}
পূর্ণাঙ্ক parse_ether_addr(ধ্রুৱ ৱর্ণ *s, সংরচনা ether_addr *ret);

প্রকারনির্দেশ গণনা HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
ৱর্ণ* hw_addr_to_string_full(
                ধ্রুৱ সংরচনা hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                ৱর্ণ buffer[স্থির HW_ADDR_TO_STRING_MAX]);
স্থির অন্তঃস্থ ৱর্ণ* hw_addr_to_string(ধ্রুৱ সংরচনা hw_addr_data *addr, ৱর্ণ buffer[স্থির HW_ADDR_TO_STRING_MAX]) {
        ঘূৰাই hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (ৱর্ণ[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((ধ্রুৱ সংরচনা hw_addr_data){})

সংরচনা hw_addr_data *hw_addr_set(সংরচনা hw_addr_data *addr, ধ্রুৱ uint8_t *bytes, size_t length);

শূন্য hw_addr_hash_func(ধ্রুৱ সংরচনা hw_addr_data *p, সংরচনা siphash *state);
পূর্ণাঙ্ক hw_addr_compare(ধ্রুৱ সংরচনা hw_addr_data *a, ধ্রুৱ সংরচনা hw_addr_data *b);
স্থির অন্তঃস্থ bool hw_addr_equal(ধ্রুৱ সংরচনা hw_addr_data *a, ধ্রুৱ সংরচনা hw_addr_data *b) {
        ঘূৰাই hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ধ্রুৱ সংরচনা hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(ধ্রুৱ সংরচনা hw_addr_data *addr, uint16_t iftype);

বাহ্য ধ্রুৱ সংরচনা hash_ops hw_addr_hash_ops;
বাহ্য ধ্রুৱ সংরচনা hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
ৱর্ণ* ether_addr_to_string(ধ্রুৱ সংরচনা ether_addr *addr, ৱর্ণ buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (ৱর্ণ[ETHER_ADDR_TO_STRING_MAX]){})

পূর্ণাঙ্ক ether_addr_compare(ধ্রুৱ সংরচনা ether_addr *a, ধ্রুৱ সংরচনা ether_addr *b);
স্থির অন্তঃস্থ bool ether_addr_equal(ধ্রুৱ সংরচনা ether_addr *a, ধ্রুৱ সংরচনা ether_addr *b) {
        ঘূৰাই ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((ধ্রুৱ সংরচনা ether_addr){})

স্থির অন্তঃস্থ bool ether_addr_is_null(ধ্রুৱ সংরচনা ether_addr *addr) {
        ঘূৰাই ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(ধ্রুৱ সংরচনা ether_addr *addr) _pure_;

স্থির অন্তঃস্থ bool ether_addr_is_multicast(ধ্রুৱ সংরচনা ether_addr *addr) {
        প্রতিজ্ঞা(addr);
        ঘূৰাই FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

স্থির অন্তঃস্থ bool ether_addr_is_unicast(ধ্রুৱ সংরচনা ether_addr *addr) {
        ঘূৰাই !ether_addr_is_multicast(addr);
}

স্থির অন্তঃস্থ bool ether_addr_is_local(ধ্রুৱ সংরচনা ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        প্রতিজ্ঞা(addr);
        ঘূৰাই FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

স্থির অন্তঃস্থ bool ether_addr_is_global(ধ্রুৱ সংরচনা ether_addr *addr) {
        ঘূৰাই !ether_addr_is_local(addr);
}

বাহ্য ধ্রুৱ সংরচনা hash_ops ether_addr_hash_ops;
বাহ্য ধ্রুৱ সংরচনা hash_ops ether_addr_hash_ops_free;

শূন্য ether_addr_mark_random(সংরচনা ether_addr *addr);

পূর্ণাঙ্ক hw_addr_ensure_broadcast(সংরচনা hw_addr_data *bcast_addr, uint16_t arp_type);
