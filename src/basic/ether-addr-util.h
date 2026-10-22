/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <linux/if_infiniband.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <net/ethernet.h>

#अन्तर्भाव "forward.h"

संरचना hw_addr_data {
        माप_प्रकार length;
        सङ्घ {
                संरचना ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                संरचना in_addr in;
                संरचना in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

पूर्णाङ्क parse_hw_addr_full(ध्रुव वर्ण *s, माप_प्रकार expected_len, संरचना hw_addr_data *ret);
स्थिर अन्तःस्थ पूर्णाङ्क parse_hw_addr(ध्रुव वर्ण *s, संरचना hw_addr_data *ret) {
        प्रत्यागम parse_hw_addr_full(s, 0, ret);
}
पूर्णाङ्क parse_ether_addr(ध्रुव वर्ण *s, संरचना ether_addr *ret);

प्रकारनिर्देश गणना HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
वर्ण* hw_addr_to_string_full(
                ध्रुव संरचना hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                वर्ण buffer[स्थिर HW_ADDR_TO_STRING_MAX]);
स्थिर अन्तःस्थ वर्ण* hw_addr_to_string(ध्रुव संरचना hw_addr_data *addr, वर्ण buffer[स्थिर HW_ADDR_TO_STRING_MAX]) {
        प्रत्यागम hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (वर्ण[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((ध्रुव संरचना hw_addr_data){})

संरचना hw_addr_data *hw_addr_set(संरचना hw_addr_data *addr, ध्रुव uint8_t *bytes, माप_प्रकार length);

शून्य hw_addr_hash_func(ध्रुव संरचना hw_addr_data *p, संरचना siphash *state);
पूर्णाङ्क hw_addr_compare(ध्रुव संरचना hw_addr_data *a, ध्रुव संरचना hw_addr_data *b);
स्थिर अन्तःस्थ bool hw_addr_equal(ध्रुव संरचना hw_addr_data *a, ध्रुव संरचना hw_addr_data *b) {
        प्रत्यागम hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ध्रुव संरचना hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(ध्रुव संरचना hw_addr_data *addr, uint16_t iftype);

बाह्य ध्रुव संरचना hash_ops hw_addr_hash_ops;
बाह्य ध्रुव संरचना hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
वर्ण* ether_addr_to_string(ध्रुव संरचना ether_addr *addr, वर्ण buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (वर्ण[ETHER_ADDR_TO_STRING_MAX]){})

पूर्णाङ्क ether_addr_compare(ध्रुव संरचना ether_addr *a, ध्रुव संरचना ether_addr *b);
स्थिर अन्तःस्थ bool ether_addr_equal(ध्रुव संरचना ether_addr *a, ध्रुव संरचना ether_addr *b) {
        प्रत्यागम ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((ध्रुव संरचना ether_addr){})

स्थिर अन्तःस्थ bool ether_addr_is_null(ध्रुव संरचना ether_addr *addr) {
        प्रत्यागम ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(ध्रुव संरचना ether_addr *addr) _pure_;

स्थिर अन्तःस्थ bool ether_addr_is_multicast(ध्रुव संरचना ether_addr *addr) {
        प्रतिज्ञा(addr);
        प्रत्यागम FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

स्थिर अन्तःस्थ bool ether_addr_is_unicast(ध्रुव संरचना ether_addr *addr) {
        प्रत्यागम !ether_addr_is_multicast(addr);
}

स्थिर अन्तःस्थ bool ether_addr_is_local(ध्रुव संरचना ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        प्रतिज्ञा(addr);
        प्रत्यागम FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

स्थिर अन्तःस्थ bool ether_addr_is_global(ध्रुव संरचना ether_addr *addr) {
        प्रत्यागम !ether_addr_is_local(addr);
}

बाह्य ध्रुव संरचना hash_ops ether_addr_hash_ops;
बाह्य ध्रुव संरचना hash_ops ether_addr_hash_ops_free;

शून्य ether_addr_mark_random(संरचना ether_addr *addr);

पूर्णाङ्क hw_addr_ensure_broadcast(संरचना hw_addr_data *bcast_addr, uint16_t arp_type);
