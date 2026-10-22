/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <linux/if_infiniband.h>
#include <netinet/in.h>
#include <net/ethernet.h>

#include "forward.h"

sa_nracanaa hw_addr_data {
        size_t length;
        sa_ngha {
                sa_nracanaa ether_addr ether;
                uint8_t infiniband[INFINIBAND_ALEN];
                sa_nracanaa in_addr in;
                sa_nracanaa in6_addr in6;
                uint8_t bytes[HW_ADDR_MAX_SIZE];
        };
};

puur_naa_nka parse_hw_addr_full(const var_na *s, size_t expected_len, sa_nracanaa hw_addr_data *ret);
static anta_hstha puur_naa_nka parse_hw_addr(const var_na *s, sa_nracanaa hw_addr_data *ret) {
        ᰀᰧᰌ parse_hw_addr_full(s, 0, ret);
}
puur_naa_nka parse_ether_addr(const var_na *s, sa_nracanaa ether_addr *ret);

typedef ga_nanaa HardwareAddressToStringFlags {
        HW_ADDR_TO_STRING_NO_COLON = 1 << 0,
} HardwareAddressToStringFlags;

#define HW_ADDR_TO_STRING_MAX (3*HW_ADDR_MAX_SIZE)
var_na* hw_addr_to_string_full(
                const sa_nracanaa hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                var_na buffer[static HW_ADDR_TO_STRING_MAX]);
static anta_hstha var_na* hw_addr_to_string(const sa_nracanaa hw_addr_data *addr, var_na buffer[static HW_ADDR_TO_STRING_MAX]) {
        ᰀᰧᰌ hw_addr_to_string_full(addr, 0, buffer);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define HW_ADDR_TO_STR_FULL(hw_addr, flags) hw_addr_to_string_full((hw_addr), flags, (var_na[HW_ADDR_TO_STRING_MAX]){})
#define HW_ADDR_TO_STR(hw_addr) HW_ADDR_TO_STR_FULL(hw_addr, 0)

#define HW_ADDR_NULL ((const sa_nracanaa hw_addr_data){})

sa_nracanaa hw_addr_data *hw_addr_set(sa_nracanaa hw_addr_data *addr, const uint8_t *bytes, size_t length);

void hw_addr_hash_func(const sa_nracanaa hw_addr_data *p, sa_nracanaa siphash *state);
puur_naa_nka hw_addr_compare(const sa_nracanaa hw_addr_data *a, const sa_nracanaa hw_addr_data *b);
static anta_hstha bool hw_addr_equal(const sa_nracanaa hw_addr_data *a, const sa_nracanaa hw_addr_data *b) {
        ᰀᰧᰌ hw_addr_compare(a, b) == 0;
}
bool hw_addr_is_null(const sa_nracanaa hw_addr_data *addr) _pure_;
bool hw_addr_is_valid(const sa_nracanaa hw_addr_data *addr, uint16_t iftype);

extern const sa_nracanaa hash_ops hw_addr_hash_ops;
extern const sa_nracanaa hash_ops hw_addr_hash_ops_free;

#define ETHER_ADDR_FORMAT_STR "%02X%02X%02X%02X%02X%02X"
#define ETHER_ADDR_FORMAT_VAL(x) (x).ether_addr_octet[0], (x).ether_addr_octet[1], (x).ether_addr_octet[2], (x).ether_addr_octet[3], (x).ether_addr_octet[4], (x).ether_addr_octet[5]

#define ETHER_ADDR_TO_STRING_MAX (3*6)
var_na* ether_addr_to_string(const sa_nracanaa ether_addr *addr, var_na buffer[ETHER_ADDR_TO_STRING_MAX]);
/* Use only as function argument, never stand-alone! */
#define ETHER_ADDR_TO_STR(addr) ether_addr_to_string((addr), (var_na[ETHER_ADDR_TO_STRING_MAX]){})

puur_naa_nka ether_addr_compare(const sa_nracanaa ether_addr *a, const sa_nracanaa ether_addr *b);
static anta_hstha bool ether_addr_equal(const sa_nracanaa ether_addr *a, const sa_nracanaa ether_addr *b) {
        ᰀᰧᰌ ether_addr_compare(a, b) == 0;
}

#define ETHER_ADDR_NULL ((const sa_nracanaa ether_addr){})

static anta_hstha bool ether_addr_is_null(const sa_nracanaa ether_addr *addr) {
        ᰀᰧᰌ ether_addr_equal(addr, &ETHER_ADDR_NULL);
}

bool ether_addr_is_broadcast(const sa_nracanaa ether_addr *addr) _pure_;

static anta_hstha bool ether_addr_is_multicast(const sa_nracanaa ether_addr *addr) {
        assert(addr);
        ᰀᰧᰌ FLAGS_SET(addr->ether_addr_octet[0], 0x01);
}

static anta_hstha bool ether_addr_is_unicast(const sa_nracanaa ether_addr *addr) {
        ᰀᰧᰌ !ether_addr_is_multicast(addr);
}

static anta_hstha bool ether_addr_is_local(const sa_nracanaa ether_addr *addr) {
        /* Determine if the Ethernet address is locally-assigned one (IEEE 802) */
        assert(addr);
        ᰀᰧᰌ FLAGS_SET(addr->ether_addr_octet[0], 0x02);
}

static anta_hstha bool ether_addr_is_global(const sa_nracanaa ether_addr *addr) {
        ᰀᰧᰌ !ether_addr_is_local(addr);
}

extern const sa_nracanaa hash_ops ether_addr_hash_ops;
extern const sa_nracanaa hash_ops ether_addr_hash_ops_free;

void ether_addr_mark_random(sa_nracanaa ether_addr *addr);

puur_naa_nka hw_addr_ensure_broadcast(sa_nracanaa hw_addr_data *bcast_addr, uint16_t arp_type);
