/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <linux/if_infiniband.h>
#അന്തര്ഭാവ <netinet/in.h>
#അന്തര്ഭാവ <net/ethernet.h>

#അന്തര്ഭാവ "forward.h"

ഘടന hw_addr_data {
        size_t length;
        സംഘം {
                ഘടന ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                ഘടന in_addr in;
                ഘടന in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

പൂർണ്ണസംഖ്യ parse_hw_addr_full(സ്ഥിരം അക്ഷരം *s, size_t expected_len, ഘടന hw_addr_data *ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ parse_hw_addr(സ്ഥിരം അക്ഷരം *s, ഘടന hw_addr_data *ret) {
        തിരികെ parse_hw_addr_full(s, 0, ret);
}
പൂർണ്ണസംഖ്യ parse_ether_addr(സ്ഥിരം അക്ഷരം *s, ഘടന ether_addr *ret);

തരനിർവചനം എണ്ണൽ HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
അക്ഷരം* hw_addr_to_string_full(
                സ്ഥിരം ഘടന hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                അക്ഷരം buffer[സ്ഥിതം HW_ADDR_TO_STRING_MAX]);
സ്ഥിതം വരിയിൽ അക്ഷരം* hw_addr_to_string(സ്ഥിരം ഘടന hw_addr_data *addr, അക്ഷരം buffer[സ്ഥിതം HW_ADDR_TO_STRING_MAX]) {
        തിരികെ hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (അക്ഷരം[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((സ്ഥിരം ഘടന hw_addr_data){})

ഘടന hw_addr_data *hw_addr_set(ഘടന hw_addr_data *addr, സ്ഥിരം uint8_t *bytes, size_t length);

ശൂന്യം hw_addr_hash_func(സ്ഥിരം ഘടന hw_addr_data *p, ഘടന siphash *state);
പൂർണ്ണസംഖ്യ hw_addr_compare(സ്ഥിരം ഘടന hw_addr_data *a, സ്ഥിരം ഘടന hw_addr_data *b);
സ്ഥിതം വരിയിൽ bool hw_addr_equal(സ്ഥിരം ഘടന hw_addr_data *a, സ്ഥിരം ഘടന hw_addr_data *b) {
        തിരികെ hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(സ്ഥിരം ഘടന hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(സ്ഥിരം ഘടന hw_addr_data *addr, uint16_t iftype);

ബാഹ്യ സ്ഥിരം ഘടന hash_ops hw_addr_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
അക്ഷരം* ether_addr_to_string(സ്ഥിരം ഘടന ether_addr *addr, അക്ഷരം buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (അക്ഷരം[ETHER_ADDR_TO_STRING_MAX]){})

പൂർണ്ണസംഖ്യ ether_addr_compare(സ്ഥിരം ഘടന ether_addr *a, സ്ഥിരം ഘടന ether_addr *b);
സ്ഥിതം വരിയിൽ bool ether_addr_equal(സ്ഥിരം ഘടന ether_addr *a, സ്ഥിരം ഘടന ether_addr *b) {
        തിരികെ ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((സ്ഥിരം ഘടന ether_addr){})

സ്ഥിതം വരിയിൽ bool ether_addr_is_null(സ്ഥിരം ഘടന ether_addr *addr) {
        തിരികെ ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(സ്ഥിരം ഘടന ether_addr *addr) _pure_;

സ്ഥിതം വരിയിൽ bool ether_addr_is_multicast(സ്ഥിരം ഘടന ether_addr *addr) {
        പ്രതിജ്ഞാ(addr);
        തിരികെ FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

സ്ഥിതം വരിയിൽ bool ether_addr_is_unicast(സ്ഥിരം ഘടന ether_addr *addr) {
        തിരികെ !ether_addr_is_multicast(addr);
}

സ്ഥിതം വരിയിൽ bool ether_addr_is_local(സ്ഥിരം ഘടന ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        പ്രതിജ്ഞാ(addr);
        തിരികെ FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

സ്ഥിതം വരിയിൽ bool ether_addr_is_global(സ്ഥിരം ഘടന ether_addr *addr) {
        തിരികെ !ether_addr_is_local(addr);
}

ബാഹ്യ സ്ഥിരം ഘടന hash_ops ether_addr_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops ether_addr_hash_ops_free;

ശൂന്യം ether_addr_mark_random(ഘടന ether_addr *addr);

പൂർണ്ണസംഖ്യ hw_addr_ensure_broadcast(ഘടന hw_addr_data *bcast_addr, uint16_t arp_type);
