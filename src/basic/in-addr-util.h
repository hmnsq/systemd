/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <netinet/in.h>

#અન્તર્ભાવ "forward.h"

સંઘ in_addr_union {
        સંરચના in_addr in;
        સંરચના in6_addr in6;
        uint8_t bytes[CONST_MAX(કદ(સંરચના in_addr), કદ(સંરચના in6_addr))];
};

સંરચના in_addr_data {
        પૂર્ણાંક family;
        સંઘ in_addr_union address;
};

bool in4_addr_is_null(અચલ સંરચના in_addr *a) _pure_;
સ્થિર અંતઃરેખ bool in4_addr_is_set(અચલ સંરચના in_addr *a) {
        પાછા !in4_addr_is_null(a);
}
bool in6_addr_is_null(અચલ સંરચના in6_addr *a) _pure_;
સ્થિર અંતઃરેખ bool in6_addr_is_set(અચલ સંરચના in6_addr *a) {
        પાછા !in6_addr_is_null(a);
}
પૂર્ણાંક in_addr_is_null(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) _pure_;
સ્થિર અંતઃરેખ bool in_addr_is_set(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) {
        પાછા in_addr_is_null(family, u) == 0;
}
સ્થિર અંતઃરેખ પૂર્ણાંક in_addr_data_is_null(અચલ સંરચના in_addr_data *a) {
        પ્રતિજ્ઞા(a);
        પાછા in_addr_is_null(a->family, &a->address);
}
સ્થિર અંતઃરેખ bool in_addr_data_is_set(અચલ સંરચના in_addr_data *a) {
        પાછા in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(અચલ સંરચના in_addr *a) _pure_;
bool in6_addr_is_multicast(અચલ સંરચના in6_addr *a) _pure_;
પૂર્ણાંક in_addr_is_multicast(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(અચલ સંરચના in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(અચલ સંરચના in_addr *a) _pure_;
bool in6_addr_is_link_local(અચલ સંરચના in6_addr *a) _pure_;
પૂર્ણાંક in_addr_is_link_local(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(અચલ સંરચના in6_addr *a) _pure_;

bool in4_addr_is_localhost(અચલ સંરચના in_addr *a) _pure_;
પૂર્ણાંક in_addr_is_localhost(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) _pure_;
પૂર્ણાંક in_addr_is_localhost_one(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(અચલ સંરચના in_addr *a) _pure_;
bool in4_addr_is_non_local(અચલ સંરચના in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(અચલ સંરચના in6_addr *a) _pure_;

bool in4_addr_equal(અચલ સંરચના in_addr *a, અચલ સંરચના in_addr *b) _pure_;
bool in6_addr_equal(અચલ સંરચના in6_addr *a, અચલ સંરચના in6_addr *b) _pure_;
પૂર્ણાંક in_addr_equal(પૂર્ણાંક family, અચલ સંઘ in_addr_union *a, અચલ સંઘ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                અચલ સંરચના in_addr *a,
                unsigned aprefixlen,
                અચલ સંરચના in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                અચલ સંરચના in6_addr *a,
                unsigned aprefixlen,
                અચલ સંરચના in6_addr *b,
                unsigned bprefixlen) _pure_;
પૂર્ણાંક in_addr_prefix_intersect(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *a,
                unsigned aprefixlen,
                અચલ સંઘ in_addr_union *b,
                unsigned bprefixlen) _pure_;
પૂર્ણાંક in_addr_random_prefix(પૂર્ણાંક family, સંઘ in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
પૂર્ણાંક in_addr_prefix_range(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *in,
                unsigned prefixlen,
                સંઘ in_addr_union *ret_start,
                સંઘ in_addr_union *ret_end);

પૂર્ણાંક in_addr_to_string(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u, અક્ષર **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક in6_addr_to_string(અચલ સંરચના in6_addr *u, અક્ષર **ret) {
        પાછા in_addr_to_string(AF_INET6, (અચલ સંઘ in_addr_union*) u, ret);
}

અચલ અક્ષર* typesafe_inet_ntop(પૂર્ણાંક family, અચલ સંઘ in_addr_union *a, અક્ષર *buf, size_t len);
અચલ અક્ષર* typesafe_inet_ntop4(અચલ સંરચના in_addr *a, અક્ષર *buf, size_t len);
અચલ અક્ષર* typesafe_inet_ntop6(અચલ સંરચના in6_addr *a, અક્ષર *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (અક્ષર[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (અક્ષર[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (અક્ષર[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

પૂર્ણાંક in_addr_prefix_to_string(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *u,
                unsigned prefixlen,
                અક્ષર *buf,
                size_t buf_len);

સ્થિર અંતઃરેખ અચલ અક્ષર* _in_addr_prefix_to_string(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *u,
                unsigned prefixlen,
                અક્ષર *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        પાછા buf;
}
સ્થિર અંતઃરેખ અચલ અક્ષર* _in4_addr_prefix_to_string(અચલ સંરચના in_addr *a, unsigned prefixlen, અક્ષર *buf, size_t buf_len) {
        પાછા _in_addr_prefix_to_string(AF_INET, (અચલ સંઘ in_addr_union *) a, prefixlen, buf, buf_len);
}
સ્થિર અંતઃરેખ અચલ અક્ષર* _in6_addr_prefix_to_string(અચલ સંરચના in6_addr *a, unsigned prefixlen, અક્ષર *buf, size_t buf_len) {
        પાછા _in_addr_prefix_to_string(AF_INET6, (અચલ સંઘ in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (અક્ષર[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (અક્ષર[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (અક્ષર[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

પૂર્ણાંક in_addr_port_ifindex_name_to_string(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u, uint16_t port, પૂર્ણાંક ifindex, અચલ અક્ષર *server_name, અક્ષર **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક in_addr_ifindex_to_string(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u, પૂર્ણાંક ifindex, અક્ષર **ret) {
        પાછા in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, ખાલી, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક in_addr_port_to_string(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u, uint16_t port, અક્ષર **ret) {
        પાછા in_addr_port_ifindex_name_to_string(family, u, port, 0, ખાલી, ret);
}
પૂર્ણાંક in_addr_from_string(પૂર્ણાંક family, અચલ અક્ષર *s, સંઘ in_addr_union *ret);
પૂર્ણાંક in_addr_from_string_auto(અચલ અક્ષર *s, પૂર્ણાંક *ret_family, સંઘ in_addr_union *ret);

unsigned અક્ષર in4_addr_netmask_to_prefixlen(અચલ સંરચના in_addr *addr);
સંરચના in_addr* in4_addr_prefixlen_to_netmask(સંરચના in_addr *addr, unsigned અક્ષર prefixlen);
સંરચના in6_addr* in6_addr_prefixlen_to_netmask(સંરચના in6_addr *addr, unsigned અક્ષર prefixlen);
પૂર્ણાંક in_addr_prefixlen_to_netmask(પૂર્ણાંક family, સંઘ in_addr_union *addr, unsigned અક્ષર prefixlen);
પૂર્ણાંક in4_addr_default_prefixlen(અચલ સંરચના in_addr *addr, unsigned અક્ષર *prefixlen);
પૂર્ણાંક in4_addr_default_subnet_mask(અચલ સંરચના in_addr *addr, સંરચના in_addr *mask);
પૂર્ણાંક in4_addr_mask(સંરચના in_addr *addr, unsigned અક્ષર prefixlen);
પૂર્ણાંક in6_addr_mask(સંરચના in6_addr *addr, unsigned અક્ષર prefixlen);
પૂર્ણાંક in_addr_mask(પૂર્ણાંક family, સંઘ in_addr_union *addr, unsigned અક્ષર prefixlen);
પૂર્ણાંક in4_addr_prefix_covers_full(અચલ સંરચના in_addr *prefix, unsigned અક્ષર prefixlen, અચલ સંરચના in_addr *address, unsigned અક્ષર address_prefixlen);
પૂર્ણાંક in6_addr_prefix_covers_full(અચલ સંરચના in6_addr *prefix, unsigned અક્ષર prefixlen, અચલ સંરચના in6_addr *address, unsigned અક્ષર address_prefixlen);
પૂર્ણાંક in_addr_prefix_covers_full(પૂર્ણાંક family, અચલ સંઘ in_addr_union *prefix, unsigned અક્ષર prefixlen, અચલ સંઘ in_addr_union *address, unsigned અક્ષર address_prefixlen);
સ્થિર અંતઃરેખ પૂર્ણાંક in4_addr_prefix_covers(અચલ સંરચના in_addr *prefix, unsigned અક્ષર prefixlen, અચલ સંરચના in_addr *address) {
        પાછા in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
સ્થિર અંતઃરેખ પૂર્ણાંક in6_addr_prefix_covers(અચલ સંરચના in6_addr *prefix, unsigned અક્ષર prefixlen, અચલ સંરચના in6_addr *address) {
        પાછા in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
સ્થિર અંતઃરેખ પૂર્ણાંક in_addr_prefix_covers(પૂર્ણાંક family, અચલ સંઘ in_addr_union *prefix, unsigned અક્ષર prefixlen, અચલ સંઘ in_addr_union *address) {
        પાછા in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
પૂર્ણાંક in_addr_parse_prefixlen(પૂર્ણાંક family, અચલ અક્ષર *p, unsigned અક્ષર *ret);
પૂર્ણાંક in_addr_prefix_from_string(અચલ અક્ષર *p, પૂર્ણાંક family, સંઘ in_addr_union *ret_prefix, unsigned અક્ષર *ret_prefixlen);

પ્રકારવ્યાખ્યા ગણના InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

પૂર્ણાંક in_addr_prefix_from_string_auto_full(અચલ અક્ષર *p, InAddrPrefixLenMode mode, પૂર્ણાંક *ret_family, સંઘ in_addr_union *ret_prefix, unsigned અક્ષર *ret_prefixlen);
સ્થિર અંતઃરેખ પૂર્ણાંક in_addr_prefix_from_string_auto(અચલ અક્ષર *p, પૂર્ણાંક *ret_family, સંઘ in_addr_union *ret_prefix, unsigned અક્ષર *ret_prefixlen) {
        પાછા in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

સ્થિર અંતઃરેખ size_t FAMILY_ADDRESS_SIZE(પૂર્ણાંક family) {
        પસંદગી (family) {
                સ્થિતિ AF_INET:
                        પાછા 4;
                સ્થિતિ AF_INET6:
                        પાછા 16;
                મૂળભૂત:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                પૂર્ણાંક _f = (f);                                           \
                _f == AF_INET ? કદ(સંરચના in_addr) :                \
                _f == AF_INET6 ? કદ(સંરચના in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((સંઘ in_addr_union) { .in6 = {} })

શૂન્ય in_addr_hash_func(અચલ સંઘ in_addr_union *u, પૂર્ણાંક family, સંરચના siphash *state);
શૂન્ય in_addr_data_hash_func(અચલ સંરચના in_addr_data *a, સંરચના siphash *state);
પૂર્ણાંક in_addr_data_compare_func(અચલ સંરચના in_addr_data *x, અચલ સંરચના in_addr_data *y);
શૂન્ય in6_addr_hash_func(અચલ સંરચના in6_addr *addr, સંરચના siphash *state);
પૂર્ણાંક in6_addr_compare_func(અચલ સંરચના in6_addr *a, અચલ સંરચના in6_addr *b);

બાહ્ય અચલ સંરચના hash_ops in_addr_data_hash_ops;
બાહ્ય અચલ સંરચના hash_ops in_addr_data_hash_ops_free;
બાહ્ય અચલ સંરચના hash_ops in6_addr_hash_ops;
બાહ્ય અચલ સંરચના hash_ops in6_addr_hash_ops_free;

સ્થિર અંતઃરેખ શૂન્ય PTR_TO_IN4_ADDR(અચલ શૂન્ય *p, સંરચના in_addr *ret) {
        પ્રતિજ્ઞા(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

સ્થિર અંતઃરેખ શૂન્ય* IN4_ADDR_TO_PTR(અચલ સંરચના in_addr *a) {
        પ્રતિજ્ઞા(a);
        પાછા (શૂન્ય*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
