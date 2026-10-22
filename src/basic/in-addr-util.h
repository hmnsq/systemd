/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <netinet/in.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸಙ್ಘ in_addr_union {
        ಸಂರಚನಾ in_addr in;
        ಸಂರಚನಾ in6_addr in6;
        uint8_t bytes[CONST_MAX(ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr), ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr))];
};

ಸಂರಚನಾ in_addr_data {
        ಪೂರ್ಣಾಙ್ಕ family;
        ಸಙ್ಘ in_addr_union address;
};

bool in4_addr_is_null(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool in4_addr_is_set(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತ್ಯಾಗಮ !in4_addr_is_null(a);
}
bool in6_addr_is_null(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool in6_addr_is_set(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) {
        ಪ್ರತ್ಯಾಗಮ !in6_addr_is_null(a);
}
ಪೂರ್ಣಾಙ್ಕ in_addr_is_null(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool in_addr_is_set(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) {
        ಪ್ರತ್ಯಾಗಮ in_addr_is_null(family, u) == 0;
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in_addr_data_is_null(ಧ್ರುವ ಸಂರಚನಾ in_addr_data *a) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತ್ಯಾಗಮ in_addr_is_null(a->family, &a->address);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool in_addr_data_is_set(ಧ್ರುವ ಸಂರಚನಾ in_addr_data *a) {
        ಪ್ರತ್ಯಾಗಮ in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) _pure_;
bool in6_addr_is_multicast(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) _pure_;
ಪೂರ್ಣಾಙ್ಕ in_addr_is_multicast(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) _pure_;
bool in6_addr_is_link_local(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) _pure_;
ಪೂರ್ಣಾಙ್ಕ in_addr_is_link_local(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) _pure_;

bool in4_addr_is_localhost(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) _pure_;
ಪೂರ್ಣಾಙ್ಕ in_addr_is_localhost(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) _pure_;
ಪೂರ್ಣಾಙ್ಕ in_addr_is_localhost_one(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) _pure_;
bool in4_addr_is_non_local(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) _pure_;

bool in4_addr_equal(ಧ್ರುವ ಸಂರಚನಾ in_addr *a, ಧ್ರುವ ಸಂರಚನಾ in_addr *b) _pure_;
bool in6_addr_equal(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a, ಧ್ರುವ ಸಂರಚನಾ in6_addr *b) _pure_;
ಪೂರ್ಣಾಙ್ಕ in_addr_equal(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *a, ಧ್ರುವ ಸಙ್ಘ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                ಧ್ರುವ ಸಂರಚನಾ in_addr *a,
                unsigned aprefixlen,
                ಧ್ರುವ ಸಂರಚನಾ in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                ಧ್ರುವ ಸಂರಚನಾ in6_addr *a,
                unsigned aprefixlen,
                ಧ್ರುವ ಸಂರಚನಾ in6_addr *b,
                unsigned bprefixlen) _pure_;
ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_intersect(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *a,
                unsigned aprefixlen,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *b,
                unsigned bprefixlen) _pure_;
ಪೂರ್ಣಾಙ್ಕ in_addr_random_prefix(ಪೂರ್ಣಾಙ್ಕ family, ಸಙ್ಘ in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_range(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *in,
                unsigned prefixlen,
                ಸಙ್ಘ in_addr_union *ret_start,
                ಸಙ್ಘ in_addr_union *ret_end);

ಪೂರ್ಣಾಙ್ಕ in_addr_to_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in6_addr_to_string(ಧ್ರುವ ಸಂರಚನಾ in6_addr *u, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ in_addr_to_string(AF_INET6, (ಧ್ರುವ ಸಙ್ಘ in_addr_union*) u, ret);
}

ಧ್ರುವ ವರ್ಣ* typesafe_inet_ntop(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *a, ವರ್ಣ *buf, size_t len);
ಧ್ರುವ ವರ್ಣ* typesafe_inet_ntop4(ಧ್ರುವ ಸಂರಚನಾ in_addr *a, ವರ್ಣ *buf, size_t len);
ಧ್ರುವ ವರ್ಣ* typesafe_inet_ntop6(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a, ವರ್ಣ *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (ವರ್ಣ[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (ವರ್ಣ[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (ವರ್ಣ[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_to_string(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *u,
                unsigned prefixlen,
                ವರ್ಣ *buf,
                size_t buf_len);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* _in_addr_prefix_to_string(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *u,
                unsigned prefixlen,
                ವರ್ಣ *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        ಪ್ರತ್ಯಾಗಮ buf;
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* _in4_addr_prefix_to_string(ಧ್ರುವ ಸಂರಚನಾ in_addr *a, unsigned prefixlen, ವರ್ಣ *buf, size_t buf_len) {
        ಪ್ರತ್ಯಾಗಮ _in_addr_prefix_to_string(AF_INET, (ಧ್ರುವ ಸಙ್ಘ in_addr_union *) a, prefixlen, buf, buf_len);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* _in6_addr_prefix_to_string(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a, unsigned prefixlen, ವರ್ಣ *buf, size_t buf_len) {
        ಪ್ರತ್ಯಾಗಮ _in_addr_prefix_to_string(AF_INET6, (ಧ್ರುವ ಸಙ್ಘ in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (ವರ್ಣ[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (ವರ್ಣ[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (ವರ್ಣ[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

ಪೂರ್ಣಾಙ್ಕ in_addr_port_ifindex_name_to_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, uint16_t port, ಪೂರ್ಣಾಙ್ಕ ifindex, ಧ್ರುವ ವರ್ಣ *server_name, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in_addr_ifindex_to_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, ಪೂರ್ಣಾಙ್ಕ ifindex, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, ರಿಕ್ತ, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in_addr_port_to_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, uint16_t port, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ in_addr_port_ifindex_name_to_string(family, u, port, 0, ರಿಕ್ತ, ret);
}
ಪೂರ್ಣಾಙ್ಕ in_addr_from_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ವರ್ಣ *s, ಸಙ್ಘ in_addr_union *ret);
ಪೂರ್ಣಾಙ್ಕ in_addr_from_string_auto(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret_family, ಸಙ್ಘ in_addr_union *ret);

unsigned ವರ್ಣ in4_addr_netmask_to_prefixlen(ಧ್ರುವ ಸಂರಚನಾ in_addr *addr);
ಸಂರಚನಾ in_addr* in4_addr_prefixlen_to_netmask(ಸಂರಚನಾ in_addr *addr, unsigned ವರ್ಣ prefixlen);
ಸಂರಚನಾ in6_addr* in6_addr_prefixlen_to_netmask(ಸಂರಚನಾ in6_addr *addr, unsigned ವರ್ಣ prefixlen);
ಪೂರ್ಣಾಙ್ಕ in_addr_prefixlen_to_netmask(ಪೂರ್ಣಾಙ್ಕ family, ಸಙ್ಘ in_addr_union *addr, unsigned ವರ್ಣ prefixlen);
ಪೂರ್ಣಾಙ್ಕ in4_addr_default_prefixlen(ಧ್ರುವ ಸಂರಚನಾ in_addr *addr, unsigned ವರ್ಣ *prefixlen);
ಪೂರ್ಣಾಙ್ಕ in4_addr_default_subnet_mask(ಧ್ರುವ ಸಂರಚನಾ in_addr *addr, ಸಂರಚನಾ in_addr *mask);
ಪೂರ್ಣಾಙ್ಕ in4_addr_mask(ಸಂರಚನಾ in_addr *addr, unsigned ವರ್ಣ prefixlen);
ಪೂರ್ಣಾಙ್ಕ in6_addr_mask(ಸಂರಚನಾ in6_addr *addr, unsigned ವರ್ಣ prefixlen);
ಪೂರ್ಣಾಙ್ಕ in_addr_mask(ಪೂರ್ಣಾಙ್ಕ family, ಸಙ್ಘ in_addr_union *addr, unsigned ವರ್ಣ prefixlen);
ಪೂರ್ಣಾಙ್ಕ in4_addr_prefix_covers_full(ಧ್ರುವ ಸಂರಚನಾ in_addr *prefix, unsigned ವರ್ಣ prefixlen, ಧ್ರುವ ಸಂರಚನಾ in_addr *address, unsigned ವರ್ಣ address_prefixlen);
ಪೂರ್ಣಾಙ್ಕ in6_addr_prefix_covers_full(ಧ್ರುವ ಸಂರಚನಾ in6_addr *prefix, unsigned ವರ್ಣ prefixlen, ಧ್ರುವ ಸಂರಚನಾ in6_addr *address, unsigned ವರ್ಣ address_prefixlen);
ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_covers_full(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *prefix, unsigned ವರ್ಣ prefixlen, ಧ್ರುವ ಸಙ್ಘ in_addr_union *address, unsigned ವರ್ಣ address_prefixlen);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in4_addr_prefix_covers(ಧ್ರುವ ಸಂರಚನಾ in_addr *prefix, unsigned ವರ್ಣ prefixlen, ಧ್ರುವ ಸಂರಚನಾ in_addr *address) {
        ಪ್ರತ್ಯಾಗಮ in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in6_addr_prefix_covers(ಧ್ರುವ ಸಂರಚನಾ in6_addr *prefix, unsigned ವರ್ಣ prefixlen, ಧ್ರುವ ಸಂರಚನಾ in6_addr *address) {
        ಪ್ರತ್ಯಾಗಮ in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_covers(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *prefix, unsigned ವರ್ಣ prefixlen, ಧ್ರುವ ಸಙ್ಘ in_addr_union *address) {
        ಪ್ರತ್ಯಾಗಮ in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
ಪೂರ್ಣಾಙ್ಕ in_addr_parse_prefixlen(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ವರ್ಣ *p, unsigned ವರ್ಣ *ret);
ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_from_string(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ family, ಸಙ್ಘ in_addr_union *ret_prefix, unsigned ವರ್ಣ *ret_prefixlen);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_from_string_auto_full(ಧ್ರುವ ವರ್ಣ *p, InAddrPrefixLenMode mode, ಪೂರ್ಣಾಙ್ಕ *ret_family, ಸಙ್ಘ in_addr_union *ret_prefix, unsigned ವರ್ಣ *ret_prefixlen);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_from_string_auto(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ *ret_family, ಸಙ್ಘ in_addr_union *ret_prefix, unsigned ವರ್ಣ *ret_prefixlen) {
        ಪ್ರತ್ಯಾಗಮ in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ size_t FAMILY_ADDRESS_SIZE(ಪೂರ್ಣಾಙ್ಕ family) {
        ವಿಕಲ್ಪ (family) {
                ಪಕ್ಷ AF_INET:
                        ಪ್ರತ್ಯಾಗಮ 4;
                ಪಕ್ಷ AF_INET6:
                        ಪ್ರತ್ಯಾಗಮ 16;
                ಸ್ವಭಾವ:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                ಪೂರ್ಣಾಙ್ಕ _f = (f);                                           \
                _f == AF_INET ? ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr) :                \
                _f == AF_INET6 ? ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((ಸಙ್ಘ in_addr_union) { .in6 = {} })

ಶೂನ್ಯ in_addr_hash_func(ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, ಪೂರ್ಣಾಙ್ಕ family, ಸಂರಚನಾ siphash *state);
ಶೂನ್ಯ in_addr_data_hash_func(ಧ್ರುವ ಸಂರಚನಾ in_addr_data *a, ಸಂರಚನಾ siphash *state);
ಪೂರ್ಣಾಙ್ಕ in_addr_data_compare_func(ಧ್ರುವ ಸಂರಚನಾ in_addr_data *x, ಧ್ರುವ ಸಂರಚನಾ in_addr_data *y);
ಶೂನ್ಯ in6_addr_hash_func(ಧ್ರುವ ಸಂರಚನಾ in6_addr *addr, ಸಂರಚನಾ siphash *state);
ಪೂರ್ಣಾಙ್ಕ in6_addr_compare_func(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a, ಧ್ರುವ ಸಂರಚನಾ in6_addr *b);

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops in_addr_data_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops in_addr_data_hash_ops_free;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops in6_addr_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops in6_addr_hash_ops_free;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ PTR_TO_IN4_ADDR(ಧ್ರುವ ಶೂನ್ಯ *p, ಸಂರಚನಾ in_addr *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* IN4_ADDR_TO_PTR(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
