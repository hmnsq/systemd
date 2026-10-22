/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <linux/if_infiniband.h>
#समावेश <netinet/in.h>
#समावेश <net/ethernet.h>

#समावेश "forward.h"

काष्ठा hw_addr_तथ्य {
        माप_प्रकार length;
        जोड़ {
                काष्ठा ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                काष्ठा in_addr in;
                काष्ठा in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_आकार];
        };
};

पूर्णांक parse_hw_addr_full(स्थिर अक्षर *s, माप_प्रकार expected_len, काष्ठा hw_addr_तथ्य *ret);
अटल अंतरभूत पूर्णांक parse_hw_addr(स्थिर अक्षर *s, काष्ठा hw_addr_तथ्य *ret) {
        वापस parse_hw_addr_full(s, 0, ret);
}
पूर्णांक parse_ether_addr(स्थिर अक्षर *s, काष्ठा ether_addr *ret);

प्रकार क्रमागत HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#घोषणा HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_आकार)
अक्षर* hw_addr_to_string_full(
                स्थिर काष्ठा hw_addr_तथ्य *addr,
                HardwareAddressToStringFlags flags,
                अक्षर बफर[अटल HW_ADDR_TO_STRING_MAX]);
अटल अंतरभूत अक्षर* hw_addr_to_string(स्थिर काष्ठा hw_addr_तथ्य *addr, अक्षर बफर[अटल HW_ADDR_TO_STRING_MAX]) {
        वापस hw_addr_to_string_full(addr, 0, बफर);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#घोषणा HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (अक्षर[HW_ADDR_TO_STRING_MAX]){})
#घोषणा HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#घोषणा HW_ADDR_NULL ((स्थिर काष्ठा hw_addr_तथ्य){})

काष्ठा hw_addr_तथ्य *hw_addr_set(काष्ठा hw_addr_तथ्य *addr, स्थिर uint8_t *bytes, माप_प्रकार length);

व्योम hw_addr_hash_func(स्थिर काष्ठा hw_addr_तथ्य *p, काष्ठा siphash *state);
पूर्णांक hw_addr_compare(स्थिर काष्ठा hw_addr_तथ्य *a, स्थिर काष्ठा hw_addr_तथ्य *b);
अटल अंतरभूत bool hw_addr_equal(स्थिर काष्ठा hw_addr_तथ्य *a, स्थिर काष्ठा hw_addr_तथ्य *b) {
        वापस hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(स्थिर काष्ठा hw_addr_तथ्य *addr) _pure_;
bool hw_addr_is_valid(स्थिर काष्ठा hw_addr_तथ्य *addr, uint16_t iftype);

बाह्य स्थिर काष्ठा hash_ops hw_addr_hash_ops;
बाह्य स्थिर काष्ठा hash_ops hw_addr_hash_ops_free;

#घोषणा ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#घोषणा ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#घोषणा ETHER_ADDR_TO_STRING_MAX (3*6)
अक्षर* ether_addr_to_string(स्थिर काष्ठा ether_addr *addr, अक्षर बफर[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#घोषणा ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (अक्षर[ETHER_ADDR_TO_STRING_MAX]){})

पूर्णांक ether_addr_compare(स्थिर काष्ठा ether_addr *a, स्थिर काष्ठा ether_addr *b);
अटल अंतरभूत bool ether_addr_equal(स्थिर काष्ठा ether_addr *a, स्थिर काष्ठा ether_addr *b) {
        वापस ether_addr_compare(a, b) == 0;
}

#घोषणा ETHER_ADDR_NULL ((स्थिर काष्ठा ether_addr){})

अटल अंतरभूत bool ether_addr_is_null(स्थिर काष्ठा ether_addr *addr) {
        वापस ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(स्थिर काष्ठा ether_addr *addr) _pure_;

अटल अंतरभूत bool ether_addr_is_multicast(स्थिर काष्ठा ether_addr *addr) {
        निश्चित(addr);
        वापस FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

अटल अंतरभूत bool ether_addr_is_unicast(स्थिर काष्ठा ether_addr *addr) {
        वापस !ether_addr_is_multicast(addr);
}

अटल अंतरभूत bool ether_addr_is_local(स्थिर काष्ठा ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        निश्चित(addr);
        वापस FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

अटल अंतरभूत bool ether_addr_is_global(स्थिर काष्ठा ether_addr *addr) {
        वापस !ether_addr_is_local(addr);
}

बाह्य स्थिर काष्ठा hash_ops ether_addr_hash_ops;
बाह्य स्थिर काष्ठा hash_ops ether_addr_hash_ops_free;

व्योम ether_addr_mark_random(काष्ठा ether_addr *addr);

पूर्णांक hw_addr_ensure_broadcast(काष्ठा hw_addr_तथ्य *bcast_addr, uint16_t arp_type);
