/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <netinet/in.h>

#অন্তর্ভাব "forward.h"

সঙ্ঘ in_addr_union {
        সংরচনা in_addr in;
        সংরচনা in6_addr in6;
        uint8_t bytes[CONST_MAX(পরিমাণ(সংরচনা in_addr), পরিমাণ(সংরচনা in6_addr))];
};

সংরচনা in_addr_data {
        পূর্ণাঙ্ক family;
        সঙ্ঘ in_addr_union address;
};

bool in4_addr_is_null(ধ্রুব সংরচনা in_addr *a) _pure_;
স্থির অন্তঃস্থ bool in4_addr_is_set(ধ্রুব সংরচনা in_addr *a) {
        প্রত্যাগম !in4_addr_is_null(a);
}
bool in6_addr_is_null(ধ্রুব সংরচনা in6_addr *a) _pure_;
স্থির অন্তঃস্থ bool in6_addr_is_set(ধ্রুব সংরচনা in6_addr *a) {
        প্রত্যাগম !in6_addr_is_null(a);
}
পূর্ণাঙ্ক in_addr_is_null(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u) _pure_;
স্থির অন্তঃস্থ bool in_addr_is_set(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u) {
        প্রত্যাগম in_addr_is_null(family, u) == 0;
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক in_addr_data_is_null(ধ্রুব সংরচনা in_addr_data *a) {
        প্রতিজ্ঞা(a);
        প্রত্যাগম in_addr_is_null(a->family, &a->address);
}
স্থির অন্তঃস্থ bool in_addr_data_is_set(ধ্রুব সংরচনা in_addr_data *a) {
        প্রত্যাগম in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(ধ্রুব সংরচনা in_addr *a) _pure_;
bool in6_addr_is_multicast(ধ্রুব সংরচনা in6_addr *a) _pure_;
পূর্ণাঙ্ক in_addr_is_multicast(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(ধ্রুব সংরচনা in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(ধ্রুব সংরচনা in_addr *a) _pure_;
bool in6_addr_is_link_local(ধ্রুব সংরচনা in6_addr *a) _pure_;
পূর্ণাঙ্ক in_addr_is_link_local(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(ধ্রুব সংরচনা in6_addr *a) _pure_;

bool in4_addr_is_localhost(ধ্রুব সংরচনা in_addr *a) _pure_;
পূর্ণাঙ্ক in_addr_is_localhost(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u) _pure_;
পূর্ণাঙ্ক in_addr_is_localhost_one(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(ধ্রুব সংরচনা in_addr *a) _pure_;
bool in4_addr_is_non_local(ধ্রুব সংরচনা in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(ধ্রুব সংরচনা in6_addr *a) _pure_;

bool in4_addr_equal(ধ্রুব সংরচনা in_addr *a, ধ্রুব সংরচনা in_addr *b) _pure_;
bool in6_addr_equal(ধ্রুব সংরচনা in6_addr *a, ধ্রুব সংরচনা in6_addr *b) _pure_;
পূর্ণাঙ্ক in_addr_equal(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *a, ধ্রুব সঙ্ঘ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                ধ্রুব সংরচনা in_addr *a,
                unsigned aprefixlen,
                ধ্রুব সংরচনা in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                ধ্রুব সংরচনা in6_addr *a,
                unsigned aprefixlen,
                ধ্রুব সংরচনা in6_addr *b,
                unsigned bprefixlen) _pure_;
পূর্ণাঙ্ক in_addr_prefix_intersect(
                পূর্ণাঙ্ক family,
                ধ্রুব সঙ্ঘ in_addr_union *a,
                unsigned aprefixlen,
                ধ্রুব সঙ্ঘ in_addr_union *b,
                unsigned bprefixlen) _pure_;
পূর্ণাঙ্ক in_addr_random_prefix(পূর্ণাঙ্ক family, সঙ্ঘ in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
পূর্ণাঙ্ক in_addr_prefix_range(
                পূর্ণাঙ্ক family,
                ধ্রুব সঙ্ঘ in_addr_union *in,
                unsigned prefixlen,
                সঙ্ঘ in_addr_union *ret_start,
                সঙ্ঘ in_addr_union *ret_end);

পূর্ণাঙ্ক in_addr_to_string(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u, বর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক in6_addr_to_string(ধ্রুব সংরচনা in6_addr *u, বর্ণ **ret) {
        প্রত্যাগম in_addr_to_string(AF_INET6, (ধ্রুব সঙ্ঘ in_addr_union*) u, ret);
}

ধ্রুব বর্ণ* typesafe_inet_ntop(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *a, বর্ণ *buf, size_t len);
ধ্রুব বর্ণ* typesafe_inet_ntop4(ধ্রুব সংরচনা in_addr *a, বর্ণ *buf, size_t len);
ধ্রুব বর্ণ* typesafe_inet_ntop6(ধ্রুব সংরচনা in6_addr *a, বর্ণ *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (বর্ণ[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (বর্ণ[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (বর্ণ[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

পূর্ণাঙ্ক in_addr_prefix_to_string(
                পূর্ণাঙ্ক family,
                ধ্রুব সঙ্ঘ in_addr_union *u,
                unsigned prefixlen,
                বর্ণ *buf,
                size_t buf_len);

স্থির অন্তঃস্থ ধ্রুব বর্ণ* _in_addr_prefix_to_string(
                পূর্ণাঙ্ক family,
                ধ্রুব সঙ্ঘ in_addr_union *u,
                unsigned prefixlen,
                বর্ণ *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        প্রত্যাগম buf;
}
স্থির অন্তঃস্থ ধ্রুব বর্ণ* _in4_addr_prefix_to_string(ধ্রুব সংরচনা in_addr *a, unsigned prefixlen, বর্ণ *buf, size_t buf_len) {
        প্রত্যাগম _in_addr_prefix_to_string(AF_INET, (ধ্রুব সঙ্ঘ in_addr_union *) a, prefixlen, buf, buf_len);
}
স্থির অন্তঃস্থ ধ্রুব বর্ণ* _in6_addr_prefix_to_string(ধ্রুব সংরচনা in6_addr *a, unsigned prefixlen, বর্ণ *buf, size_t buf_len) {
        প্রত্যাগম _in_addr_prefix_to_string(AF_INET6, (ধ্রুব সঙ্ঘ in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (বর্ণ[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (বর্ণ[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (বর্ণ[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

পূর্ণাঙ্ক in_addr_port_ifindex_name_to_string(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u, uint16_t port, পূর্ণাঙ্ক ifindex, ধ্রুব বর্ণ *server_name, বর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক in_addr_ifindex_to_string(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u, পূর্ণাঙ্ক ifindex, বর্ণ **ret) {
        প্রত্যাগম in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, রিক্ত, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক in_addr_port_to_string(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *u, uint16_t port, বর্ণ **ret) {
        প্রত্যাগম in_addr_port_ifindex_name_to_string(family, u, port, 0, রিক্ত, ret);
}
পূর্ণাঙ্ক in_addr_from_string(পূর্ণাঙ্ক family, ধ্রুব বর্ণ *s, সঙ্ঘ in_addr_union *ret);
পূর্ণাঙ্ক in_addr_from_string_auto(ধ্রুব বর্ণ *s, পূর্ণাঙ্ক *ret_family, সঙ্ঘ in_addr_union *ret);

unsigned বর্ণ in4_addr_netmask_to_prefixlen(ধ্রুব সংরচনা in_addr *addr);
সংরচনা in_addr* in4_addr_prefixlen_to_netmask(সংরচনা in_addr *addr, unsigned বর্ণ prefixlen);
সংরচনা in6_addr* in6_addr_prefixlen_to_netmask(সংরচনা in6_addr *addr, unsigned বর্ণ prefixlen);
পূর্ণাঙ্ক in_addr_prefixlen_to_netmask(পূর্ণাঙ্ক family, সঙ্ঘ in_addr_union *addr, unsigned বর্ণ prefixlen);
পূর্ণাঙ্ক in4_addr_default_prefixlen(ধ্রুব সংরচনা in_addr *addr, unsigned বর্ণ *prefixlen);
পূর্ণাঙ্ক in4_addr_default_subnet_mask(ধ্রুব সংরচনা in_addr *addr, সংরচনা in_addr *mask);
পূর্ণাঙ্ক in4_addr_mask(সংরচনা in_addr *addr, unsigned বর্ণ prefixlen);
পূর্ণাঙ্ক in6_addr_mask(সংরচনা in6_addr *addr, unsigned বর্ণ prefixlen);
পূর্ণাঙ্ক in_addr_mask(পূর্ণাঙ্ক family, সঙ্ঘ in_addr_union *addr, unsigned বর্ণ prefixlen);
পূর্ণাঙ্ক in4_addr_prefix_covers_full(ধ্রুব সংরচনা in_addr *prefix, unsigned বর্ণ prefixlen, ধ্রুব সংরচনা in_addr *address, unsigned বর্ণ address_prefixlen);
পূর্ণাঙ্ক in6_addr_prefix_covers_full(ধ্রুব সংরচনা in6_addr *prefix, unsigned বর্ণ prefixlen, ধ্রুব সংরচনা in6_addr *address, unsigned বর্ণ address_prefixlen);
পূর্ণাঙ্ক in_addr_prefix_covers_full(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *prefix, unsigned বর্ণ prefixlen, ধ্রুব সঙ্ঘ in_addr_union *address, unsigned বর্ণ address_prefixlen);
স্থির অন্তঃস্থ পূর্ণাঙ্ক in4_addr_prefix_covers(ধ্রুব সংরচনা in_addr *prefix, unsigned বর্ণ prefixlen, ধ্রুব সংরচনা in_addr *address) {
        প্রত্যাগম in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক in6_addr_prefix_covers(ধ্রুব সংরচনা in6_addr *prefix, unsigned বর্ণ prefixlen, ধ্রুব সংরচনা in6_addr *address) {
        প্রত্যাগম in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক in_addr_prefix_covers(পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *prefix, unsigned বর্ণ prefixlen, ধ্রুব সঙ্ঘ in_addr_union *address) {
        প্রত্যাগম in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
পূর্ণাঙ্ক in_addr_parse_prefixlen(পূর্ণাঙ্ক family, ধ্রুব বর্ণ *p, unsigned বর্ণ *ret);
পূর্ণাঙ্ক in_addr_prefix_from_string(ধ্রুব বর্ণ *p, পূর্ণাঙ্ক family, সঙ্ঘ in_addr_union *ret_prefix, unsigned বর্ণ *ret_prefixlen);

প্রকারনির্দেশ গণনা InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

পূর্ণাঙ্ক in_addr_prefix_from_string_auto_full(ধ্রুব বর্ণ *p, InAddrPrefixLenMode mode, পূর্ণাঙ্ক *ret_family, সঙ্ঘ in_addr_union *ret_prefix, unsigned বর্ণ *ret_prefixlen);
স্থির অন্তঃস্থ পূর্ণাঙ্ক in_addr_prefix_from_string_auto(ধ্রুব বর্ণ *p, পূর্ণাঙ্ক *ret_family, সঙ্ঘ in_addr_union *ret_prefix, unsigned বর্ণ *ret_prefixlen) {
        প্রত্যাগম in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

স্থির অন্তঃস্থ size_t FAMILY_ADDRESS_SIZE(পূর্ণাঙ্ক family) {
        বিকল্প (family) {
                পক্ষ AF_INET:
                        প্রত্যাগম 4;
                পক্ষ AF_INET6:
                        প্রত্যাগম 16;
                স্বভাব:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                পূর্ণাঙ্ক _f = (f);                                           \
                _f == AF_INET ? পরিমাণ(সংরচনা in_addr) :                \
                _f == AF_INET6 ? পরিমাণ(সংরচনা in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((সঙ্ঘ in_addr_union) { .in6 = {} })

শূন্য in_addr_hash_func(ধ্রুব সঙ্ঘ in_addr_union *u, পূর্ণাঙ্ক family, সংরচনা siphash *state);
শূন্য in_addr_data_hash_func(ধ্রুব সংরচনা in_addr_data *a, সংরচনা siphash *state);
পূর্ণাঙ্ক in_addr_data_compare_func(ধ্রুব সংরচনা in_addr_data *x, ধ্রুব সংরচনা in_addr_data *y);
শূন্য in6_addr_hash_func(ধ্রুব সংরচনা in6_addr *addr, সংরচনা siphash *state);
পূর্ণাঙ্ক in6_addr_compare_func(ধ্রুব সংরচনা in6_addr *a, ধ্রুব সংরচনা in6_addr *b);

বাহ্য ধ্রুব সংরচনা hash_ops in_addr_data_hash_ops;
বাহ্য ধ্রুব সংরচনা hash_ops in_addr_data_hash_ops_free;
বাহ্য ধ্রুব সংরচনা hash_ops in6_addr_hash_ops;
বাহ্য ধ্রুব সংরচনা hash_ops in6_addr_hash_ops_free;

স্থির অন্তঃস্থ শূন্য PTR_TO_IN4_ADDR(ধ্রুব শূন্য *p, সংরচনা in_addr *ret) {
        প্রতিজ্ঞা(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

স্থির অন্তঃস্থ শূন্য* IN4_ADDR_TO_PTR(ধ্রুব সংরচনা in_addr *a) {
        প্রতিজ্ঞা(a);
        প্রত্যাগম (শূন্য*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
