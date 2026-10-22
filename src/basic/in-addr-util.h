/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <netinet/in.h>

#অন্তর্ভাব "forward.h"

সংঘ in_addr_union {
        কাঠামো in_addr in;
        কাঠামো in6_addr in6;
        uint8_t bytes[CONST_MAX(আকার(কাঠামো in_addr), আকার(কাঠামো in6_addr))];
};

কাঠামো in_addr_তথ্য {
        পূর্ণসংখ্যা family;
        সংঘ in_addr_union address;
};

bool in4_addr_is_null(ধ্রুবক কাঠামো in_addr *a) _pure_;
স্থির অন্তঃরেখ bool in4_addr_is_set(ধ্রুবক কাঠামো in_addr *a) {
        ফেরত !in4_addr_is_null(a);
}
bool in6_addr_is_null(ধ্রুবক কাঠামো in6_addr *a) _pure_;
স্থির অন্তঃরেখ bool in6_addr_is_set(ধ্রুবক কাঠামো in6_addr *a) {
        ফেরত !in6_addr_is_null(a);
}
পূর্ণসংখ্যা in_addr_is_null(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) _pure_;
স্থির অন্তঃরেখ bool in_addr_is_set(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) {
        ফেরত in_addr_is_null(family, u) == 0;
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা in_addr_তথ্য_is_null(ধ্রুবক কাঠামো in_addr_তথ্য *a) {
        প্রতিজ্ঞা(a);
        ফেরত in_addr_is_null(a->family, &a->address);
}
স্থির অন্তঃরেখ bool in_addr_তথ্য_is_set(ধ্রুবক কাঠামো in_addr_তথ্য *a) {
        ফেরত in_addr_তথ্য_is_null(a);
}

bool in4_addr_is_multicast(ধ্রুবক কাঠামো in_addr *a) _pure_;
bool in6_addr_is_multicast(ধ্রুবক কাঠামো in6_addr *a) _pure_;
পূর্ণসংখ্যা in_addr_is_multicast(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(ধ্রুবক কাঠামো in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(ধ্রুবক কাঠামো in_addr *a) _pure_;
bool in6_addr_is_link_local(ধ্রুবক কাঠামো in6_addr *a) _pure_;
পূর্ণসংখ্যা in_addr_is_link_local(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(ধ্রুবক কাঠামো in6_addr *a) _pure_;

bool in4_addr_is_localhost(ধ্রুবক কাঠামো in_addr *a) _pure_;
পূর্ণসংখ্যা in_addr_is_localhost(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) _pure_;
পূর্ণসংখ্যা in_addr_is_localhost_one(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(ধ্রুবক কাঠামো in_addr *a) _pure_;
bool in4_addr_is_non_local(ধ্রুবক কাঠামো in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(ধ্রুবক কাঠামো in6_addr *a) _pure_;

bool in4_addr_equal(ধ্রুবক কাঠামো in_addr *a, ধ্রুবক কাঠামো in_addr *b) _pure_;
bool in6_addr_equal(ধ্রুবক কাঠামো in6_addr *a, ধ্রুবক কাঠামো in6_addr *b) _pure_;
পূর্ণসংখ্যা in_addr_equal(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *a, ধ্রুবক সংঘ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                ধ্রুবক কাঠামো in_addr *a,
                unsigned aprefixlen,
                ধ্রুবক কাঠামো in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                ধ্রুবক কাঠামো in6_addr *a,
                unsigned aprefixlen,
                ধ্রুবক কাঠামো in6_addr *b,
                unsigned bprefixlen) _pure_;
পূর্ণসংখ্যা in_addr_prefix_intersect(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *a,
                unsigned aprefixlen,
                ধ্রুবক সংঘ in_addr_union *b,
                unsigned bprefixlen) _pure_;
পূর্ণসংখ্যা in_addr_random_prefix(পূর্ণসংখ্যা family, সংঘ in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
পূর্ণসংখ্যা in_addr_prefix_range(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *in,
                unsigned prefixlen,
                সংঘ in_addr_union *ret_start,
                সংঘ in_addr_union *ret_end);

পূর্ণসংখ্যা in_addr_to_string(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u, অক্ষর **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা in6_addr_to_string(ধ্রুবক কাঠামো in6_addr *u, অক্ষর **ret) {
        ফেরত in_addr_to_string(AF_INET6, (ধ্রুবক সংঘ in_addr_union*) u, ret);
}

ধ্রুবক অক্ষর* typesafe_inet_ntop(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *a, অক্ষর *buf, size_t len);
ধ্রুবক অক্ষর* typesafe_inet_ntop4(ধ্রুবক কাঠামো in_addr *a, অক্ষর *buf, size_t len);
ধ্রুবক অক্ষর* typesafe_inet_ntop6(ধ্রুবক কাঠামো in6_addr *a, অক্ষর *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (অক্ষর[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (অক্ষর[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (অক্ষর[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

পূর্ণসংখ্যা in_addr_prefix_to_string(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *u,
                unsigned prefixlen,
                অক্ষর *buf,
                size_t buf_len);

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* _in_addr_prefix_to_string(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *u,
                unsigned prefixlen,
                অক্ষর *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        ফেরত buf;
}
স্থির অন্তঃরেখ ধ্রুবক অক্ষর* _in4_addr_prefix_to_string(ধ্রুবক কাঠামো in_addr *a, unsigned prefixlen, অক্ষর *buf, size_t buf_len) {
        ফেরত _in_addr_prefix_to_string(AF_INET, (ধ্রুবক সংঘ in_addr_union *) a, prefixlen, buf, buf_len);
}
স্থির অন্তঃরেখ ধ্রুবক অক্ষর* _in6_addr_prefix_to_string(ধ্রুবক কাঠামো in6_addr *a, unsigned prefixlen, অক্ষর *buf, size_t buf_len) {
        ফেরত _in_addr_prefix_to_string(AF_INET6, (ধ্রুবক সংঘ in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (অক্ষর[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (অক্ষর[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (অক্ষর[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

পূর্ণসংখ্যা in_addr_port_ifindex_name_to_string(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u, uint16_t port, পূর্ণসংখ্যা ifindex, ধ্রুবক অক্ষর *server_name, অক্ষর **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা in_addr_ifindex_to_string(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u, পূর্ণসংখ্যা ifindex, অক্ষর **ret) {
        ফেরত in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, ফাঁকা, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা in_addr_port_to_string(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u, uint16_t port, অক্ষর **ret) {
        ফেরত in_addr_port_ifindex_name_to_string(family, u, port, 0, ফাঁকা, ret);
}
পূর্ণসংখ্যা in_addr_from_string(পূর্ণসংখ্যা family, ধ্রুবক অক্ষর *s, সংঘ in_addr_union *ret);
পূর্ণসংখ্যা in_addr_from_string_auto(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret_family, সংঘ in_addr_union *ret);

unsigned অক্ষর in4_addr_netmask_to_prefixlen(ধ্রুবক কাঠামো in_addr *addr);
কাঠামো in_addr* in4_addr_prefixlen_to_netmask(কাঠামো in_addr *addr, unsigned অক্ষর prefixlen);
কাঠামো in6_addr* in6_addr_prefixlen_to_netmask(কাঠামো in6_addr *addr, unsigned অক্ষর prefixlen);
পূর্ণসংখ্যা in_addr_prefixlen_to_netmask(পূর্ণসংখ্যা family, সংঘ in_addr_union *addr, unsigned অক্ষর prefixlen);
পূর্ণসংখ্যা in4_addr_default_prefixlen(ধ্রুবক কাঠামো in_addr *addr, unsigned অক্ষর *prefixlen);
পূর্ণসংখ্যা in4_addr_default_subnet_mask(ধ্রুবক কাঠামো in_addr *addr, কাঠামো in_addr *mask);
পূর্ণসংখ্যা in4_addr_mask(কাঠামো in_addr *addr, unsigned অক্ষর prefixlen);
পূর্ণসংখ্যা in6_addr_mask(কাঠামো in6_addr *addr, unsigned অক্ষর prefixlen);
পূর্ণসংখ্যা in_addr_mask(পূর্ণসংখ্যা family, সংঘ in_addr_union *addr, unsigned অক্ষর prefixlen);
পূর্ণসংখ্যা in4_addr_prefix_covers_full(ধ্রুবক কাঠামো in_addr *prefix, unsigned অক্ষর prefixlen, ধ্রুবক কাঠামো in_addr *address, unsigned অক্ষর address_prefixlen);
পূর্ণসংখ্যা in6_addr_prefix_covers_full(ধ্রুবক কাঠামো in6_addr *prefix, unsigned অক্ষর prefixlen, ধ্রুবক কাঠামো in6_addr *address, unsigned অক্ষর address_prefixlen);
পূর্ণসংখ্যা in_addr_prefix_covers_full(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *prefix, unsigned অক্ষর prefixlen, ধ্রুবক সংঘ in_addr_union *address, unsigned অক্ষর address_prefixlen);
স্থির অন্তঃরেখ পূর্ণসংখ্যা in4_addr_prefix_covers(ধ্রুবক কাঠামো in_addr *prefix, unsigned অক্ষর prefixlen, ধ্রুবক কাঠামো in_addr *address) {
        ফেরত in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা in6_addr_prefix_covers(ধ্রুবক কাঠামো in6_addr *prefix, unsigned অক্ষর prefixlen, ধ্রুবক কাঠামো in6_addr *address) {
        ফেরত in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা in_addr_prefix_covers(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *prefix, unsigned অক্ষর prefixlen, ধ্রুবক সংঘ in_addr_union *address) {
        ফেরত in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
পূর্ণসংখ্যা in_addr_parse_prefixlen(পূর্ণসংখ্যা family, ধ্রুবক অক্ষর *p, unsigned অক্ষর *ret);
পূর্ণসংখ্যা in_addr_prefix_from_string(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা family, সংঘ in_addr_union *ret_prefix, unsigned অক্ষর *ret_prefixlen);

প্রকারলেখ গণনা InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

পূর্ণসংখ্যা in_addr_prefix_from_string_auto_full(ধ্রুবক অক্ষর *p, InAddrPrefixLenMode mode, পূর্ণসংখ্যা *ret_family, সংঘ in_addr_union *ret_prefix, unsigned অক্ষর *ret_prefixlen);
স্থির অন্তঃরেখ পূর্ণসংখ্যা in_addr_prefix_from_string_auto(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা *ret_family, সংঘ in_addr_union *ret_prefix, unsigned অক্ষর *ret_prefixlen) {
        ফেরত in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

স্থির অন্তঃরেখ size_t FAMILY_ADDRESS_আকার(পূর্ণসংখ্যা family) {
        নির্বাচন (family) {
                ক্ষেত্র AF_INET:
                        ফেরত 4;
                ক্ষেত্র AF_INET6:
                        ফেরত 16;
                পূর্বনির্ধারিত:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_আকার_SAFE(f)                                     \
        ({                                                              \
                পূর্ণসংখ্যা _f = (f);                                           \
                _f == AF_INET ? আকার(কাঠামো in_addr) :                \
                _f == AF_INET6 ? আকার(কাঠামো in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((সংঘ in_addr_union) { .in6 = {} })

শূন্য in_addr_hash_func(ধ্রুবক সংঘ in_addr_union *u, পূর্ণসংখ্যা family, কাঠামো siphash *state);
শূন্য in_addr_তথ্য_hash_func(ধ্রুবক কাঠামো in_addr_তথ্য *a, কাঠামো siphash *state);
পূর্ণসংখ্যা in_addr_তথ্য_compare_func(ধ্রুবক কাঠামো in_addr_তথ্য *x, ধ্রুবক কাঠামো in_addr_তথ্য *y);
শূন্য in6_addr_hash_func(ধ্রুবক কাঠামো in6_addr *addr, কাঠামো siphash *state);
পূর্ণসংখ্যা in6_addr_compare_func(ধ্রুবক কাঠামো in6_addr *a, ধ্রুবক কাঠামো in6_addr *b);

বাহ্যিক ধ্রুবক কাঠামো hash_ops in_addr_তথ্য_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops in_addr_তথ্য_hash_ops_free;
বাহ্যিক ধ্রুবক কাঠামো hash_ops in6_addr_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops in6_addr_hash_ops_free;

স্থির অন্তঃরেখ শূন্য PTR_TO_IN4_ADDR(ধ্রুবক শূন্য *p, কাঠামো in_addr *ret) {
        প্রতিজ্ঞা(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

স্থির অন্তঃরেখ শূন্য* IN4_ADDR_TO_PTR(ধ্রুবক কাঠামো in_addr *a) {
        প্রতিজ্ঞা(a);
        ফেরত (শূন্য*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
