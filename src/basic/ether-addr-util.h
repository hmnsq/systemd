/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <linux/if_infiniband.h>
#ضمن <netinet/in.h>
#ضمن <net/ethernet.h>

#ضمن "forward.h"

بنية hw_addr_انکڑے {
        size_t length;
        اتحاد {
                بنية ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                بنية in_addr وچ;
                بنية in6_addr in6;
                uint8_t بائٹاں[HW_ADDR_MAX_ناپ];
        };
};

صحيح parse_hw_addr_بھریا(ثابت حرف *s, size_t expected_len, بنية hw_addr_انکڑے *ret);
ساكن مضمن صحيح parse_hw_addr(ثابت حرف *s, بنية hw_addr_انکڑے *ret) {
        أرجع parse_hw_addr_بھریا(s, 0, ret);
}
صحيح parse_ether_addr(ثابت حرف *s, بنية ether_addr *ret);

تعريفنوع تعداد Hardware_پتہ_منزل_String_Flags {
        HW_ADDR_منزل_STRING_NO_COLON = 1 << 0,
} Hardware_پتہ_منزل_String_Flags;

#تعریف_کرو HW_ADDR_منزل_STRING_MAX (3*HW_ADDR_MAX_ناپ)
حرف* hw_addr_منزل_string_بھریا(
                ثابت بنية hw_addr_انکڑے *addr,
                Hardware_پتہ_منزل_String_Flags flags,
                حرف بفر[ساكن HW_ADDR_منزل_STRING_MAX]);
ساكن مضمن حرف* hw_addr_منزل_string(ثابت بنية hw_addr_انکڑے *addr, حرف بفر[ساكن HW_ADDR_منزل_STRING_MAX]) {
        أرجع hw_addr_منزل_string_بھریا(addr, 0, بفر);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو HW_ADDR_منزل_STR_بھریا(hw_addr, flags) hw_addr_منزل_string_بھریا((hw_addr), flags, (حرف[HW_ADDR_منزل_STRING_MAX]){})
#تعریف_کرو HW_ADDR_منزل_STR(hw_addr) HW_ADDR_منزل_STR_بھریا(hw_addr, 0)

#تعریف_کرو HW_ADDR_NULL ((ثابت بنية hw_addr_انکڑے){})

بنية hw_addr_انکڑے *hw_addr_سیٹ(بنية hw_addr_انکڑے *addr, ثابت uint8_t *بائٹاں, size_t length);

فراغ hw_addr_hash_func(ثابت بنية hw_addr_انکڑے *p, بنية siphash *state);
صحيح hw_addr_compare(ثابت بنية hw_addr_انکڑے *a, ثابت بنية hw_addr_انکڑے *b);
ساكن مضمن bool hw_addr_equal(ثابت بنية hw_addr_انکڑے *a, ثابت بنية hw_addr_انکڑے *b) {
        أرجع hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ثابت بنية hw_addr_انکڑے *addr) خالص;
bool hw_addr_is_valid(ثابت بنية hw_addr_انکڑے *addr, uint16_t iftype);

خارجي ثابت بنية hash_ops hw_addr_hash_ops;
خارجي ثابت بنية hash_ops hw_addr_hash_ops_free;

#تعریف_کرو ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#تعریف_کرو ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#تعریف_کرو ETHER_ADDR_منزل_STRING_MAX (3*6)
حرف* ether_addr_منزل_string(ثابت بنية ether_addr *addr, حرف بفر[ETHER_ADDR_منزل_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#تعریف_کرو ETHER_ADDR_منزل_STR(addr) ether_addr_منزل_string((addr), (حرف[ETHER_ADDR_منزل_STRING_MAX]){})

صحيح ether_addr_compare(ثابت بنية ether_addr *a, ثابت بنية ether_addr *b);
ساكن مضمن bool ether_addr_equal(ثابت بنية ether_addr *a, ثابت بنية ether_addr *b) {
        أرجع ether_addr_compare(a, b) == 0;
}

#تعریف_کرو ETHER_ADDR_NULL ((ثابت بنية ether_addr){})

ساكن مضمن bool ether_addr_is_null(ثابت بنية ether_addr *addr) {
        أرجع ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_نشر(ثابت بنية ether_addr *addr) خالص;

ساكن مضمن bool ether_addr_is_multicast(ثابت بنية ether_addr *addr) {
        أكد(addr);
        أرجع FLAGS_سیٹ(addr->ether_addr_octet[0], 0x01);
}

ساكن مضمن bool ether_addr_is_unicast(ثابت بنية ether_addr *addr) {
        أرجع !ether_addr_is_multicast(addr);
}

ساكن مضمن bool ether_addr_is_local(ثابت بنية ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        أكد(addr);
        أرجع FLAGS_سیٹ(addr->ether_addr_octet[0], 0x02);
}

ساكن مضمن bool ether_addr_is_global(ثابت بنية ether_addr *addr) {
        أرجع !ether_addr_is_local(addr);
}

خارجي ثابت بنية hash_ops ether_addr_hash_ops;
خارجي ثابت بنية hash_ops ether_addr_hash_ops_free;

فراغ ether_addr_mark_random(بنية ether_addr *addr);

صحيح hw_addr_ensure_نشر(بنية hw_addr_انکڑے *bcast_addr, uint16_t arp_type);
