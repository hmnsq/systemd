/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <netinet/in.h>

#ضمن "forward.h"

اتحاد in_addr_union {
        بنية in_addr in;
        بنية in6_addr in6;
        uint8_t bytes[CONST_MAX(حجم(بنية in_addr), حجم(بنية in6_addr))];
};

بنية in_addr_data {
        صحيح family;
        اتحاد in_addr_union address;
};

bool in4_addr_is_null(ثابت بنية in_addr *a) _pure_;
ساكن مضمن bool in4_addr_is_set(ثابت بنية in_addr *a) {
        החזר !in4_addr_is_null(a);
}
bool in6_addr_is_null(ثابت بنية in6_addr *a) _pure_;
ساكن مضمن bool in6_addr_is_set(ثابت بنية in6_addr *a) {
        החזר !in6_addr_is_null(a);
}
صحيح in_addr_is_null(صحيح family, ثابت اتحاد in_addr_union *u) _pure_;
ساكن مضمن bool in_addr_is_set(صحيح family, ثابت اتحاد in_addr_union *u) {
        החזר in_addr_is_null(family, u) == 0;
}
ساكن مضمن صحيح in_addr_data_is_null(ثابت بنية in_addr_data *a) {
        أكد(a);
        החזר in_addr_is_null(a->family, &a->address);
}
ساكن مضمن bool in_addr_data_is_set(ثابت بنية in_addr_data *a) {
        החזר in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(ثابت بنية in_addr *a) _pure_;
bool in6_addr_is_multicast(ثابت بنية in6_addr *a) _pure_;
صحيح in_addr_is_multicast(صحيح family, ثابت اتحاد in_addr_union *u) _pure_;

bool in4_addr_is_link_local(ثابت بنية in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(ثابت بنية in_addr *a) _pure_;
bool in6_addr_is_link_local(ثابت بنية in6_addr *a) _pure_;
صحيح in_addr_is_link_local(صحيح family, ثابت اتحاد in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(ثابت بنية in6_addr *a) _pure_;

bool in4_addr_is_localhost(ثابت بنية in_addr *a) _pure_;
صحيح in_addr_is_localhost(صحيح family, ثابت اتحاد in_addr_union *u) _pure_;
صحيح in_addr_is_localhost_one(صحيح family, ثابت اتحاد in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(ثابت بنية in_addr *a) _pure_;
bool in4_addr_is_non_local(ثابت بنية in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(ثابت بنية in6_addr *a) _pure_;

bool in4_addr_equal(ثابت بنية in_addr *a, ثابت بنية in_addr *b) _pure_;
bool in6_addr_equal(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) _pure_;
صحيح in_addr_equal(صحيح family, ثابت اتحاد in_addr_union *a, ثابت اتحاد in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                ثابت بنية in_addr *a,
                unsigned aprefixlen,
                ثابت بنية in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                ثابت بنية in6_addr *a,
                unsigned aprefixlen,
                ثابت بنية in6_addr *b,
                unsigned bprefixlen) _pure_;
صحيح in_addr_prefix_intersect(
                صحيح family,
                ثابت اتحاد in_addr_union *a,
                unsigned aprefixlen,
                ثابت اتحاد in_addr_union *b,
                unsigned bprefixlen) _pure_;
صحيح in_addr_random_prefix(صحيح family, اتحاد in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
صحيح in_addr_prefix_range(
                صحيح family,
                ثابت اتحاد in_addr_union *in,
                unsigned prefixlen,
                اتحاد in_addr_union *ret_start,
                اتحاد in_addr_union *ret_end);

صحيح in_addr_to_string(صحيح family, ثابت اتحاد in_addr_union *u, حرف **ret);
ساكن مضمن صحيح in6_addr_to_string(ثابت بنية in6_addr *u, حرف **ret) {
        החזר in_addr_to_string(AF_INET6, (ثابت اتحاد in_addr_union*) u, ret);
}

ثابت حرف* typesafe_inet_ntop(صحيح family, ثابت اتحاد in_addr_union *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop4(ثابت بنية in_addr *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop6(ثابت بنية in6_addr *a, حرف *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (حرف[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (حرف[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (حرف[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

صحيح in_addr_prefix_to_string(
                صحيح family,
                ثابت اتحاد in_addr_union *u,
                unsigned prefixlen,
                حرف *buf,
                size_t buf_len);

ساكن مضمن ثابت حرف* _in_addr_prefix_to_string(
                صحيح family,
                ثابت اتحاد in_addr_union *u,
                unsigned prefixlen,
                حرف *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        החזר buf;
}
ساكن مضمن ثابت حرف* _in4_addr_prefix_to_string(ثابت بنية in_addr *a, unsigned prefixlen, حرف *buf, size_t buf_len) {
        החזר _in_addr_prefix_to_string(AF_INET, (ثابت اتحاد in_addr_union *) a, prefixlen, buf, buf_len);
}
ساكن مضمن ثابت حرف* _in6_addr_prefix_to_string(ثابت بنية in6_addr *a, unsigned prefixlen, حرف *buf, size_t buf_len) {
        החזר _in_addr_prefix_to_string(AF_INET6, (ثابت اتحاد in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (حرف[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (حرف[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (حرف[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

صحيح in_addr_port_ifindex_name_to_string(صحيح family, ثابت اتحاد in_addr_union *u, uint16_t port, صحيح ifindex, ثابت حرف *server_name, حرف **ret);
ساكن مضمن صحيح in_addr_ifindex_to_string(صحيح family, ثابت اتحاد in_addr_union *u, صحيح ifindex, حرف **ret) {
        החזר in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, عدم, ret);
}
ساكن مضمن صحيح in_addr_port_to_string(صحيح family, ثابت اتحاد in_addr_union *u, uint16_t port, حرف **ret) {
        החזר in_addr_port_ifindex_name_to_string(family, u, port, 0, عدم, ret);
}
صحيح in_addr_from_string(صحيح family, ثابت حرف *s, اتحاد in_addr_union *ret);
صحيح in_addr_from_string_auto(ثابت حرف *s, صحيح *ret_family, اتحاد in_addr_union *ret);

unsigned حرف in4_addr_netmask_to_prefixlen(ثابت بنية in_addr *addr);
بنية in_addr* in4_addr_prefixlen_to_netmask(بنية in_addr *addr, unsigned حرف prefixlen);
بنية in6_addr* in6_addr_prefixlen_to_netmask(بنية in6_addr *addr, unsigned حرف prefixlen);
صحيح in_addr_prefixlen_to_netmask(صحيح family, اتحاد in_addr_union *addr, unsigned حرف prefixlen);
صحيح in4_addr_default_prefixlen(ثابت بنية in_addr *addr, unsigned حرف *prefixlen);
صحيح in4_addr_default_subnet_mask(ثابت بنية in_addr *addr, بنية in_addr *mask);
صحيح in4_addr_mask(بنية in_addr *addr, unsigned حرف prefixlen);
صحيح in6_addr_mask(بنية in6_addr *addr, unsigned حرف prefixlen);
صحيح in_addr_mask(صحيح family, اتحاد in_addr_union *addr, unsigned حرف prefixlen);
صحيح in4_addr_prefix_covers_full(ثابت بنية in_addr *prefix, unsigned حرف prefixlen, ثابت بنية in_addr *address, unsigned حرف address_prefixlen);
صحيح in6_addr_prefix_covers_full(ثابت بنية in6_addr *prefix, unsigned حرف prefixlen, ثابت بنية in6_addr *address, unsigned حرف address_prefixlen);
صحيح in_addr_prefix_covers_full(صحيح family, ثابت اتحاد in_addr_union *prefix, unsigned حرف prefixlen, ثابت اتحاد in_addr_union *address, unsigned حرف address_prefixlen);
ساكن مضمن صحيح in4_addr_prefix_covers(ثابت بنية in_addr *prefix, unsigned حرف prefixlen, ثابت بنية in_addr *address) {
        החזר in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
ساكن مضمن صحيح in6_addr_prefix_covers(ثابت بنية in6_addr *prefix, unsigned حرف prefixlen, ثابت بنية in6_addr *address) {
        החזר in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
ساكن مضمن صحيح in_addr_prefix_covers(صحيح family, ثابت اتحاد in_addr_union *prefix, unsigned حرف prefixlen, ثابت اتحاد in_addr_union *address) {
        החזר in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
صحيح in_addr_parse_prefixlen(صحيح family, ثابت حرف *p, unsigned حرف *ret);
صحيح in_addr_prefix_from_string(ثابت حرف *p, صحيح family, اتحاد in_addr_union *ret_prefix, unsigned حرف *ret_prefixlen);

تعريفنوع تعداد InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

صحيح in_addr_prefix_from_string_auto_full(ثابت حرف *p, InAddrPrefixLenMode mode, صحيح *ret_family, اتحاد in_addr_union *ret_prefix, unsigned حرف *ret_prefixlen);
ساكن مضمن صحيح in_addr_prefix_from_string_auto(ثابت حرف *p, صحيح *ret_family, اتحاد in_addr_union *ret_prefix, unsigned حرف *ret_prefixlen) {
        החזר in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

ساكن مضمن size_t FAMILY_ADDRESS_SIZE(صحيح family) {
        بدل (family) {
                حالة AF_INET:
                        החזר 4;
                حالة AF_INET6:
                        החזר 16;
                افتراضي:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                صحيح _f = (f);                                           \
                _f == AF_INET ? حجم(بنية in_addr) :                \
                _f == AF_INET6 ? حجم(بنية in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((اتحاد in_addr_union) { .in6 = {} })

فراغ in_addr_hash_func(ثابت اتحاد in_addr_union *u, صحيح family, بنية siphash *state);
فراغ in_addr_data_hash_func(ثابت بنية in_addr_data *a, بنية siphash *state);
صحيح in_addr_data_compare_func(ثابت بنية in_addr_data *x, ثابت بنية in_addr_data *y);
فراغ in6_addr_hash_func(ثابت بنية in6_addr *addr, بنية siphash *state);
صحيح in6_addr_compare_func(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b);

خارجي ثابت بنية hash_ops in_addr_data_hash_ops;
خارجي ثابت بنية hash_ops in_addr_data_hash_ops_free;
خارجي ثابت بنية hash_ops in6_addr_hash_ops;
خارجي ثابت بنية hash_ops in6_addr_hash_ops_free;

ساكن مضمن فراغ PTR_TO_IN4_ADDR(ثابت فراغ *p, بنية in_addr *ret) {
        أكد(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ساكن مضمن فراغ* IN4_ADDR_TO_PTR(ثابت بنية in_addr *a) {
        أكد(a);
        החזר (فراغ*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
