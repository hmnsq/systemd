/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if_infiniband.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <net/ethernet.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਬਣਤਰ hw_addr_ਅੰਕੜੇ {
        size_t length;
        ਸੰਘ {
                ਬਣਤਰ ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                ਬਣਤਰ in_addr ਵਿੱਚ;
                ਬਣਤਰ in6_addr in6;
                uint8_t ਬਾਈਟਾਂ[HW_ADDR_MAX_ਮਾਪ];
        };
};

ਪੂਰਨ parse_hw_addr_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, size_t expected_len, ਬਣਤਰ hw_addr_ਅੰਕੜੇ *ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ parse_hw_addr(ਅਚਲ ਅੱਖਰ *s, ਬਣਤਰ hw_addr_ਅੰਕੜੇ *ret) {
        ਵਾਪਸ parse_hw_addr_ਭਰਿਆ(s, 0, ret);
}
ਪੂਰਨ parse_ether_addr(ਅਚਲ ਅੱਖਰ *s, ਬਣਤਰ ether_addr *ret);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Hardware_ਪਤਾ_ਮੰਜ਼ਿਲ_String_Flags {
        HW_ADDR_ਮੰਜ਼ਿਲ_STRING_NO_COLON = 1 << 0,
} Hardware_ਪਤਾ_ਮੰਜ਼ਿਲ_String_Flags;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX (3*HW_ADDR_MAX_ਮਾਪ)
ਅੱਖਰ* hw_addr_ਮੰਜ਼ਿਲ_string_ਭਰਿਆ(
                ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr,
                Hardware_ਪਤਾ_ਮੰਜ਼ਿਲ_String_Flags flags,
                ਅੱਖਰ ਬਫਰ[ਸਥਿਰ HW_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX]);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* hw_addr_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr, ਅੱਖਰ ਬਫਰ[ਸਥਿਰ HW_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX]) {
        ਵਾਪਸ hw_addr_ਮੰਜ਼ਿਲ_string_ਭਰਿਆ(addr, 0, ਬਫਰ);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_ADDR_ਮੰਜ਼ਿਲ_STR_ਭਰਿਆ(hw_addr, flags) hw_addr_ਮੰਜ਼ਿਲ_string_ਭਰਿਆ((hw_addr), flags, (ਅੱਖਰ[HW_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX]){})
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_ADDR_ਮੰਜ਼ਿਲ_STR(hw_addr) HW_ADDR_ਮੰਜ਼ਿਲ_STR_ਭਰਿਆ(hw_addr, 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_ADDR_NULL ((ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ){})

ਬਣਤਰ hw_addr_ਅੰਕੜੇ *hw_addr_ਸੈੱਟ(ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr, ਅਚਲ uint8_t *ਬਾਈਟਾਂ, size_t length);

ਖਾਲੀ hw_addr_hash_func(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *p, ਬਣਤਰ siphash *state);
ਪੂਰਨ hw_addr_compare(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *a, ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *b);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool hw_addr_equal(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *a, ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *b) {
        ਵਾਪਸ hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr) ਸ਼ੁੱਧ;
bool hw_addr_is_valid(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr, uint16_t iftype);

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops hw_addr_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops hw_addr_hash_ops_free;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETHER_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX (3*6)
ਅੱਖਰ* ether_addr_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਬਣਤਰ ether_addr *addr, ਅੱਖਰ ਬਫਰ[ETHER_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETHER_ADDR_ਮੰਜ਼ਿਲ_STR(addr) ether_addr_ਮੰਜ਼ਿਲ_string((addr), (ਅੱਖਰ[ETHER_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX]){})

ਪੂਰਨ ether_addr_compare(ਅਚਲ ਬਣਤਰ ether_addr *a, ਅਚਲ ਬਣਤਰ ether_addr *b);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ether_addr_equal(ਅਚਲ ਬਣਤਰ ether_addr *a, ਅਚਲ ਬਣਤਰ ether_addr *b) {
        ਵਾਪਸ ether_addr_compare(a, b) == 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETHER_ADDR_NULL ((ਅਚਲ ਬਣਤਰ ether_addr){})

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ether_addr_is_null(ਅਚਲ ਬਣਤਰ ether_addr *addr) {
        ਵਾਪਸ ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_ਪ੍ਰਸਾਰਣ(ਅਚਲ ਬਣਤਰ ether_addr *addr) ਸ਼ੁੱਧ;

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ether_addr_is_multicast(ਅਚਲ ਬਣਤਰ ether_addr *addr) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਵਾਪਸ FLAGS_ਸੈੱਟ(addr->ether_addr_octet[0], 0x01);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ether_addr_is_unicast(ਅਚਲ ਬਣਤਰ ether_addr *addr) {
        ਵਾਪਸ !ether_addr_is_multicast(addr);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ether_addr_is_local(ਅਚਲ ਬਣਤਰ ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਵਾਪਸ FLAGS_ਸੈੱਟ(addr->ether_addr_octet[0], 0x02);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ether_addr_is_global(ਅਚਲ ਬਣਤਰ ether_addr *addr) {
        ਵਾਪਸ !ether_addr_is_local(addr);
}

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops ether_addr_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops ether_addr_hash_ops_free;

ਖਾਲੀ ether_addr_mark_random(ਬਣਤਰ ether_addr *addr);

ਪੂਰਨ hw_addr_ensure_ਪ੍ਰਸਾਰਣ(ਬਣਤਰ hw_addr_ਅੰਕੜੇ *bcast_addr, uint16_t arp_type);
