/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <linux/if_infiniband.h>
#ضمن <netinet/in.h>
#ضمن <net/ethernet.h>

#ضمن "forward.h"

بنية hw_addr_داده {
        size_t length;
        اتحاد {
                بنية ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                بنية in_addr درون;
                بنية in6_addr in6;
                uint8_t بایتها[HW_ADDR_MAX_اندازه];
        };
};

صحيح parse_hw_addr_پر(ثابت حرف *s, size_t expected_len, بنية hw_addr_داده *ret);
ساكن مضمن صحيح parse_hw_addr(ثابت حرف *s, بنية hw_addr_داده *ret) {
        أرجع parse_hw_addr_پر(s, 0, ret);
}
صحيح parse_ether_addr(ثابت حرف *s, بنية ether_addr *ret);

تعريفنوع تعداد Hardware_آدرس_مقصد_String_Flags {
        HW_ADDR_مقصد_STRING_NO_COLON = 1 << 0,
} Hardware_آدرس_مقصد_String_Flags;

#تعریف_کن HW_ADDR_مقصد_STRING_MAX (3*HW_ADDR_MAX_اندازه)
حرف* hw_addr_مقصد_string_پر(
                ثابت بنية hw_addr_داده *addr,
                Hardware_آدرس_مقصد_String_Flags flags,
                حرف میانگیر[ساكن HW_ADDR_مقصد_STRING_MAX]);
ساكن مضمن حرف* hw_addr_مقصد_string(ثابت بنية hw_addr_داده *addr, حرف میانگیر[ساكن HW_ADDR_مقصد_STRING_MAX]) {
        أرجع hw_addr_مقصد_string_پر(addr, 0, میانگیر);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کن HW_ADDR_مقصد_STR_پر(hw_addr, flags) hw_addr_مقصد_string_پر((hw_addr), flags, (حرف[HW_ADDR_مقصد_STRING_MAX]){})
#تعریف_کن HW_ADDR_مقصد_STR(hw_addr) HW_ADDR_مقصد_STR_پر(hw_addr, 0)

#تعریف_کن HW_ADDR_NULL ((ثابت بنية hw_addr_داده){})

بنية hw_addr_داده *hw_addr_مجموعه(بنية hw_addr_داده *addr, ثابت uint8_t *بایتها, size_t length);

فراغ hw_addr_hash_func(ثابت بنية hw_addr_داده *p, بنية siphash *state);
صحيح hw_addr_compare(ثابت بنية hw_addr_داده *a, ثابت بنية hw_addr_داده *b);
ساكن مضمن bool hw_addr_equal(ثابت بنية hw_addr_داده *a, ثابت بنية hw_addr_داده *b) {
        أرجع hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ثابت بنية hw_addr_داده *addr) خالص;
bool hw_addr_is_valid(ثابت بنية hw_addr_داده *addr, uint16_t iftype);

خارجي ثابت بنية hash_ops hw_addr_hash_ops;
خارجي ثابت بنية hash_ops hw_addr_hash_ops_free;

#تعریف_کن ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#تعریف_کن ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#تعریف_کن ETHER_ADDR_مقصد_STRING_MAX (3*6)
حرف* ether_addr_مقصد_string(ثابت بنية ether_addr *addr, حرف میانگیر[ETHER_ADDR_مقصد_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#تعریف_کن ETHER_ADDR_مقصد_STR(addr) ether_addr_مقصد_string((addr), (حرف[ETHER_ADDR_مقصد_STRING_MAX]){})

صحيح ether_addr_compare(ثابت بنية ether_addr *a, ثابت بنية ether_addr *b);
ساكن مضمن bool ether_addr_equal(ثابت بنية ether_addr *a, ثابت بنية ether_addr *b) {
        أرجع ether_addr_compare(a, b) == 0;
}

#تعریف_کن ETHER_ADDR_NULL ((ثابت بنية ether_addr){})

ساكن مضمن bool ether_addr_is_null(ثابت بنية ether_addr *addr) {
        أرجع ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_پخش(ثابت بنية ether_addr *addr) خالص;

ساكن مضمن bool ether_addr_is_multicast(ثابت بنية ether_addr *addr) {
        أكد(addr);
        أرجع FLAGS_مجموعه(addr->ether_addr_octet[0], 0x01);
}

ساكن مضمن bool ether_addr_is_unicast(ثابت بنية ether_addr *addr) {
        أرجع !ether_addr_is_multicast(addr);
}

ساكن مضمن bool ether_addr_is_local(ثابت بنية ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        أكد(addr);
        أرجع FLAGS_مجموعه(addr->ether_addr_octet[0], 0x02);
}

ساكن مضمن bool ether_addr_is_global(ثابت بنية ether_addr *addr) {
        أرجع !ether_addr_is_local(addr);
}

خارجي ثابت بنية hash_ops ether_addr_hash_ops;
خارجي ثابت بنية hash_ops ether_addr_hash_ops_free;

فراغ ether_addr_mark_random(بنية ether_addr *addr);

صحيح hw_addr_ensure_پخش(بنية hw_addr_داده *bcast_addr, uint16_t arp_type);
