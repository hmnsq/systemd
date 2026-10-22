/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <netinet/in.h>

#अन्तर्भाव "forward.h"

सङ्घ in_addr_union {
        संरचना in_addr in;
        संरचना in6_addr in6;
        uint8_t bytes[CONST_MAX(परिमाण(संरचना in_addr), परिमाण(संरचना in6_addr))];
};

संरचना in_addr_data {
        पूर्णाङ्क family;
        सङ्घ in_addr_union address;
};

bool in4_addr_is_null(ध्रुव संरचना in_addr *a) _pure_;
स्थिर अन्तःस्थ bool in4_addr_is_set(ध्रुव संरचना in_addr *a) {
        प्रत्यागम !in4_addr_is_null(a);
}
bool in6_addr_is_null(ध्रुव संरचना in6_addr *a) _pure_;
स्थिर अन्तःस्थ bool in6_addr_is_set(ध्रुव संरचना in6_addr *a) {
        प्रत्यागम !in6_addr_is_null(a);
}
पूर्णाङ्क in_addr_is_null(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) _pure_;
स्थिर अन्तःस्थ bool in_addr_is_set(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) {
        प्रत्यागम in_addr_is_null(family, u) == 0;
}
स्थिर अन्तःस्थ पूर्णाङ्क in_addr_data_is_null(ध्रुव संरचना in_addr_data *a) {
        प्रतिज्ञा(a);
        प्रत्यागम in_addr_is_null(a->family, &a->address);
}
स्थिर अन्तःस्थ bool in_addr_data_is_set(ध्रुव संरचना in_addr_data *a) {
        प्रत्यागम in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(ध्रुव संरचना in_addr *a) _pure_;
bool in6_addr_is_multicast(ध्रुव संरचना in6_addr *a) _pure_;
पूर्णाङ्क in_addr_is_multicast(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(ध्रुव संरचना in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(ध्रुव संरचना in_addr *a) _pure_;
bool in6_addr_is_link_local(ध्रुव संरचना in6_addr *a) _pure_;
पूर्णाङ्क in_addr_is_link_local(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(ध्रुव संरचना in6_addr *a) _pure_;

bool in4_addr_is_localhost(ध्रुव संरचना in_addr *a) _pure_;
पूर्णाङ्क in_addr_is_localhost(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) _pure_;
पूर्णाङ्क in_addr_is_localhost_one(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(ध्रुव संरचना in_addr *a) _pure_;
bool in4_addr_is_non_local(ध्रुव संरचना in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(ध्रुव संरचना in6_addr *a) _pure_;

bool in4_addr_equal(ध्रुव संरचना in_addr *a, ध्रुव संरचना in_addr *b) _pure_;
bool in6_addr_equal(ध्रुव संरचना in6_addr *a, ध्रुव संरचना in6_addr *b) _pure_;
पूर्णाङ्क in_addr_equal(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *a, ध्रुव सङ्घ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                ध्रुव संरचना in_addr *a,
                unsigned aprefixlen,
                ध्रुव संरचना in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                ध्रुव संरचना in6_addr *a,
                unsigned aprefixlen,
                ध्रुव संरचना in6_addr *b,
                unsigned bprefixlen) _pure_;
पूर्णाङ्क in_addr_prefix_intersect(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *a,
                unsigned aprefixlen,
                ध्रुव सङ्घ in_addr_union *b,
                unsigned bprefixlen) _pure_;
पूर्णाङ्क in_addr_random_prefix(पूर्णाङ्क family, सङ्घ in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
पूर्णाङ्क in_addr_prefix_range(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *in,
                unsigned prefixlen,
                सङ्घ in_addr_union *ret_start,
                सङ्घ in_addr_union *ret_end);

पूर्णाङ्क in_addr_to_string(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क in6_addr_to_string(ध्रुव संरचना in6_addr *u, वर्ण **ret) {
        प्रत्यागम in_addr_to_string(AF_INET6, (ध्रुव सङ्घ in_addr_union*) u, ret);
}

ध्रुव वर्ण* typesafe_inet_ntop(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *a, वर्ण *buf, size_t len);
ध्रुव वर्ण* typesafe_inet_ntop4(ध्रुव संरचना in_addr *a, वर्ण *buf, size_t len);
ध्रुव वर्ण* typesafe_inet_ntop6(ध्रुव संरचना in6_addr *a, वर्ण *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (वर्ण[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (वर्ण[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (वर्ण[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

पूर्णाङ्क in_addr_prefix_to_string(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *u,
                unsigned prefixlen,
                वर्ण *buf,
                size_t buf_len);

स्थिर अन्तःस्थ ध्रुव वर्ण* _in_addr_prefix_to_string(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *u,
                unsigned prefixlen,
                वर्ण *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        प्रत्यागम buf;
}
स्थिर अन्तःस्थ ध्रुव वर्ण* _in4_addr_prefix_to_string(ध्रुव संरचना in_addr *a, unsigned prefixlen, वर्ण *buf, size_t buf_len) {
        प्रत्यागम _in_addr_prefix_to_string(AF_INET, (ध्रुव सङ्घ in_addr_union *) a, prefixlen, buf, buf_len);
}
स्थिर अन्तःस्थ ध्रुव वर्ण* _in6_addr_prefix_to_string(ध्रुव संरचना in6_addr *a, unsigned prefixlen, वर्ण *buf, size_t buf_len) {
        प्रत्यागम _in_addr_prefix_to_string(AF_INET6, (ध्रुव सङ्घ in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (वर्ण[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (वर्ण[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (वर्ण[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

पूर्णाङ्क in_addr_port_ifindex_name_to_string(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u, uint16_t port, पूर्णाङ्क ifindex, ध्रुव वर्ण *server_name, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क in_addr_ifindex_to_string(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u, पूर्णाङ्क ifindex, वर्ण **ret) {
        प्रत्यागम in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क in_addr_port_to_string(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u, uint16_t port, वर्ण **ret) {
        प्रत्यागम in_addr_port_ifindex_name_to_string(family, u, port, 0, रिक्त, ret);
}
पूर्णाङ्क in_addr_from_string(पूर्णाङ्क family, ध्रुव वर्ण *s, सङ्घ in_addr_union *ret);
पूर्णाङ्क in_addr_from_string_auto(ध्रुव वर्ण *s, पूर्णाङ्क *ret_family, सङ्घ in_addr_union *ret);

unsigned वर्ण in4_addr_netmask_to_prefixlen(ध्रुव संरचना in_addr *addr);
संरचना in_addr* in4_addr_prefixlen_to_netmask(संरचना in_addr *addr, unsigned वर्ण prefixlen);
संरचना in6_addr* in6_addr_prefixlen_to_netmask(संरचना in6_addr *addr, unsigned वर्ण prefixlen);
पूर्णाङ्क in_addr_prefixlen_to_netmask(पूर्णाङ्क family, सङ्घ in_addr_union *addr, unsigned वर्ण prefixlen);
पूर्णाङ्क in4_addr_default_prefixlen(ध्रुव संरचना in_addr *addr, unsigned वर्ण *prefixlen);
पूर्णाङ्क in4_addr_default_subnet_mask(ध्रुव संरचना in_addr *addr, संरचना in_addr *mask);
पूर्णाङ्क in4_addr_mask(संरचना in_addr *addr, unsigned वर्ण prefixlen);
पूर्णाङ्क in6_addr_mask(संरचना in6_addr *addr, unsigned वर्ण prefixlen);
पूर्णाङ्क in_addr_mask(पूर्णाङ्क family, सङ्घ in_addr_union *addr, unsigned वर्ण prefixlen);
पूर्णाङ्क in4_addr_prefix_covers_full(ध्रुव संरचना in_addr *prefix, unsigned वर्ण prefixlen, ध्रुव संरचना in_addr *address, unsigned वर्ण address_prefixlen);
पूर्णाङ्क in6_addr_prefix_covers_full(ध्रुव संरचना in6_addr *prefix, unsigned वर्ण prefixlen, ध्रुव संरचना in6_addr *address, unsigned वर्ण address_prefixlen);
पूर्णाङ्क in_addr_prefix_covers_full(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *prefix, unsigned वर्ण prefixlen, ध्रुव सङ्घ in_addr_union *address, unsigned वर्ण address_prefixlen);
स्थिर अन्तःस्थ पूर्णाङ्क in4_addr_prefix_covers(ध्रुव संरचना in_addr *prefix, unsigned वर्ण prefixlen, ध्रुव संरचना in_addr *address) {
        प्रत्यागम in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
स्थिर अन्तःस्थ पूर्णाङ्क in6_addr_prefix_covers(ध्रुव संरचना in6_addr *prefix, unsigned वर्ण prefixlen, ध्रुव संरचना in6_addr *address) {
        प्रत्यागम in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
स्थिर अन्तःस्थ पूर्णाङ्क in_addr_prefix_covers(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *prefix, unsigned वर्ण prefixlen, ध्रुव सङ्घ in_addr_union *address) {
        प्रत्यागम in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
पूर्णाङ्क in_addr_parse_prefixlen(पूर्णाङ्क family, ध्रुव वर्ण *p, unsigned वर्ण *ret);
पूर्णाङ्क in_addr_prefix_from_string(ध्रुव वर्ण *p, पूर्णाङ्क family, सङ्घ in_addr_union *ret_prefix, unsigned वर्ण *ret_prefixlen);

प्रकारनिर्देश गणना InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

पूर्णाङ्क in_addr_prefix_from_string_auto_full(ध्रुव वर्ण *p, InAddrPrefixLenMode mode, पूर्णाङ्क *ret_family, सङ्घ in_addr_union *ret_prefix, unsigned वर्ण *ret_prefixlen);
स्थिर अन्तःस्थ पूर्णाङ्क in_addr_prefix_from_string_auto(ध्रुव वर्ण *p, पूर्णाङ्क *ret_family, सङ्घ in_addr_union *ret_prefix, unsigned वर्ण *ret_prefixlen) {
        प्रत्यागम in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

स्थिर अन्तःस्थ size_t FAMILY_ADDRESS_SIZE(पूर्णाङ्क family) {
        विकल्प (family) {
                पक्ष AF_INET:
                        प्रत्यागम 4;
                पक्ष AF_INET6:
                        प्रत्यागम 16;
                स्वभाव:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                पूर्णाङ्क _f = (f);                                           \
                _f == AF_INET ? परिमाण(संरचना in_addr) :                \
                _f == AF_INET6 ? परिमाण(संरचना in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((सङ्घ in_addr_union) { .in6 = {} })

शून्य in_addr_hash_func(ध्रुव सङ्घ in_addr_union *u, पूर्णाङ्क family, संरचना siphash *state);
शून्य in_addr_data_hash_func(ध्रुव संरचना in_addr_data *a, संरचना siphash *state);
पूर्णाङ्क in_addr_data_compare_func(ध्रुव संरचना in_addr_data *x, ध्रुव संरचना in_addr_data *y);
शून्य in6_addr_hash_func(ध्रुव संरचना in6_addr *addr, संरचना siphash *state);
पूर्णाङ्क in6_addr_compare_func(ध्रुव संरचना in6_addr *a, ध्रुव संरचना in6_addr *b);

बाह्य ध्रुव संरचना hash_ops in_addr_data_hash_ops;
बाह्य ध्रुव संरचना hash_ops in_addr_data_hash_ops_free;
बाह्य ध्रुव संरचना hash_ops in6_addr_hash_ops;
बाह्य ध्रुव संरचना hash_ops in6_addr_hash_ops_free;

स्थिर अन्तःस्थ शून्य PTR_TO_IN4_ADDR(ध्रुव शून्य *p, संरचना in_addr *ret) {
        प्रतिज्ञा(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

स्थिर अन्तःस्थ शून्य* IN4_ADDR_TO_PTR(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);
        प्रत्यागम (शून्य*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
