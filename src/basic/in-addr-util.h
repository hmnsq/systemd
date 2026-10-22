/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <netinet/in.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସଂଘ in_addr_union {
        ସଂରଚନା in_addr in;
        ସଂରଚନା in6_addr in6;
        uint8_t bytes[CONST_MAX(ଆକାର(ସଂରଚନା in_addr), ଆକାର(ସଂରଚନା in6_addr))];
};

ସଂରଚନା in_addr_data {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା family;
        ସଂଘ in_addr_union address;
};

bool in4_addr_is_null(ଅଚଳ ସଂରଚନା in_addr *a) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool in4_addr_is_set(ଅଚଳ ସଂରଚନା in_addr *a) {
        ଫେରନ୍ତୁ !in4_addr_is_null(a);
}
bool in6_addr_is_null(ଅଚଳ ସଂରଚନା in6_addr *a) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool in6_addr_is_set(ଅଚଳ ସଂରଚନା in6_addr *a) {
        ଫେରନ୍ତୁ !in6_addr_is_null(a);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_null(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool in_addr_is_set(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) {
        ଫେରନ୍ତୁ in_addr_is_null(family, u) == 0;
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_data_is_null(ଅଚଳ ସଂରଚନା in_addr_data *a) {
        ପ୍ରତିଜ୍ଞା(a);
        ଫେରନ୍ତୁ in_addr_is_null(a->family, &a->address);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool in_addr_data_is_set(ଅଚଳ ସଂରଚନା in_addr_data *a) {
        ଫେରନ୍ତୁ in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(ଅଚଳ ସଂରଚନା in_addr *a) _pure_;
bool in6_addr_is_multicast(ଅଚଳ ସଂରଚନା in6_addr *a) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_multicast(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(ଅଚଳ ସଂରଚନା in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(ଅଚଳ ସଂରଚନା in_addr *a) _pure_;
bool in6_addr_is_link_local(ଅଚଳ ସଂରଚନା in6_addr *a) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_link_local(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(ଅଚଳ ସଂରଚନା in6_addr *a) _pure_;

bool in4_addr_is_localhost(ଅଚଳ ସଂରଚନା in_addr *a) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_localhost(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_localhost_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(ଅଚଳ ସଂରଚନା in_addr *a) _pure_;
bool in4_addr_is_non_local(ଅଚଳ ସଂରଚନା in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(ଅଚଳ ସଂରଚନା in6_addr *a) _pure_;

bool in4_addr_equal(ଅଚଳ ସଂରଚନା in_addr *a, ଅଚଳ ସଂରଚନା in_addr *b) _pure_;
bool in6_addr_equal(ଅଚଳ ସଂରଚନା in6_addr *a, ଅଚଳ ସଂରଚନା in6_addr *b) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_equal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *a, ଅଚଳ ସଂଘ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                ଅଚଳ ସଂରଚନା in_addr *a,
                unsigned aprefixlen,
                ଅଚଳ ସଂରଚନା in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                ଅଚଳ ସଂରଚନା in6_addr *a,
                unsigned aprefixlen,
                ଅଚଳ ସଂରଚନା in6_addr *b,
                unsigned bprefixlen) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_intersect(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *a,
                unsigned aprefixlen,
                ଅଚଳ ସଂଘ in_addr_union *b,
                unsigned bprefixlen) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_random_prefix(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂଘ in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_range(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *in,
                unsigned prefixlen,
                ସଂଘ in_addr_union *ret_start,
                ସଂଘ in_addr_union *ret_end);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u, ଅକ୍ଷର **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_to_string(ଅଚଳ ସଂରଚନା in6_addr *u, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ in_addr_to_string(AF_INET6, (ଅଚଳ ସଂଘ in_addr_union*) u, ret);
}

ଅଚଳ ଅକ୍ଷର* typesafe_inet_ntop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *a, ଅକ୍ଷର *buf, size_t len);
ଅଚଳ ଅକ୍ଷର* typesafe_inet_ntop4(ଅଚଳ ସଂରଚନା in_addr *a, ଅକ୍ଷର *buf, size_t len);
ଅଚଳ ଅକ୍ଷର* typesafe_inet_ntop6(ଅଚଳ ସଂରଚନା in6_addr *a, ଅକ୍ଷର *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (ଅକ୍ଷର[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (ଅକ୍ଷର[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (ଅକ୍ଷର[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_to_string(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *u,
                unsigned prefixlen,
                ଅକ୍ଷର *buf,
                size_t buf_len);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* _in_addr_prefix_to_string(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *u,
                unsigned prefixlen,
                ଅକ୍ଷର *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        ଫେରନ୍ତୁ buf;
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* _in4_addr_prefix_to_string(ଅଚଳ ସଂରଚନା in_addr *a, unsigned prefixlen, ଅକ୍ଷର *buf, size_t buf_len) {
        ଫେରନ୍ତୁ _in_addr_prefix_to_string(AF_INET, (ଅଚଳ ସଂଘ in_addr_union *) a, prefixlen, buf, buf_len);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* _in6_addr_prefix_to_string(ଅଚଳ ସଂରଚନା in6_addr *a, unsigned prefixlen, ଅକ୍ଷର *buf, size_t buf_len) {
        ଫେରନ୍ତୁ _in_addr_prefix_to_string(AF_INET6, (ଅଚଳ ସଂଘ in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (ଅକ୍ଷର[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (ଅକ୍ଷର[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (ଅକ୍ଷର[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_port_ifindex_name_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u, uint16_t port, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, ଅଚଳ ଅକ୍ଷର *server_name, ଅକ୍ଷର **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_ifindex_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, ଖାଲି, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_port_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u, uint16_t port, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ in_addr_port_ifindex_name_to_string(family, u, port, 0, ଖାଲି, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_from_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ଅକ୍ଷର *s, ସଂଘ in_addr_union *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_from_string_auto(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_family, ସଂଘ in_addr_union *ret);

unsigned ଅକ୍ଷର in4_addr_netmask_to_prefixlen(ଅଚଳ ସଂରଚନା in_addr *addr);
ସଂରଚନା in_addr* in4_addr_prefixlen_to_netmask(ସଂରଚନା in_addr *addr, unsigned ଅକ୍ଷର prefixlen);
ସଂରଚନା in6_addr* in6_addr_prefixlen_to_netmask(ସଂରଚନା in6_addr *addr, unsigned ଅକ୍ଷର prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefixlen_to_netmask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂଘ in_addr_union *addr, unsigned ଅକ୍ଷର prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_default_prefixlen(ଅଚଳ ସଂରଚନା in_addr *addr, unsigned ଅକ୍ଷର *prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_default_subnet_mask(ଅଚଳ ସଂରଚନା in_addr *addr, ସଂରଚନା in_addr *mask);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_mask(ସଂରଚନା in_addr *addr, unsigned ଅକ୍ଷର prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_mask(ସଂରଚନା in6_addr *addr, unsigned ଅକ୍ଷର prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_mask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂଘ in_addr_union *addr, unsigned ଅକ୍ଷର prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_prefix_covers_full(ଅଚଳ ସଂରଚନା in_addr *prefix, unsigned ଅକ୍ଷର prefixlen, ଅଚଳ ସଂରଚନା in_addr *address, unsigned ଅକ୍ଷର address_prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_prefix_covers_full(ଅଚଳ ସଂରଚନା in6_addr *prefix, unsigned ଅକ୍ଷର prefixlen, ଅଚଳ ସଂରଚନା in6_addr *address, unsigned ଅକ୍ଷର address_prefixlen);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_covers_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *prefix, unsigned ଅକ୍ଷର prefixlen, ଅଚଳ ସଂଘ in_addr_union *address, unsigned ଅକ୍ଷର address_prefixlen);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_prefix_covers(ଅଚଳ ସଂରଚନା in_addr *prefix, unsigned ଅକ୍ଷର prefixlen, ଅଚଳ ସଂରଚନା in_addr *address) {
        ଫେରନ୍ତୁ in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_prefix_covers(ଅଚଳ ସଂରଚନା in6_addr *prefix, unsigned ଅକ୍ଷର prefixlen, ଅଚଳ ସଂରଚନା in6_addr *address) {
        ଫେରନ୍ତୁ in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_covers(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *prefix, unsigned ଅକ୍ଷର prefixlen, ଅଚଳ ସଂଘ in_addr_union *address) {
        ଫେରନ୍ତୁ in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_parse_prefixlen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ଅକ୍ଷର *p, unsigned ଅକ୍ଷର *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_from_string(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂଘ in_addr_union *ret_prefix, unsigned ଅକ୍ଷର *ret_prefixlen);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_from_string_auto_full(ଅଚଳ ଅକ୍ଷର *p, InAddrPrefixLenMode mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_family, ସଂଘ in_addr_union *ret_prefix, unsigned ଅକ୍ଷର *ret_prefixlen);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_from_string_auto(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_family, ସଂଘ in_addr_union *ret_prefix, unsigned ଅକ୍ଷର *ret_prefixlen) {
        ଫେରନ୍ତୁ in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ size_t FAMILY_ADDRESS_SIZE(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family) {
        ବାଛ (family) {
                ସ୍ଥିତି AF_INET:
                        ଫେରନ୍ତୁ 4;
                ସ୍ଥିତି AF_INET6:
                        ଫେରନ୍ତୁ 16;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା _f = (f);                                           \
                _f == AF_INET ? ଆକାର(ସଂରଚନା in_addr) :                \
                _f == AF_INET6 ? ଆକାର(ସଂରଚନା in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((ସଂଘ in_addr_union) { .in6 = {} })

ଶୂନ୍ୟ in_addr_hash_func(ଅଚଳ ସଂଘ in_addr_union *u, ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂରଚନା siphash *state);
ଶୂନ୍ୟ in_addr_data_hash_func(ଅଚଳ ସଂରଚନା in_addr_data *a, ସଂରଚନା siphash *state);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_data_compare_func(ଅଚଳ ସଂରଚନା in_addr_data *x, ଅଚଳ ସଂରଚନା in_addr_data *y);
ଶୂନ୍ୟ in6_addr_hash_func(ଅଚଳ ସଂରଚନା in6_addr *addr, ସଂରଚନା siphash *state);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_compare_func(ଅଚଳ ସଂରଚନା in6_addr *a, ଅଚଳ ସଂରଚନା in6_addr *b);

ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops in_addr_data_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops in_addr_data_hash_ops_free;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops in6_addr_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops in6_addr_hash_ops_free;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ PTR_TO_IN4_ADDR(ଅଚଳ ଶୂନ୍ୟ *p, ସଂରଚନା in_addr *ret) {
        ପ୍ରତିଜ୍ଞା(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* IN4_ADDR_TO_PTR(ଅଚଳ ସଂରଚନା in_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);
        ଫେରନ୍ତୁ (ଶୂନ୍ୟ*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
