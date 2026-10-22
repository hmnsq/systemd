/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <linux/if_infiniband.h>
#شامل <netinet/in.h>
#شامل <net/ethernet.h>

#شامل "forward.h"

ساخت hw_addr_ڈیٹا {
        size_t length;
        اتحاد {
                ساخت ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                ساخت in_addr اندر;
                ساخت in6_addr in6;
                uint8_t بائٹیں[HW_ADDR_MAX_سائز];
        };
};

صحیح parse_hw_addr_بھرا(ثابت حرف *s, size_t expected_len, ساخت hw_addr_ڈیٹا *ret);
ساکن اندرونی صحیح parse_hw_addr(ثابت حرف *s, ساخت hw_addr_ڈیٹا *ret) {
        واپس parse_hw_addr_بھرا(s, 0, ret);
}
صحیح parse_ether_addr(ثابت حرف *s, ساخت ether_addr *ret);

قسمتعریف شمار Hardware_پتہ_مقصد_String_Flags {
        HW_ADDR_مقصد_STRING_NO_COLON = 1 << 0,
} Hardware_پتہ_مقصد_String_Flags;

#تعریف_کرو HW_ADDR_مقصد_STRING_MAX (3*HW_ADDR_MAX_سائز)
حرف* hw_addr_مقصد_string_بھرا(
                ثابت ساخت hw_addr_ڈیٹا *addr,
                Hardware_پتہ_مقصد_String_Flags flags,
                حرف بفر[ساکن HW_ADDR_مقصد_STRING_MAX]);
ساکن اندرونی حرف* hw_addr_مقصد_string(ثابت ساخت hw_addr_ڈیٹا *addr, حرف بفر[ساکن HW_ADDR_مقصد_STRING_MAX]) {
        واپس hw_addr_مقصد_string_بھرا(addr, 0, بفر);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو HW_ADDR_مقصد_STR_بھرا(hw_addr, flags) hw_addr_مقصد_string_بھرا((hw_addr), flags, (حرف[HW_ADDR_مقصد_STRING_MAX]){})
#تعریف_کرو HW_ADDR_مقصد_STR(hw_addr) HW_ADDR_مقصد_STR_بھرا(hw_addr, 0)

#تعریف_کرو HW_ADDR_NULL ((ثابت ساخت hw_addr_ڈیٹا){})

ساخت hw_addr_ڈیٹا *hw_addr_سیٹ(ساخت hw_addr_ڈیٹا *addr, ثابت uint8_t *بائٹیں, size_t length);

خالی hw_addr_hash_func(ثابت ساخت hw_addr_ڈیٹا *p, ساخت siphash *state);
صحیح hw_addr_compare(ثابت ساخت hw_addr_ڈیٹا *a, ثابت ساخت hw_addr_ڈیٹا *b);
ساکن اندرونی bool hw_addr_equal(ثابت ساخت hw_addr_ڈیٹا *a, ثابت ساخت hw_addr_ڈیٹا *b) {
        واپس hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ثابت ساخت hw_addr_ڈیٹا *addr) خالص;
bool hw_addr_is_valid(ثابت ساخت hw_addr_ڈیٹا *addr, uint16_t iftype);

خارجی ثابت ساخت hash_ops hw_addr_hash_ops;
خارجی ثابت ساخت hash_ops hw_addr_hash_ops_free;

#تعریف_کرو ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#تعریف_کرو ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#تعریف_کرو ETHER_ADDR_مقصد_STRING_MAX (3*6)
حرف* ether_addr_مقصد_string(ثابت ساخت ether_addr *addr, حرف بفر[ETHER_ADDR_مقصد_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#تعریف_کرو ETHER_ADDR_مقصد_STR(addr) ether_addr_مقصد_string((addr), (حرف[ETHER_ADDR_مقصد_STRING_MAX]){})

صحیح ether_addr_compare(ثابت ساخت ether_addr *a, ثابت ساخت ether_addr *b);
ساکن اندرونی bool ether_addr_equal(ثابت ساخت ether_addr *a, ثابت ساخت ether_addr *b) {
        واپس ether_addr_compare(a, b) == 0;
}

#تعریف_کرو ETHER_ADDR_NULL ((ثابت ساخت ether_addr){})

ساکن اندرونی bool ether_addr_is_null(ثابت ساخت ether_addr *addr) {
        واپس ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_نشر(ثابت ساخت ether_addr *addr) خالص;

ساکن اندرونی bool ether_addr_is_multicast(ثابت ساخت ether_addr *addr) {
        أكد(addr);
        واپس FLAGS_سیٹ(addr->ether_addr_octet[0], 0x01);
}

ساکن اندرونی bool ether_addr_is_unicast(ثابت ساخت ether_addr *addr) {
        واپس !ether_addr_is_multicast(addr);
}

ساکن اندرونی bool ether_addr_is_local(ثابت ساخت ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        أكد(addr);
        واپس FLAGS_سیٹ(addr->ether_addr_octet[0], 0x02);
}

ساکن اندرونی bool ether_addr_is_global(ثابت ساخت ether_addr *addr) {
        واپس !ether_addr_is_local(addr);
}

خارجی ثابت ساخت hash_ops ether_addr_hash_ops;
خارجی ثابت ساخت hash_ops ether_addr_hash_ops_free;

خالی ether_addr_mark_random(ساخت ether_addr *addr);

صحیح hw_addr_ensure_نشر(ساخت hw_addr_ڈیٹا *bcast_addr, uint16_t arp_type);
