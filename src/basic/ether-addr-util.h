/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <linux/if_infiniband.h>
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#ଅନ୍ତର୍ଭାଵ <net/ethernet.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସଂରଚନା hw_addr_data {
        size_t length;
        ସଂଘ {
                ସଂରଚନା ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                ସଂରଚନା in_addr in;
                ସଂରଚନା in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_hw_addr_full(ଅଚଳ ଅକ୍ଷର *s, size_t expected_len, ସଂରଚନା hw_addr_data *ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_hw_addr(ଅଚଳ ଅକ୍ଷର *s, ସଂରଚନା hw_addr_data *ret) {
        ଫେରନ୍ତୁ parse_hw_addr_full(s, 0, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ether_addr(ଅଚଳ ଅକ୍ଷର *s, ସଂରଚନା ether_addr *ret);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
ଅକ୍ଷର* hw_addr_to_string_full(
                ଅଚଳ ସଂରଚନା hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                ଅକ୍ଷର buffer[ସ୍ଥିର HW_ADDR_TO_STRING_MAX]);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* hw_addr_to_string(ଅଚଳ ସଂରଚନା hw_addr_data *addr, ଅକ୍ଷର buffer[ସ୍ଥିର HW_ADDR_TO_STRING_MAX]) {
        ଫେରନ୍ତୁ hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (ଅକ୍ଷର[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((ଅଚଳ ସଂରଚନା hw_addr_data){})

ସଂରଚନା hw_addr_data *hw_addr_set(ସଂରଚନା hw_addr_data *addr, ଅଚଳ uint8_t *bytes, size_t length);

ଶୂନ୍ୟ hw_addr_hash_func(ଅଚଳ ସଂରଚନା hw_addr_data *p, ସଂରଚନା siphash *state);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା hw_addr_compare(ଅଚଳ ସଂରଚନା hw_addr_data *a, ଅଚଳ ସଂରଚନା hw_addr_data *b);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool hw_addr_equal(ଅଚଳ ସଂରଚନା hw_addr_data *a, ଅଚଳ ସଂରଚନା hw_addr_data *b) {
        ଫେରନ୍ତୁ hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(ଅଚଳ ସଂରଚନା hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(ଅଚଳ ସଂରଚନା hw_addr_data *addr, uint16_t iftype);

ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops hw_addr_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
ଅକ୍ଷର* ether_addr_to_string(ଅଚଳ ସଂରଚନା ether_addr *addr, ଅକ୍ଷର buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (ଅକ୍ଷର[ETHER_ADDR_TO_STRING_MAX]){})

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ether_addr_compare(ଅଚଳ ସଂରଚନା ether_addr *a, ଅଚଳ ସଂରଚନା ether_addr *b);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ether_addr_equal(ଅଚଳ ସଂରଚନା ether_addr *a, ଅଚଳ ସଂରଚନା ether_addr *b) {
        ଫେରନ୍ତୁ ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((ଅଚଳ ସଂରଚନା ether_addr){})

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ether_addr_is_null(ଅଚଳ ସଂରଚନା ether_addr *addr) {
        ଫେରନ୍ତୁ ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(ଅଚଳ ସଂରଚନା ether_addr *addr) _pure_;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ether_addr_is_multicast(ଅଚଳ ସଂରଚନା ether_addr *addr) {
        ପ୍ରତିଜ୍ଞା(addr);
        ଫେରନ୍ତୁ FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ether_addr_is_unicast(ଅଚଳ ସଂରଚନା ether_addr *addr) {
        ଫେରନ୍ତୁ !ether_addr_is_multicast(addr);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ether_addr_is_local(ଅଚଳ ସଂରଚନା ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        ପ୍ରତିଜ୍ଞା(addr);
        ଫେରନ୍ତୁ FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ether_addr_is_global(ଅଚଳ ସଂରଚନା ether_addr *addr) {
        ଫେରନ୍ତୁ !ether_addr_is_local(addr);
}

ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops ether_addr_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops ether_addr_hash_ops_free;

ଶୂନ୍ୟ ether_addr_mark_random(ସଂରଚନା ether_addr *addr);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hw_addr_ensure_broadcast(ସଂରଚନା hw_addr_data *bcast_addr, uint16_t arp_type);
