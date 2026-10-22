/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <netinet/in.h>

#شامل "forward.h"

اجتماع in_addr_union {
        ساختار in_addr in;
        ساختار in6_addr in6;
        uint8_t bytes[CONST_MAX(اندازه(ساختار in_addr), اندازه(ساختار in6_addr))];
};

ساختار in_addr_data {
        صحیح family;
        اجتماع in_addr_union address;
};

bool in4_addr_is_null(ثابت ساختار in_addr *a) _pure_;
ایستا درونخطی bool in4_addr_is_set(ثابت ساختار in_addr *a) {
        بازگشت !in4_addr_is_null(a);
}
bool in6_addr_is_null(ثابت ساختار in6_addr *a) _pure_;
ایستا درونخطی bool in6_addr_is_set(ثابت ساختار in6_addr *a) {
        بازگشت !in6_addr_is_null(a);
}
صحیح in_addr_is_null(صحیح family, ثابت اجتماع in_addr_union *u) _pure_;
ایستا درونخطی bool in_addr_is_set(صحیح family, ثابت اجتماع in_addr_union *u) {
        بازگشت in_addr_is_null(family, u) == 0;
}
ایستا درونخطی صحیح in_addr_data_is_null(ثابت ساختار in_addr_data *a) {
        أكد(a);
        بازگشت in_addr_is_null(a->family, &a->address);
}
ایستا درونخطی bool in_addr_data_is_set(ثابت ساختار in_addr_data *a) {
        بازگشت in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(ثابت ساختار in_addr *a) _pure_;
bool in6_addr_is_multicast(ثابت ساختار in6_addr *a) _pure_;
صحیح in_addr_is_multicast(صحیح family, ثابت اجتماع in_addr_union *u) _pure_;

bool in4_addr_is_link_local(ثابت ساختار in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(ثابت ساختار in_addr *a) _pure_;
bool in6_addr_is_link_local(ثابت ساختار in6_addr *a) _pure_;
صحیح in_addr_is_link_local(صحیح family, ثابت اجتماع in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(ثابت ساختار in6_addr *a) _pure_;

bool in4_addr_is_localhost(ثابت ساختار in_addr *a) _pure_;
صحیح in_addr_is_localhost(صحیح family, ثابت اجتماع in_addr_union *u) _pure_;
صحیح in_addr_is_localhost_one(صحیح family, ثابت اجتماع in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(ثابت ساختار in_addr *a) _pure_;
bool in4_addr_is_non_local(ثابت ساختار in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(ثابت ساختار in6_addr *a) _pure_;

bool in4_addr_equal(ثابت ساختار in_addr *a, ثابت ساختار in_addr *b) _pure_;
bool in6_addr_equal(ثابت ساختار in6_addr *a, ثابت ساختار in6_addr *b) _pure_;
صحیح in_addr_equal(صحیح family, ثابت اجتماع in_addr_union *a, ثابت اجتماع in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                ثابت ساختار in_addr *a,
                unsigned aprefixlen,
                ثابت ساختار in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                ثابت ساختار in6_addr *a,
                unsigned aprefixlen,
                ثابت ساختار in6_addr *b,
                unsigned bprefixlen) _pure_;
صحیح in_addr_prefix_intersect(
                صحیح family,
                ثابت اجتماع in_addr_union *a,
                unsigned aprefixlen,
                ثابت اجتماع in_addr_union *b,
                unsigned bprefixlen) _pure_;
صحیح in_addr_random_prefix(صحیح family, اجتماع in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
صحیح in_addr_prefix_range(
                صحیح family,
                ثابت اجتماع in_addr_union *in,
                unsigned prefixlen,
                اجتماع in_addr_union *ret_start,
                اجتماع in_addr_union *ret_end);

صحیح in_addr_to_string(صحیح family, ثابت اجتماع in_addr_union *u, نویسه **ret);
ایستا درونخطی صحیح in6_addr_to_string(ثابت ساختار in6_addr *u, نویسه **ret) {
        بازگشت in_addr_to_string(AF_INET6, (ثابت اجتماع in_addr_union*) u, ret);
}

ثابت نویسه* typesafe_inet_ntop(صحیح family, ثابت اجتماع in_addr_union *a, نویسه *buf, size_t len);
ثابت نویسه* typesafe_inet_ntop4(ثابت ساختار in_addr *a, نویسه *buf, size_t len);
ثابت نویسه* typesafe_inet_ntop6(ثابت ساختار in6_addr *a, نویسه *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (نویسه[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (نویسه[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (نویسه[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

صحیح in_addr_prefix_to_string(
                صحیح family,
                ثابت اجتماع in_addr_union *u,
                unsigned prefixlen,
                نویسه *buf,
                size_t buf_len);

ایستا درونخطی ثابت نویسه* _in_addr_prefix_to_string(
                صحیح family,
                ثابت اجتماع in_addr_union *u,
                unsigned prefixlen,
                نویسه *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        بازگشت buf;
}
ایستا درونخطی ثابت نویسه* _in4_addr_prefix_to_string(ثابت ساختار in_addr *a, unsigned prefixlen, نویسه *buf, size_t buf_len) {
        بازگشت _in_addr_prefix_to_string(AF_INET, (ثابت اجتماع in_addr_union *) a, prefixlen, buf, buf_len);
}
ایستا درونخطی ثابت نویسه* _in6_addr_prefix_to_string(ثابت ساختار in6_addr *a, unsigned prefixlen, نویسه *buf, size_t buf_len) {
        بازگشت _in_addr_prefix_to_string(AF_INET6, (ثابت اجتماع in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (نویسه[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (نویسه[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (نویسه[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

صحیح in_addr_port_ifindex_name_to_string(صحیح family, ثابت اجتماع in_addr_union *u, uint16_t port, صحیح ifindex, ثابت نویسه *server_name, نویسه **ret);
ایستا درونخطی صحیح in_addr_ifindex_to_string(صحیح family, ثابت اجتماع in_addr_union *u, صحیح ifindex, نویسه **ret) {
        بازگشت in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, پوچ, ret);
}
ایستا درونخطی صحیح in_addr_port_to_string(صحیح family, ثابت اجتماع in_addr_union *u, uint16_t port, نویسه **ret) {
        بازگشت in_addr_port_ifindex_name_to_string(family, u, port, 0, پوچ, ret);
}
صحیح in_addr_from_string(صحیح family, ثابت نویسه *s, اجتماع in_addr_union *ret);
صحیح in_addr_from_string_auto(ثابت نویسه *s, صحیح *ret_family, اجتماع in_addr_union *ret);

unsigned نویسه in4_addr_netmask_to_prefixlen(ثابت ساختار in_addr *addr);
ساختار in_addr* in4_addr_prefixlen_to_netmask(ساختار in_addr *addr, unsigned نویسه prefixlen);
ساختار in6_addr* in6_addr_prefixlen_to_netmask(ساختار in6_addr *addr, unsigned نویسه prefixlen);
صحیح in_addr_prefixlen_to_netmask(صحیح family, اجتماع in_addr_union *addr, unsigned نویسه prefixlen);
صحیح in4_addr_default_prefixlen(ثابت ساختار in_addr *addr, unsigned نویسه *prefixlen);
صحیح in4_addr_default_subnet_mask(ثابت ساختار in_addr *addr, ساختار in_addr *mask);
صحیح in4_addr_mask(ساختار in_addr *addr, unsigned نویسه prefixlen);
صحیح in6_addr_mask(ساختار in6_addr *addr, unsigned نویسه prefixlen);
صحیح in_addr_mask(صحیح family, اجتماع in_addr_union *addr, unsigned نویسه prefixlen);
صحیح in4_addr_prefix_covers_full(ثابت ساختار in_addr *prefix, unsigned نویسه prefixlen, ثابت ساختار in_addr *address, unsigned نویسه address_prefixlen);
صحیح in6_addr_prefix_covers_full(ثابت ساختار in6_addr *prefix, unsigned نویسه prefixlen, ثابت ساختار in6_addr *address, unsigned نویسه address_prefixlen);
صحیح in_addr_prefix_covers_full(صحیح family, ثابت اجتماع in_addr_union *prefix, unsigned نویسه prefixlen, ثابت اجتماع in_addr_union *address, unsigned نویسه address_prefixlen);
ایستا درونخطی صحیح in4_addr_prefix_covers(ثابت ساختار in_addr *prefix, unsigned نویسه prefixlen, ثابت ساختار in_addr *address) {
        بازگشت in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
ایستا درونخطی صحیح in6_addr_prefix_covers(ثابت ساختار in6_addr *prefix, unsigned نویسه prefixlen, ثابت ساختار in6_addr *address) {
        بازگشت in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
ایستا درونخطی صحیح in_addr_prefix_covers(صحیح family, ثابت اجتماع in_addr_union *prefix, unsigned نویسه prefixlen, ثابت اجتماع in_addr_union *address) {
        بازگشت in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
صحیح in_addr_parse_prefixlen(صحیح family, ثابت نویسه *p, unsigned نویسه *ret);
صحیح in_addr_prefix_from_string(ثابت نویسه *p, صحیح family, اجتماع in_addr_union *ret_prefix, unsigned نویسه *ret_prefixlen);

تعریفنوع شمارش InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

صحیح in_addr_prefix_from_string_auto_full(ثابت نویسه *p, InAddrPrefixLenMode mode, صحیح *ret_family, اجتماع in_addr_union *ret_prefix, unsigned نویسه *ret_prefixlen);
ایستا درونخطی صحیح in_addr_prefix_from_string_auto(ثابت نویسه *p, صحیح *ret_family, اجتماع in_addr_union *ret_prefix, unsigned نویسه *ret_prefixlen) {
        بازگشت in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

ایستا درونخطی size_t FAMILY_ADDRESS_SIZE(صحیح family) {
        انتخاب (family) {
                حالت AF_INET:
                        بازگشت 4;
                حالت AF_INET6:
                        بازگشت 16;
                پیشفرض:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                صحیح _f = (f);                                           \
                _f == AF_INET ? اندازه(ساختار in_addr) :                \
                _f == AF_INET6 ? اندازه(ساختار in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((اجتماع in_addr_union) { .in6 = {} })

تهی in_addr_hash_func(ثابت اجتماع in_addr_union *u, صحیح family, ساختار siphash *state);
تهی in_addr_data_hash_func(ثابت ساختار in_addr_data *a, ساختار siphash *state);
صحیح in_addr_data_compare_func(ثابت ساختار in_addr_data *x, ثابت ساختار in_addr_data *y);
تهی in6_addr_hash_func(ثابت ساختار in6_addr *addr, ساختار siphash *state);
صحیح in6_addr_compare_func(ثابت ساختار in6_addr *a, ثابت ساختار in6_addr *b);

بیرونی ثابت ساختار hash_ops in_addr_data_hash_ops;
بیرونی ثابت ساختار hash_ops in_addr_data_hash_ops_free;
بیرونی ثابت ساختار hash_ops in6_addr_hash_ops;
بیرونی ثابت ساختار hash_ops in6_addr_hash_ops_free;

ایستا درونخطی تهی PTR_TO_IN4_ADDR(ثابت تهی *p, ساختار in_addr *ret) {
        أكد(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ایستا درونخطی تهی* IN4_ADDR_TO_PTR(ثابت ساختار in_addr *a) {
        أكد(a);
        بازگشت (تهی*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
