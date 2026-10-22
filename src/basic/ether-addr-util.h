/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <linux/if_infiniband.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <net/ethernet.h>

#अन्तर्भाव "forward.h"

संरचना hw_addr_दत्त {
        size_t length;
        सङ्घ {
                संरचना ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                संरचना in_addr अन्तर्;
                संरचना in6_addr in6;
                uint8_t अष्टकानि[HW_ADDR_MAX_आकार];
        };
};

पूर्णाङ्क parse_hw_addr_पूर्णम्(ध्रुव वर्ण *s, size_t expected_len, संरचना hw_addr_दत्त *ret);
स्थिर अन्तःस्थ पूर्णाङ्क parse_hw_addr(ध्रुव वर्ण *s, संरचना hw_addr_दत्त *ret) {
        प्रत्यागम parse_hw_addr_पूर्णम्(s, 0, ret);
}
पूर्णाङ्क parse_ether_addr(ध्रुव वर्ण *s, संरचना ether_addr *ret);

प्रकारनिर्देश गणना Hardware_स्थानाङ्कः_गन्तव्यम्_String_Flags {
        HW_ADDR_गन्तव्यम्_STRING_NO_COLON = 1 << 0,
} Hardware_स्थानाङ्कः_गन्तव्यम्_String_Flags;

#परिभाषय HW_ADDR_गन्तव्यम्_STRING_MAX (3*HW_ADDR_MAX_आकार)
वर्ण* hw_addr_गन्तव्यम्_string_पूर्णम्(
                ध्रुव संरचना hw_addr_दत्त *addr,
                Hardware_स्थानाङ्कः_गन्तव्यम्_String_Flags flags,
                वर्ण बफर[स्थिर HW_ADDR_गन्तव्यम्_STRING_MAX]);
स्थिर अन्तःस्थ वर्ण* hw_addr_गन्तव्यम्_string(ध्रुव संरचना hw_addr_दत्त *addr, वर्ण बफर[स्थिर HW_ADDR_गन्तव्यम्_STRING_MAX]) {
        प्रत्यागम hw_addr_गन्तव्यम्_string_पूर्णम्(addr, 0, बफर);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#परिभाषय HW_ADDR_गन्तव्यम्_STR_पूर्णम्(hw_addr, flags) hw_addr_गन्तव्यम्_string_पूर्णम्((hw_addr), flags, (वर्ण[HW_ADDR_गन्तव्यम्_STRING_MAX]){})
#परिभाषय HW_ADDR_गन्तव्यम्_STR(hw_addr) HW_ADDR_गन्तव्यम्_STR_पूर्णम्(hw_addr, 0)

#परिभाषय HW_ADDR_NULL ((ध्रुव संरचना hw_addr_दत्त){})

संरचना hw_addr_दत्त *hw_addr_समुच्चयः(संरचना hw_addr_दत्त *addr, ध्रुव uint8_t *अष्टकानि, size_t length);

शून्य hw_addr_hash_func(ध्रुव संरचना hw_addr_दत्त *p, संरचना siphash *state);
पूर्णाङ्क hw_addr_compare(ध्रुव संरचना hw_addr_दत्त *a, ध्रुव संरचना hw_addr_दत्त *b);
स्थिर अन्तःस्थ bool hw_addr_equal(ध्रुव संरचना hw_addr_दत्त *a, ध्रुव संरचना hw_addr_दत्त *b) {
        प्रत्यागम hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ध्रुव संरचना hw_addr_दत्त *addr) विशुद्धम्;
bool hw_addr_is_valid(ध्रुव संरचना hw_addr_दत्त *addr, uint16_t iftype);

बाह्य ध्रुव संरचना hash_ops hw_addr_hash_ops;
बाह्य ध्रुव संरचना hash_ops hw_addr_hash_ops_free;

#परिभाषय ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#परिभाषय ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#परिभाषय ETHER_ADDR_गन्तव्यम्_STRING_MAX (3*6)
वर्ण* ether_addr_गन्तव्यम्_string(ध्रुव संरचना ether_addr *addr, वर्ण बफर[ETHER_ADDR_गन्तव्यम्_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#परिभाषय ETHER_ADDR_गन्तव्यम्_STR(addr) ether_addr_गन्तव्यम्_string((addr), (वर्ण[ETHER_ADDR_गन्तव्यम्_STRING_MAX]){})

पूर्णाङ्क ether_addr_compare(ध्रुव संरचना ether_addr *a, ध्रुव संरचना ether_addr *b);
स्थिर अन्तःस्थ bool ether_addr_equal(ध्रुव संरचना ether_addr *a, ध्रुव संरचना ether_addr *b) {
        प्रत्यागम ether_addr_compare(a, b) == 0;
}

#परिभाषय ETHER_ADDR_NULL ((ध्रुव संरचना ether_addr){})

स्थिर अन्तःस्थ bool ether_addr_is_null(ध्रुव संरचना ether_addr *addr) {
        प्रत्यागम ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_प्रसारणम्(ध्रुव संरचना ether_addr *addr) विशुद्धम्;

स्थिर अन्तःस्थ bool ether_addr_is_multicast(ध्रुव संरचना ether_addr *addr) {
        प्रतिज्ञा(addr);
        प्रत्यागम FLAGS_समुच्चयः(addr->ether_addr_octet[0], 0x01);
}

स्थिर अन्तःस्थ bool ether_addr_is_unicast(ध्रुव संरचना ether_addr *addr) {
        प्रत्यागम !ether_addr_is_multicast(addr);
}

स्थिर अन्तःस्थ bool ether_addr_is_local(ध्रुव संरचना ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        प्रतिज्ञा(addr);
        प्रत्यागम FLAGS_समुच्चयः(addr->ether_addr_octet[0], 0x02);
}

स्थिर अन्तःस्थ bool ether_addr_is_global(ध्रुव संरचना ether_addr *addr) {
        प्रत्यागम !ether_addr_is_local(addr);
}

बाह्य ध्रुव संरचना hash_ops ether_addr_hash_ops;
बाह्य ध्रुव संरचना hash_ops ether_addr_hash_ops_free;

शून्य ether_addr_mark_random(संरचना ether_addr *addr);

पूर्णाङ्क hw_addr_ensure_प्रसारणम्(संरचना hw_addr_दत्त *bcast_addr, uint16_t arp_type);
