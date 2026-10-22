/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <linux/if_infiniband.h>
#شامل <netinet/in.h>
#شامل <net/ethernet.h>

#شامل "forward.h"

ساختار hw_addr_data {
        size_t length;
        اجتماع {
                ساختار ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                ساختار in_addr in;
                ساختار in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

صحیح parse_hw_addr_full(ثابت نویسه *s, size_t expected_len, ساختار hw_addr_data *ret);
ایستا درونخطی صحیح parse_hw_addr(ثابت نویسه *s, ساختار hw_addr_data *ret) {
        بازگشت parse_hw_addr_full(s, 0, ret);
}
صحیح parse_ether_addr(ثابت نویسه *s, ساختار ether_addr *ret);

تعریفنوع شمارش HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
نویسه* hw_addr_to_string_full(
                ثابت ساختار hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                نویسه buffer[ایستا HW_ADDR_TO_STRING_MAX]);
ایستا درونخطی نویسه* hw_addr_to_string(ثابت ساختار hw_addr_data *addr, نویسه buffer[ایستا HW_ADDR_TO_STRING_MAX]) {
        بازگشت hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (نویسه[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((ثابت ساختار hw_addr_data){})

ساختار hw_addr_data *hw_addr_set(ساختار hw_addr_data *addr, ثابت uint8_t *bytes, size_t length);

تهی hw_addr_hash_func(ثابت ساختار hw_addr_data *p, ساختار siphash *state);
صحیح hw_addr_compare(ثابت ساختار hw_addr_data *a, ثابت ساختار hw_addr_data *b);
ایستا درونخطی bool hw_addr_equal(ثابت ساختار hw_addr_data *a, ثابت ساختار hw_addr_data *b) {
        بازگشت hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ثابت ساختار hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(ثابت ساختار hw_addr_data *addr, uint16_t iftype);

بیرونی ثابت ساختار hash_ops hw_addr_hash_ops;
بیرونی ثابت ساختار hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
نویسه* ether_addr_to_string(ثابت ساختار ether_addr *addr, نویسه buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (نویسه[ETHER_ADDR_TO_STRING_MAX]){})

صحیح ether_addr_compare(ثابت ساختار ether_addr *a, ثابت ساختار ether_addr *b);
ایستا درونخطی bool ether_addr_equal(ثابت ساختار ether_addr *a, ثابت ساختار ether_addr *b) {
        بازگشت ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((ثابت ساختار ether_addr){})

ایستا درونخطی bool ether_addr_is_null(ثابت ساختار ether_addr *addr) {
        بازگشت ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(ثابت ساختار ether_addr *addr) _pure_;

ایستا درونخطی bool ether_addr_is_multicast(ثابت ساختار ether_addr *addr) {
        أكد(addr);
        بازگشت FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

ایستا درونخطی bool ether_addr_is_unicast(ثابت ساختار ether_addr *addr) {
        بازگشت !ether_addr_is_multicast(addr);
}

ایستا درونخطی bool ether_addr_is_local(ثابت ساختار ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        أكد(addr);
        بازگشت FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

ایستا درونخطی bool ether_addr_is_global(ثابت ساختار ether_addr *addr) {
        بازگشت !ether_addr_is_local(addr);
}

بیرونی ثابت ساختار hash_ops ether_addr_hash_ops;
بیرونی ثابت ساختار hash_ops ether_addr_hash_ops_free;

تهی ether_addr_mark_random(ساختار ether_addr *addr);

صحیح hw_addr_ensure_broadcast(ساختار hw_addr_data *bcast_addr, uint16_t arp_type);
