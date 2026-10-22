/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <netinet/in.h>

#अन्तर्भाव "forward.h"

संघ in_addr_union {
        संरचना in_addr in;
        संरचना in6_addr in6;
        uint8_t bytes[CONST_MAX(आकार(संरचना in_addr), आकार(संरचना in6_addr))];
};

संरचना in_addr_data {
        पूर्णांक family;
        संघ in_addr_union address;
};

bool in4_addr_is_null(स्थिरांक संरचना in_addr *a) _pure_;
स्थिर ओळीत bool in4_addr_is_set(स्थिरांक संरचना in_addr *a) {
        परत !in4_addr_is_null(a);
}
bool in6_addr_is_null(स्थिरांक संरचना in6_addr *a) _pure_;
स्थिर ओळीत bool in6_addr_is_set(स्थिरांक संरचना in6_addr *a) {
        परत !in6_addr_is_null(a);
}
पूर्णांक in_addr_is_null(पूर्णांक family, स्थिरांक संघ in_addr_union *u) _pure_;
स्थिर ओळीत bool in_addr_is_set(पूर्णांक family, स्थिरांक संघ in_addr_union *u) {
        परत in_addr_is_null(family, u) == 0;
}
स्थिर ओळीत पूर्णांक in_addr_data_is_null(स्थिरांक संरचना in_addr_data *a) {
        प्रतिज्ञा(a);
        परत in_addr_is_null(a->family, &a->address);
}
स्थिर ओळीत bool in_addr_data_is_set(स्थिरांक संरचना in_addr_data *a) {
        परत in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(स्थिरांक संरचना in_addr *a) _pure_;
bool in6_addr_is_multicast(स्थिरांक संरचना in6_addr *a) _pure_;
पूर्णांक in_addr_is_multicast(पूर्णांक family, स्थिरांक संघ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(स्थिरांक संरचना in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(स्थिरांक संरचना in_addr *a) _pure_;
bool in6_addr_is_link_local(स्थिरांक संरचना in6_addr *a) _pure_;
पूर्णांक in_addr_is_link_local(पूर्णांक family, स्थिरांक संघ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(स्थिरांक संरचना in6_addr *a) _pure_;

bool in4_addr_is_localhost(स्थिरांक संरचना in_addr *a) _pure_;
पूर्णांक in_addr_is_localhost(पूर्णांक family, स्थिरांक संघ in_addr_union *u) _pure_;
पूर्णांक in_addr_is_localhost_one(पूर्णांक family, स्थिरांक संघ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(स्थिरांक संरचना in_addr *a) _pure_;
bool in4_addr_is_non_local(स्थिरांक संरचना in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(स्थिरांक संरचना in6_addr *a) _pure_;

bool in4_addr_equal(स्थिरांक संरचना in_addr *a, स्थिरांक संरचना in_addr *b) _pure_;
bool in6_addr_equal(स्थिरांक संरचना in6_addr *a, स्थिरांक संरचना in6_addr *b) _pure_;
पूर्णांक in_addr_equal(पूर्णांक family, स्थिरांक संघ in_addr_union *a, स्थिरांक संघ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                स्थिरांक संरचना in_addr *a,
                अचिन्हित aprefixlen,
                स्थिरांक संरचना in_addr *b,
                अचिन्हित bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                स्थिरांक संरचना in6_addr *a,
                अचिन्हित aprefixlen,
                स्थिरांक संरचना in6_addr *b,
                अचिन्हित bprefixlen) _pure_;
पूर्णांक in_addr_prefix_intersect(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *a,
                अचिन्हित aprefixlen,
                स्थिरांक संघ in_addr_union *b,
                अचिन्हित bprefixlen) _pure_;
पूर्णांक in_addr_random_prefix(पूर्णांक family, संघ in_addr_union *u, अचिन्हित prefixlen_fixed_part, अचिन्हित prefixlen);
पूर्णांक in_addr_prefix_range(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *in,
                अचिन्हित prefixlen,
                संघ in_addr_union *ret_start,
                संघ in_addr_union *ret_end);

पूर्णांक in_addr_to_string(पूर्णांक family, स्थिरांक संघ in_addr_union *u, अक्षर **ret);
स्थिर ओळीत पूर्णांक in6_addr_to_string(स्थिरांक संरचना in6_addr *u, अक्षर **ret) {
        परत in_addr_to_string(AF_INET6, (स्थिरांक संघ in_addr_union*) u, ret);
}

स्थिरांक अक्षर* typesafe_inet_ntop(पूर्णांक family, स्थिरांक संघ in_addr_union *a, अक्षर *buf, माप_प्रकार len);
स्थिरांक अक्षर* typesafe_inet_ntop4(स्थिरांक संरचना in_addr *a, अक्षर *buf, माप_प्रकार len);
स्थिरांक अक्षर* typesafe_inet_ntop6(स्थिरांक संरचना in6_addr *a, अक्षर *buf, माप_प्रकार len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (अक्षर[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (अक्षर[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (अक्षर[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

पूर्णांक in_addr_prefix_to_string(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *u,
                अचिन्हित prefixlen,
                अक्षर *buf,
                माप_प्रकार buf_len);

स्थिर ओळीत स्थिरांक अक्षर* _in_addr_prefix_to_string(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *u,
                अचिन्हित prefixlen,
                अक्षर *buf,
                माप_प्रकार buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        परत buf;
}
स्थिर ओळीत स्थिरांक अक्षर* _in4_addr_prefix_to_string(स्थिरांक संरचना in_addr *a, अचिन्हित prefixlen, अक्षर *buf, माप_प्रकार buf_len) {
        परत _in_addr_prefix_to_string(AF_INET, (स्थिरांक संघ in_addr_union *) a, prefixlen, buf, buf_len);
}
स्थिर ओळीत स्थिरांक अक्षर* _in6_addr_prefix_to_string(स्थिरांक संरचना in6_addr *a, अचिन्हित prefixlen, अक्षर *buf, माप_प्रकार buf_len) {
        परत _in_addr_prefix_to_string(AF_INET6, (स्थिरांक संघ in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(अचिन्हित))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (अक्षर[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (अक्षर[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (अक्षर[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

पूर्णांक in_addr_port_ifindex_name_to_string(पूर्णांक family, स्थिरांक संघ in_addr_union *u, uint16_t port, पूर्णांक ifindex, स्थिरांक अक्षर *server_name, अक्षर **ret);
स्थिर ओळीत पूर्णांक in_addr_ifindex_to_string(पूर्णांक family, स्थिरांक संघ in_addr_union *u, पूर्णांक ifindex, अक्षर **ret) {
        परत in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, रिक्त, ret);
}
स्थिर ओळीत पूर्णांक in_addr_port_to_string(पूर्णांक family, स्थिरांक संघ in_addr_union *u, uint16_t port, अक्षर **ret) {
        परत in_addr_port_ifindex_name_to_string(family, u, port, 0, रिक्त, ret);
}
पूर्णांक in_addr_from_string(पूर्णांक family, स्थिरांक अक्षर *s, संघ in_addr_union *ret);
पूर्णांक in_addr_from_string_auto(स्थिरांक अक्षर *s, पूर्णांक *ret_family, संघ in_addr_union *ret);

अचिन्हित अक्षर in4_addr_netmask_to_prefixlen(स्थिरांक संरचना in_addr *addr);
संरचना in_addr* in4_addr_prefixlen_to_netmask(संरचना in_addr *addr, अचिन्हित अक्षर prefixlen);
संरचना in6_addr* in6_addr_prefixlen_to_netmask(संरचना in6_addr *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in_addr_prefixlen_to_netmask(पूर्णांक family, संघ in_addr_union *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in4_addr_default_prefixlen(स्थिरांक संरचना in_addr *addr, अचिन्हित अक्षर *prefixlen);
पूर्णांक in4_addr_default_subnet_mask(स्थिरांक संरचना in_addr *addr, संरचना in_addr *mask);
पूर्णांक in4_addr_mask(संरचना in_addr *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in6_addr_mask(संरचना in6_addr *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in_addr_mask(पूर्णांक family, संघ in_addr_union *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in4_addr_prefix_covers_full(स्थिरांक संरचना in_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिरांक संरचना in_addr *address, अचिन्हित अक्षर address_prefixlen);
पूर्णांक in6_addr_prefix_covers_full(स्थिरांक संरचना in6_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिरांक संरचना in6_addr *address, अचिन्हित अक्षर address_prefixlen);
पूर्णांक in_addr_prefix_covers_full(पूर्णांक family, स्थिरांक संघ in_addr_union *prefix, अचिन्हित अक्षर prefixlen, स्थिरांक संघ in_addr_union *address, अचिन्हित अक्षर address_prefixlen);
स्थिर ओळीत पूर्णांक in4_addr_prefix_covers(स्थिरांक संरचना in_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिरांक संरचना in_addr *address) {
        परत in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
स्थिर ओळीत पूर्णांक in6_addr_prefix_covers(स्थिरांक संरचना in6_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिरांक संरचना in6_addr *address) {
        परत in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
स्थिर ओळीत पूर्णांक in_addr_prefix_covers(पूर्णांक family, स्थिरांक संघ in_addr_union *prefix, अचिन्हित अक्षर prefixlen, स्थिरांक संघ in_addr_union *address) {
        परत in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
पूर्णांक in_addr_parse_prefixlen(पूर्णांक family, स्थिरांक अक्षर *p, अचिन्हित अक्षर *ret);
पूर्णांक in_addr_prefix_from_string(स्थिरांक अक्षर *p, पूर्णांक family, संघ in_addr_union *ret_prefix, अचिन्हित अक्षर *ret_prefixlen);

प्रकारव्याख्या गणना InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

पूर्णांक in_addr_prefix_from_string_auto_full(स्थिरांक अक्षर *p, InAddrPrefixLenMode mode, पूर्णांक *ret_family, संघ in_addr_union *ret_prefix, अचिन्हित अक्षर *ret_prefixlen);
स्थिर ओळीत पूर्णांक in_addr_prefix_from_string_auto(स्थिरांक अक्षर *p, पूर्णांक *ret_family, संघ in_addr_union *ret_prefix, अचिन्हित अक्षर *ret_prefixlen) {
        परत in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

स्थिर ओळीत माप_प्रकार FAMILY_ADDRESS_SIZE(पूर्णांक family) {
        निवड (family) {
                स्थिती AF_INET:
                        परत 4;
                स्थिती AF_INET6:
                        परत 16;
                पूर्वनिर्धारित:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                पूर्णांक _f = (f);                                           \
                _f == AF_INET ? आकार(संरचना in_addr) :                \
                _f == AF_INET6 ? आकार(संरचना in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((संघ in_addr_union) { .in6 = {} })

शून्य in_addr_hash_func(स्थिरांक संघ in_addr_union *u, पूर्णांक family, संरचना siphash *state);
शून्य in_addr_data_hash_func(स्थिरांक संरचना in_addr_data *a, संरचना siphash *state);
पूर्णांक in_addr_data_compare_func(स्थिरांक संरचना in_addr_data *x, स्थिरांक संरचना in_addr_data *y);
शून्य in6_addr_hash_func(स्थिरांक संरचना in6_addr *addr, संरचना siphash *state);
पूर्णांक in6_addr_compare_func(स्थिरांक संरचना in6_addr *a, स्थिरांक संरचना in6_addr *b);

बाह्य स्थिरांक संरचना hash_ops in_addr_data_hash_ops;
बाह्य स्थिरांक संरचना hash_ops in_addr_data_hash_ops_free;
बाह्य स्थिरांक संरचना hash_ops in6_addr_hash_ops;
बाह्य स्थिरांक संरचना hash_ops in6_addr_hash_ops_free;

स्थिर ओळीत शून्य PTR_TO_IN4_ADDR(स्थिरांक शून्य *p, संरचना in_addr *ret) {
        प्रतिज्ञा(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

स्थिर ओळीत शून्य* IN4_ADDR_TO_PTR(स्थिरांक संरचना in_addr *a) {
        प्रतिज्ञा(a);
        परत (शून्य*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
