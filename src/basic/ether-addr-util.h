/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <linux/if_infiniband.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <net/ethernet.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸಂರಚನಾ hw_addr_data {
        size_t length;
        ಸಙ್ಘ {
                ಸಂರಚನಾ ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                ಸಂರಚನಾ in_addr in;
                ಸಂರಚನಾ in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

ಪೂರ್ಣಾಙ್ಕ parse_hw_addr_full(ಧ್ರುವ ವರ್ಣ *s, size_t expected_len, ಸಂರಚನಾ hw_addr_data *ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ parse_hw_addr(ಧ್ರುವ ವರ್ಣ *s, ಸಂರಚನಾ hw_addr_data *ret) {
        ಪ್ರತ್ಯಾಗಮ parse_hw_addr_full(s, 0, ret);
}
ಪೂರ್ಣಾಙ್ಕ parse_ether_addr(ಧ್ರುವ ವರ್ಣ *s, ಸಂರಚನಾ ether_addr *ret);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
ವರ್ಣ* hw_addr_to_string_full(
                ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                ವರ್ಣ buffer[ಸ್ಥಿರ HW_ADDR_TO_STRING_MAX]);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* hw_addr_to_string(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *addr, ವರ್ಣ buffer[ಸ್ಥಿರ HW_ADDR_TO_STRING_MAX]) {
        ಪ್ರತ್ಯಾಗಮ hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (ವರ್ಣ[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((ಧ್ರುವ ಸಂರಚನಾ hw_addr_data){})

ಸಂರಚನಾ hw_addr_data *hw_addr_set(ಸಂರಚನಾ hw_addr_data *addr, ಧ್ರುವ uint8_t *bytes, size_t length);

ಶೂನ್ಯ hw_addr_hash_func(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *p, ಸಂರಚನಾ siphash *state);
ಪೂರ್ಣಾಙ್ಕ hw_addr_compare(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *a, ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *b);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool hw_addr_equal(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *a, ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *b) {
        ಪ್ರತ್ಯಾಗಮ hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *addr, uint16_t iftype);

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops hw_addr_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
ವರ್ಣ* ether_addr_to_string(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr, ವರ್ಣ buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (ವರ್ಣ[ETHER_ADDR_TO_STRING_MAX]){})

ಪೂರ್ಣಾಙ್ಕ ether_addr_compare(ಧ್ರುವ ಸಂರಚನಾ ether_addr *a, ಧ್ರುವ ಸಂರಚನಾ ether_addr *b);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ether_addr_equal(ಧ್ರುವ ಸಂರಚನಾ ether_addr *a, ಧ್ರುವ ಸಂರಚನಾ ether_addr *b) {
        ಪ್ರತ್ಯಾಗಮ ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((ಧ್ರುವ ಸಂರಚನಾ ether_addr){})

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ether_addr_is_null(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr) {
        ಪ್ರತ್ಯಾಗಮ ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr) _pure_;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ether_addr_is_multicast(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ether_addr_is_unicast(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr) {
        ಪ್ರತ್ಯಾಗಮ !ether_addr_is_multicast(addr);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ether_addr_is_local(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ether_addr_is_global(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr) {
        ಪ್ರತ್ಯಾಗಮ !ether_addr_is_local(addr);
}

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops ether_addr_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops ether_addr_hash_ops_free;

ಶೂನ್ಯ ether_addr_mark_random(ಸಂರಚನಾ ether_addr *addr);

ಪೂರ್ಣಾಙ್ಕ hw_addr_ensure_broadcast(ಸಂರಚನಾ hw_addr_data *bcast_addr, uint16_t arp_type);
