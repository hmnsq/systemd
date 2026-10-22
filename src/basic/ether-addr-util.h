/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <linux/if_infiniband.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <net/ethernet.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ರಚನೆ hw_addr_ದತ್ತಾಂಶ {
        size_t length;
        ಒಕ್ಕೂಟ {
                ರಚನೆ ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                ರಚನೆ in_addr ಒಳ;
                ರಚನೆ in6_addr in6;
                uint8_t ಬೈಟುಗಳು[HW_ADDR_MAX_ಗಾತ್ರ];
        };
};

ಪೂರ್ಣಾಂಕ parse_hw_addr_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t expected_len, ರಚನೆ hw_addr_ದತ್ತಾಂಶ *ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ parse_hw_addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ರಚನೆ hw_addr_ದತ್ತಾಂಶ *ret) {
        ಹಿಂತಿರುಗಿ parse_hw_addr_ತುಂಬಿದ(s, 0, ret);
}
ಪೂರ್ಣಾಂಕ parse_ether_addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ರಚನೆ ether_addr *ret);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Hardware_ವಿಳಾಸ_ಗಮ್ಯ_String_Flags {
        HW_ADDR_ಗಮ್ಯ_STRING_NO_COLON = 1 << 0,
} Hardware_ವಿಳಾಸ_ಗಮ್ಯ_String_Flags;

#ವ್ಯಾಖ್ಯಾನಿಸು HW_ADDR_ಗಮ್ಯ_STRING_MAX (3*HW_ADDR_MAX_ಗಾತ್ರ)
ಅಕ್ಷರ* hw_addr_ಗಮ್ಯ_string_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr,
                Hardware_ವಿಳಾಸ_ಗಮ್ಯ_String_Flags flags,
                ಅಕ್ಷರ ಬಫರ್[ಸ್ಥಿರ HW_ADDR_ಗಮ್ಯ_STRING_MAX]);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* hw_addr_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr, ಅಕ್ಷರ ಬಫರ್[ಸ್ಥಿರ HW_ADDR_ಗಮ್ಯ_STRING_MAX]) {
        ಹಿಂತಿರುಗಿ hw_addr_ಗಮ್ಯ_string_ತುಂಬಿದ(addr, 0, ಬಫರ್);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ವ್ಯಾಖ್ಯಾನಿಸು HW_ADDR_ಗಮ್ಯ_STR_ತುಂಬಿದ(hw_addr, flags) hw_addr_ಗಮ್ಯ_string_ತುಂಬಿದ((hw_addr), flags, (ಅಕ್ಷರ[HW_ADDR_ಗಮ್ಯ_STRING_MAX]){})
#ವ್ಯಾಖ್ಯಾನಿಸು HW_ADDR_ಗಮ್ಯ_STR(hw_addr) HW_ADDR_ಗಮ್ಯ_STR_ತುಂಬಿದ(hw_addr, 0)

#ವ್ಯಾಖ್ಯಾನಿಸು HW_ADDR_NULL ((ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ){})

ರಚನೆ hw_addr_ದತ್ತಾಂಶ *hw_addr_ಗಣ(ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr, ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು, size_t length);

ಶೂನ್ಯ hw_addr_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *p, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ hw_addr_compare(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *a, ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *b);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool hw_addr_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *a, ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *b) {
        ಹಿಂತಿರುಗಿ hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr) ಶುದ್ಧ;
bool hw_addr_is_valid(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr, uint16_t iftype);

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops hw_addr_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops hw_addr_hash_ops_free;

#ವ್ಯಾಖ್ಯಾನಿಸು ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#ವ್ಯಾಖ್ಯಾನಿಸು ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#ವ್ಯಾಖ್ಯಾನಿಸು ETHER_ADDR_ಗಮ್ಯ_STRING_MAX (3*6)
ಅಕ್ಷರ* ether_addr_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr, ಅಕ್ಷರ ಬಫರ್[ETHER_ADDR_ಗಮ್ಯ_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#ವ್ಯಾಖ್ಯಾನಿಸು ETHER_ADDR_ಗಮ್ಯ_STR(addr) ether_addr_ಗಮ್ಯ_string((addr), (ಅಕ್ಷರ[ETHER_ADDR_ಗಮ್ಯ_STRING_MAX]){})

ಪೂರ್ಣಾಂಕ ether_addr_compare(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *b);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ether_addr_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *b) {
        ಹಿಂತಿರುಗಿ ether_addr_compare(a, b) == 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ETHER_ADDR_NULL ((ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr){})

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ether_addr_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr) {
        ಹಿಂತಿರುಗಿ ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_ಪ್ರಸಾರ(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr) ಶುದ್ಧ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ether_addr_is_multicast(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(addr->ether_addr_octet[0], 0x01);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ether_addr_is_unicast(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr) {
        ಹಿಂತಿರುಗಿ !ether_addr_is_multicast(addr);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ether_addr_is_local(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(addr->ether_addr_octet[0], 0x02);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ether_addr_is_global(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr) {
        ಹಿಂತಿರುಗಿ !ether_addr_is_local(addr);
}

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops ether_addr_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops ether_addr_hash_ops_free;

ಶೂನ್ಯ ether_addr_mark_random(ರಚನೆ ether_addr *addr);

ಪೂರ್ಣಾಂಕ hw_addr_ensure_ಪ್ರಸಾರ(ರಚನೆ hw_addr_ದತ್ತಾಂಶ *bcast_addr, uint16_t arp_type);
