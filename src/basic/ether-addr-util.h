/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <linux/if_infiniband.h>
#অন্তর্ভাব <netinet/in.h>
#অন্তর্ভাব <net/ethernet.h>

#অন্তর্ভাব "forward.h"

কাঠামো hw_addr_তথ্য {
        size_t length;
        সংঘ {
                কাঠামো ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                কাঠামো in_addr in;
                কাঠামো in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_আকার];
        };
};

পূর্ণসংখ্যা parse_hw_addr_full(ধ্রুবক অক্ষর *s, size_t expected_len, কাঠামো hw_addr_তথ্য *ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা parse_hw_addr(ধ্রুবক অক্ষর *s, কাঠামো hw_addr_তথ্য *ret) {
        ফেরত parse_hw_addr_full(s, 0, ret);
}
পূর্ণসংখ্যা parse_ether_addr(ধ্রুবক অক্ষর *s, কাঠামো ether_addr *ret);

প্রকারলেখ গণনা HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_আকার)
অক্ষর* hw_addr_to_string_full(
                ধ্রুবক কাঠামো hw_addr_তথ্য *addr,
                HardwareAddressToStringFlags flags,
                অক্ষর বাফার[স্থির HW_ADDR_TO_STRING_MAX]);
স্থির অন্তঃরেখ অক্ষর* hw_addr_to_string(ধ্রুবক কাঠামো hw_addr_তথ্য *addr, অক্ষর বাফার[স্থির HW_ADDR_TO_STRING_MAX]) {
        ফেরত hw_addr_to_string_full(addr, 0, বাফার);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (অক্ষর[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((ধ্রুবক কাঠামো hw_addr_তথ্য){})

কাঠামো hw_addr_তথ্য *hw_addr_set(কাঠামো hw_addr_তথ্য *addr, ধ্রুবক uint8_t *bytes, size_t length);

শূন্য hw_addr_hash_func(ধ্রুবক কাঠামো hw_addr_তথ্য *p, কাঠামো siphash *state);
পূর্ণসংখ্যা hw_addr_compare(ধ্রুবক কাঠামো hw_addr_তথ্য *a, ধ্রুবক কাঠামো hw_addr_তথ্য *b);
স্থির অন্তঃরেখ bool hw_addr_equal(ধ্রুবক কাঠামো hw_addr_তথ্য *a, ধ্রুবক কাঠামো hw_addr_তথ্য *b) {
        ফেরত hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ধ্রুবক কাঠামো hw_addr_তথ্য *addr) _pure_;
bool hw_addr_is_valid(ধ্রুবক কাঠামো hw_addr_তথ্য *addr, uint16_t iftype);

বাহ্যিক ধ্রুবক কাঠামো hash_ops hw_addr_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
অক্ষর* ether_addr_to_string(ধ্রুবক কাঠামো ether_addr *addr, অক্ষর বাফার[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (অক্ষর[ETHER_ADDR_TO_STRING_MAX]){})

পূর্ণসংখ্যা ether_addr_compare(ধ্রুবক কাঠামো ether_addr *a, ধ্রুবক কাঠামো ether_addr *b);
স্থির অন্তঃরেখ bool ether_addr_equal(ধ্রুবক কাঠামো ether_addr *a, ধ্রুবক কাঠামো ether_addr *b) {
        ফেরত ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((ধ্রুবক কাঠামো ether_addr){})

স্থির অন্তঃরেখ bool ether_addr_is_null(ধ্রুবক কাঠামো ether_addr *addr) {
        ফেরত ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(ধ্রুবক কাঠামো ether_addr *addr) _pure_;

স্থির অন্তঃরেখ bool ether_addr_is_multicast(ধ্রুবক কাঠামো ether_addr *addr) {
        প্রতিজ্ঞা(addr);
        ফেরত FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

স্থির অন্তঃরেখ bool ether_addr_is_unicast(ধ্রুবক কাঠামো ether_addr *addr) {
        ফেরত !ether_addr_is_multicast(addr);
}

স্থির অন্তঃরেখ bool ether_addr_is_local(ধ্রুবক কাঠামো ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        প্রতিজ্ঞা(addr);
        ফেরত FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

স্থির অন্তঃরেখ bool ether_addr_is_global(ধ্রুবক কাঠামো ether_addr *addr) {
        ফেরত !ether_addr_is_local(addr);
}

বাহ্যিক ধ্রুবক কাঠামো hash_ops ether_addr_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops ether_addr_hash_ops_free;

শূন্য ether_addr_mark_random(কাঠামো ether_addr *addr);

পূর্ণসংখ্যা hw_addr_ensure_broadcast(কাঠামো hw_addr_তথ্য *bcast_addr, uint16_t arp_type);
