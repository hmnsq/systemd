/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <netinet/in.h>

#அந்தர்பாவ "forward.h"

ஒன்றிப்பு in_addr_union {
        கட்டமைப்பு in_addr in;
        கட்டமைப்பு in6_addr in6;
        uint8_t bytes[CONST_MAX(அளவு(கட்டமைப்பு in_addr), அளவு(கட்டமைப்பு in6_addr))];
};

கட்டமைப்பு in_addr_data {
        முழு family;
        ஒன்றிப்பு in_addr_union address;
};

bool in4_addr_is_null(மாறிலி கட்டமைப்பு in_addr *a) _pure_;
நிலையான உள்வரி bool in4_addr_is_set(மாறிலி கட்டமைப்பு in_addr *a) {
        திரும்பு !in4_addr_is_null(a);
}
bool in6_addr_is_null(மாறிலி கட்டமைப்பு in6_addr *a) _pure_;
நிலையான உள்வரி bool in6_addr_is_set(மாறிலி கட்டமைப்பு in6_addr *a) {
        திரும்பு !in6_addr_is_null(a);
}
முழு in_addr_is_null(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) _pure_;
நிலையான உள்வரி bool in_addr_is_set(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) {
        திரும்பு in_addr_is_null(family, u) == 0;
}
நிலையான உள்வரி முழு in_addr_data_is_null(மாறிலி கட்டமைப்பு in_addr_data *a) {
        ப்ரதிஜ்ஞா(a);
        திரும்பு in_addr_is_null(a->family, &a->address);
}
நிலையான உள்வரி bool in_addr_data_is_set(மாறிலி கட்டமைப்பு in_addr_data *a) {
        திரும்பு in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(மாறிலி கட்டமைப்பு in_addr *a) _pure_;
bool in6_addr_is_multicast(மாறிலி கட்டமைப்பு in6_addr *a) _pure_;
முழு in_addr_is_multicast(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) _pure_;

bool in4_addr_is_link_local(மாறிலி கட்டமைப்பு in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(மாறிலி கட்டமைப்பு in_addr *a) _pure_;
bool in6_addr_is_link_local(மாறிலி கட்டமைப்பு in6_addr *a) _pure_;
முழு in_addr_is_link_local(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(மாறிலி கட்டமைப்பு in6_addr *a) _pure_;

bool in4_addr_is_localhost(மாறிலி கட்டமைப்பு in_addr *a) _pure_;
முழு in_addr_is_localhost(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) _pure_;
முழு in_addr_is_localhost_one(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(மாறிலி கட்டமைப்பு in_addr *a) _pure_;
bool in4_addr_is_non_local(மாறிலி கட்டமைப்பு in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(மாறிலி கட்டமைப்பு in6_addr *a) _pure_;

bool in4_addr_equal(மாறிலி கட்டமைப்பு in_addr *a, மாறிலி கட்டமைப்பு in_addr *b) _pure_;
bool in6_addr_equal(மாறிலி கட்டமைப்பு in6_addr *a, மாறிலி கட்டமைப்பு in6_addr *b) _pure_;
முழு in_addr_equal(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *a, மாறிலி ஒன்றிப்பு in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                மாறிலி கட்டமைப்பு in_addr *a,
                unsigned aprefixlen,
                மாறிலி கட்டமைப்பு in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                மாறிலி கட்டமைப்பு in6_addr *a,
                unsigned aprefixlen,
                மாறிலி கட்டமைப்பு in6_addr *b,
                unsigned bprefixlen) _pure_;
முழு in_addr_prefix_intersect(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *a,
                unsigned aprefixlen,
                மாறிலி ஒன்றிப்பு in_addr_union *b,
                unsigned bprefixlen) _pure_;
முழு in_addr_random_prefix(முழு family, ஒன்றிப்பு in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
முழு in_addr_prefix_range(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *in,
                unsigned prefixlen,
                ஒன்றிப்பு in_addr_union *ret_start,
                ஒன்றிப்பு in_addr_union *ret_end);

முழு in_addr_to_string(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u, எழுத்து **ret);
நிலையான உள்வரி முழு in6_addr_to_string(மாறிலி கட்டமைப்பு in6_addr *u, எழுத்து **ret) {
        திரும்பு in_addr_to_string(AF_INET6, (மாறிலி ஒன்றிப்பு in_addr_union*) u, ret);
}

மாறிலி எழுத்து* typesafe_inet_ntop(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *a, எழுத்து *buf, size_t len);
மாறிலி எழுத்து* typesafe_inet_ntop4(மாறிலி கட்டமைப்பு in_addr *a, எழுத்து *buf, size_t len);
மாறிலி எழுத்து* typesafe_inet_ntop6(மாறிலி கட்டமைப்பு in6_addr *a, எழுத்து *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (எழுத்து[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (எழுத்து[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (எழுத்து[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

முழு in_addr_prefix_to_string(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *u,
                unsigned prefixlen,
                எழுத்து *buf,
                size_t buf_len);

நிலையான உள்வரி மாறிலி எழுத்து* _in_addr_prefix_to_string(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *u,
                unsigned prefixlen,
                எழுத்து *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        திரும்பு buf;
}
நிலையான உள்வரி மாறிலி எழுத்து* _in4_addr_prefix_to_string(மாறிலி கட்டமைப்பு in_addr *a, unsigned prefixlen, எழுத்து *buf, size_t buf_len) {
        திரும்பு _in_addr_prefix_to_string(AF_INET, (மாறிலி ஒன்றிப்பு in_addr_union *) a, prefixlen, buf, buf_len);
}
நிலையான உள்வரி மாறிலி எழுத்து* _in6_addr_prefix_to_string(மாறிலி கட்டமைப்பு in6_addr *a, unsigned prefixlen, எழுத்து *buf, size_t buf_len) {
        திரும்பு _in_addr_prefix_to_string(AF_INET6, (மாறிலி ஒன்றிப்பு in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (எழுத்து[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (எழுத்து[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (எழுத்து[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

முழு in_addr_port_ifindex_name_to_string(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u, uint16_t port, முழு ifindex, மாறிலி எழுத்து *server_name, எழுத்து **ret);
நிலையான உள்வரி முழு in_addr_ifindex_to_string(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u, முழு ifindex, எழுத்து **ret) {
        திரும்பு in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, வெற்று, ret);
}
நிலையான உள்வரி முழு in_addr_port_to_string(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u, uint16_t port, எழுத்து **ret) {
        திரும்பு in_addr_port_ifindex_name_to_string(family, u, port, 0, வெற்று, ret);
}
முழு in_addr_from_string(முழு family, மாறிலி எழுத்து *s, ஒன்றிப்பு in_addr_union *ret);
முழு in_addr_from_string_auto(மாறிலி எழுத்து *s, முழு *ret_family, ஒன்றிப்பு in_addr_union *ret);

unsigned எழுத்து in4_addr_netmask_to_prefixlen(மாறிலி கட்டமைப்பு in_addr *addr);
கட்டமைப்பு in_addr* in4_addr_prefixlen_to_netmask(கட்டமைப்பு in_addr *addr, unsigned எழுத்து prefixlen);
கட்டமைப்பு in6_addr* in6_addr_prefixlen_to_netmask(கட்டமைப்பு in6_addr *addr, unsigned எழுத்து prefixlen);
முழு in_addr_prefixlen_to_netmask(முழு family, ஒன்றிப்பு in_addr_union *addr, unsigned எழுத்து prefixlen);
முழு in4_addr_default_prefixlen(மாறிலி கட்டமைப்பு in_addr *addr, unsigned எழுத்து *prefixlen);
முழு in4_addr_default_subnet_mask(மாறிலி கட்டமைப்பு in_addr *addr, கட்டமைப்பு in_addr *mask);
முழு in4_addr_mask(கட்டமைப்பு in_addr *addr, unsigned எழுத்து prefixlen);
முழு in6_addr_mask(கட்டமைப்பு in6_addr *addr, unsigned எழுத்து prefixlen);
முழு in_addr_mask(முழு family, ஒன்றிப்பு in_addr_union *addr, unsigned எழுத்து prefixlen);
முழு in4_addr_prefix_covers_full(மாறிலி கட்டமைப்பு in_addr *prefix, unsigned எழுத்து prefixlen, மாறிலி கட்டமைப்பு in_addr *address, unsigned எழுத்து address_prefixlen);
முழு in6_addr_prefix_covers_full(மாறிலி கட்டமைப்பு in6_addr *prefix, unsigned எழுத்து prefixlen, மாறிலி கட்டமைப்பு in6_addr *address, unsigned எழுத்து address_prefixlen);
முழு in_addr_prefix_covers_full(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *prefix, unsigned எழுத்து prefixlen, மாறிலி ஒன்றிப்பு in_addr_union *address, unsigned எழுத்து address_prefixlen);
நிலையான உள்வரி முழு in4_addr_prefix_covers(மாறிலி கட்டமைப்பு in_addr *prefix, unsigned எழுத்து prefixlen, மாறிலி கட்டமைப்பு in_addr *address) {
        திரும்பு in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
நிலையான உள்வரி முழு in6_addr_prefix_covers(மாறிலி கட்டமைப்பு in6_addr *prefix, unsigned எழுத்து prefixlen, மாறிலி கட்டமைப்பு in6_addr *address) {
        திரும்பு in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
நிலையான உள்வரி முழு in_addr_prefix_covers(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *prefix, unsigned எழுத்து prefixlen, மாறிலி ஒன்றிப்பு in_addr_union *address) {
        திரும்பு in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
முழு in_addr_parse_prefixlen(முழு family, மாறிலி எழுத்து *p, unsigned எழுத்து *ret);
முழு in_addr_prefix_from_string(மாறிலி எழுத்து *p, முழு family, ஒன்றிப்பு in_addr_union *ret_prefix, unsigned எழுத்து *ret_prefixlen);

வகைவரை எண்ணிக்கை InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

முழு in_addr_prefix_from_string_auto_full(மாறிலி எழுத்து *p, InAddrPrefixLenMode mode, முழு *ret_family, ஒன்றிப்பு in_addr_union *ret_prefix, unsigned எழுத்து *ret_prefixlen);
நிலையான உள்வரி முழு in_addr_prefix_from_string_auto(மாறிலி எழுத்து *p, முழு *ret_family, ஒன்றிப்பு in_addr_union *ret_prefix, unsigned எழுத்து *ret_prefixlen) {
        திரும்பு in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

நிலையான உள்வரி size_t FAMILY_ADDRESS_SIZE(முழு family) {
        தேர்வு (family) {
                நிலை AF_INET:
                        திரும்பு 4;
                நிலை AF_INET6:
                        திரும்பு 16;
                இயல்பு:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                முழு _f = (f);                                           \
                _f == AF_INET ? அளவு(கட்டமைப்பு in_addr) :                \
                _f == AF_INET6 ? அளவு(கட்டமைப்பு in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((ஒன்றிப்பு in_addr_union) { .in6 = {} })

வெறுமை in_addr_hash_func(மாறிலி ஒன்றிப்பு in_addr_union *u, முழு family, கட்டமைப்பு siphash *state);
வெறுமை in_addr_data_hash_func(மாறிலி கட்டமைப்பு in_addr_data *a, கட்டமைப்பு siphash *state);
முழு in_addr_data_compare_func(மாறிலி கட்டமைப்பு in_addr_data *x, மாறிலி கட்டமைப்பு in_addr_data *y);
வெறுமை in6_addr_hash_func(மாறிலி கட்டமைப்பு in6_addr *addr, கட்டமைப்பு siphash *state);
முழு in6_addr_compare_func(மாறிலி கட்டமைப்பு in6_addr *a, மாறிலி கட்டமைப்பு in6_addr *b);

வெளி மாறிலி கட்டமைப்பு hash_ops in_addr_data_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops in_addr_data_hash_ops_free;
வெளி மாறிலி கட்டமைப்பு hash_ops in6_addr_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops in6_addr_hash_ops_free;

நிலையான உள்வரி வெறுமை PTR_TO_IN4_ADDR(மாறிலி வெறுமை *p, கட்டமைப்பு in_addr *ret) {
        ப்ரதிஜ்ஞா(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

நிலையான உள்வரி வெறுமை* IN4_ADDR_TO_PTR(மாறிலி கட்டமைப்பு in_addr *a) {
        ப்ரதிஜ்ஞா(a);
        திரும்பு (வெறுமை*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
