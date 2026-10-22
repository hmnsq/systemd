/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <netinet/in.h>

#അന്തര്ഭാവ "forward.h"

സംഘം in_addr_union {
        ഘടന in_addr in;
        ഘടന in6_addr in6;
        uint8_t bytes[CONST_MAX(വലുപ്പം(ഘടന in_addr), വലുപ്പം(ഘടന in6_addr))];
};

ഘടന in_addr_data {
        പൂർണ്ണസംഖ്യ family;
        സംഘം in_addr_union address;
};

bool in4_addr_is_null(സ്ഥിരം ഘടന in_addr *a) _pure_;
സ്ഥിതം വരിയിൽ bool in4_addr_is_set(സ്ഥിരം ഘടന in_addr *a) {
        തിരികെ !in4_addr_is_null(a);
}
bool in6_addr_is_null(സ്ഥിരം ഘടന in6_addr *a) _pure_;
സ്ഥിതം വരിയിൽ bool in6_addr_is_set(സ്ഥിരം ഘടന in6_addr *a) {
        തിരികെ !in6_addr_is_null(a);
}
പൂർണ്ണസംഖ്യ in_addr_is_null(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) _pure_;
സ്ഥിതം വരിയിൽ bool in_addr_is_set(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) {
        തിരികെ in_addr_is_null(family, u) == 0;
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in_addr_data_is_null(സ്ഥിരം ഘടന in_addr_data *a) {
        പ്രതിജ്ഞാ(a);
        തിരികെ in_addr_is_null(a->family, &a->address);
}
സ്ഥിതം വരിയിൽ bool in_addr_data_is_set(സ്ഥിരം ഘടന in_addr_data *a) {
        തിരികെ in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(സ്ഥിരം ഘടന in_addr *a) _pure_;
bool in6_addr_is_multicast(സ്ഥിരം ഘടന in6_addr *a) _pure_;
പൂർണ്ണസംഖ്യ in_addr_is_multicast(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) _pure_;

bool in4_addr_is_link_local(സ്ഥിരം ഘടന in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(സ്ഥിരം ഘടന in_addr *a) _pure_;
bool in6_addr_is_link_local(സ്ഥിരം ഘടന in6_addr *a) _pure_;
പൂർണ്ണസംഖ്യ in_addr_is_link_local(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(സ്ഥിരം ഘടന in6_addr *a) _pure_;

bool in4_addr_is_localhost(സ്ഥിരം ഘടന in_addr *a) _pure_;
പൂർണ്ണസംഖ്യ in_addr_is_localhost(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) _pure_;
പൂർണ്ണസംഖ്യ in_addr_is_localhost_one(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(സ്ഥിരം ഘടന in_addr *a) _pure_;
bool in4_addr_is_non_local(സ്ഥിരം ഘടന in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(സ്ഥിരം ഘടന in6_addr *a) _pure_;

bool in4_addr_equal(സ്ഥിരം ഘടന in_addr *a, സ്ഥിരം ഘടന in_addr *b) _pure_;
bool in6_addr_equal(സ്ഥിരം ഘടന in6_addr *a, സ്ഥിരം ഘടന in6_addr *b) _pure_;
പൂർണ്ണസംഖ്യ in_addr_equal(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *a, സ്ഥിരം സംഘം in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                സ്ഥിരം ഘടന in_addr *a,
                unsigned aprefixlen,
                സ്ഥിരം ഘടന in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                സ്ഥിരം ഘടന in6_addr *a,
                unsigned aprefixlen,
                സ്ഥിരം ഘടന in6_addr *b,
                unsigned bprefixlen) _pure_;
പൂർണ്ണസംഖ്യ in_addr_prefix_intersect(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *a,
                unsigned aprefixlen,
                സ്ഥിരം സംഘം in_addr_union *b,
                unsigned bprefixlen) _pure_;
പൂർണ്ണസംഖ്യ in_addr_random_prefix(പൂർണ്ണസംഖ്യ family, സംഘം in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
പൂർണ്ണസംഖ്യ in_addr_prefix_range(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *in,
                unsigned prefixlen,
                സംഘം in_addr_union *ret_start,
                സംഘം in_addr_union *ret_end);

പൂർണ്ണസംഖ്യ in_addr_to_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in6_addr_to_string(സ്ഥിരം ഘടന in6_addr *u, അക്ഷരം **ret) {
        തിരികെ in_addr_to_string(AF_INET6, (സ്ഥിരം സംഘം in_addr_union*) u, ret);
}

സ്ഥിരം അക്ഷരം* typesafe_inet_ntop(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *a, അക്ഷരം *buf, size_t len);
സ്ഥിരം അക്ഷരം* typesafe_inet_ntop4(സ്ഥിരം ഘടന in_addr *a, അക്ഷരം *buf, size_t len);
സ്ഥിരം അക്ഷരം* typesafe_inet_ntop6(സ്ഥിരം ഘടന in6_addr *a, അക്ഷരം *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (അക്ഷരം[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (അക്ഷരം[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (അക്ഷരം[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

പൂർണ്ണസംഖ്യ in_addr_prefix_to_string(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *u,
                unsigned prefixlen,
                അക്ഷരം *buf,
                size_t buf_len);

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* _in_addr_prefix_to_string(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *u,
                unsigned prefixlen,
                അക്ഷരം *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        തിരികെ buf;
}
സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* _in4_addr_prefix_to_string(സ്ഥിരം ഘടന in_addr *a, unsigned prefixlen, അക്ഷരം *buf, size_t buf_len) {
        തിരികെ _in_addr_prefix_to_string(AF_INET, (സ്ഥിരം സംഘം in_addr_union *) a, prefixlen, buf, buf_len);
}
സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* _in6_addr_prefix_to_string(സ്ഥിരം ഘടന in6_addr *a, unsigned prefixlen, അക്ഷരം *buf, size_t buf_len) {
        തിരികെ _in_addr_prefix_to_string(AF_INET6, (സ്ഥിരം സംഘം in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (അക്ഷരം[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (അക്ഷരം[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (അക്ഷരം[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

പൂർണ്ണസംഖ്യ in_addr_port_ifindex_name_to_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u, uint16_t port, പൂർണ്ണസംഖ്യ ifindex, സ്ഥിരം അക്ഷരം *server_name, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in_addr_ifindex_to_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u, പൂർണ്ണസംഖ്യ ifindex, അക്ഷരം **ret) {
        തിരികെ in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, ശൂന്യ, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in_addr_port_to_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u, uint16_t port, അക്ഷരം **ret) {
        തിരികെ in_addr_port_ifindex_name_to_string(family, u, port, 0, ശൂന്യ, ret);
}
പൂർണ്ണസംഖ്യ in_addr_from_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം അക്ഷരം *s, സംഘം in_addr_union *ret);
പൂർണ്ണസംഖ്യ in_addr_from_string_auto(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret_family, സംഘം in_addr_union *ret);

unsigned അക്ഷരം in4_addr_netmask_to_prefixlen(സ്ഥിരം ഘടന in_addr *addr);
ഘടന in_addr* in4_addr_prefixlen_to_netmask(ഘടന in_addr *addr, unsigned അക്ഷരം prefixlen);
ഘടന in6_addr* in6_addr_prefixlen_to_netmask(ഘടന in6_addr *addr, unsigned അക്ഷരം prefixlen);
പൂർണ്ണസംഖ്യ in_addr_prefixlen_to_netmask(പൂർണ്ണസംഖ്യ family, സംഘം in_addr_union *addr, unsigned അക്ഷരം prefixlen);
പൂർണ്ണസംഖ്യ in4_addr_default_prefixlen(സ്ഥിരം ഘടന in_addr *addr, unsigned അക്ഷരം *prefixlen);
പൂർണ്ണസംഖ്യ in4_addr_default_subnet_mask(സ്ഥിരം ഘടന in_addr *addr, ഘടന in_addr *mask);
പൂർണ്ണസംഖ്യ in4_addr_mask(ഘടന in_addr *addr, unsigned അക്ഷരം prefixlen);
പൂർണ്ണസംഖ്യ in6_addr_mask(ഘടന in6_addr *addr, unsigned അക്ഷരം prefixlen);
പൂർണ്ണസംഖ്യ in_addr_mask(പൂർണ്ണസംഖ്യ family, സംഘം in_addr_union *addr, unsigned അക്ഷരം prefixlen);
പൂർണ്ണസംഖ്യ in4_addr_prefix_covers_full(സ്ഥിരം ഘടന in_addr *prefix, unsigned അക്ഷരം prefixlen, സ്ഥിരം ഘടന in_addr *address, unsigned അക്ഷരം address_prefixlen);
പൂർണ്ണസംഖ്യ in6_addr_prefix_covers_full(സ്ഥിരം ഘടന in6_addr *prefix, unsigned അക്ഷരം prefixlen, സ്ഥിരം ഘടന in6_addr *address, unsigned അക്ഷരം address_prefixlen);
പൂർണ്ണസംഖ്യ in_addr_prefix_covers_full(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *prefix, unsigned അക്ഷരം prefixlen, സ്ഥിരം സംഘം in_addr_union *address, unsigned അക്ഷരം address_prefixlen);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in4_addr_prefix_covers(സ്ഥിരം ഘടന in_addr *prefix, unsigned അക്ഷരം prefixlen, സ്ഥിരം ഘടന in_addr *address) {
        തിരികെ in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in6_addr_prefix_covers(സ്ഥിരം ഘടന in6_addr *prefix, unsigned അക്ഷരം prefixlen, സ്ഥിരം ഘടന in6_addr *address) {
        തിരികെ in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in_addr_prefix_covers(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *prefix, unsigned അക്ഷരം prefixlen, സ്ഥിരം സംഘം in_addr_union *address) {
        തിരികെ in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
പൂർണ്ണസംഖ്യ in_addr_parse_prefixlen(പൂർണ്ണസംഖ്യ family, സ്ഥിരം അക്ഷരം *p, unsigned അക്ഷരം *ret);
പൂർണ്ണസംഖ്യ in_addr_prefix_from_string(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ family, സംഘം in_addr_union *ret_prefix, unsigned അക്ഷരം *ret_prefixlen);

തരനിർവചനം എണ്ണൽ InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

പൂർണ്ണസംഖ്യ in_addr_prefix_from_string_auto_full(സ്ഥിരം അക്ഷരം *p, InAddrPrefixLenMode mode, പൂർണ്ണസംഖ്യ *ret_family, സംഘം in_addr_union *ret_prefix, unsigned അക്ഷരം *ret_prefixlen);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ in_addr_prefix_from_string_auto(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ *ret_family, സംഘം in_addr_union *ret_prefix, unsigned അക്ഷരം *ret_prefixlen) {
        തിരികെ in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

സ്ഥിതം വരിയിൽ size_t FAMILY_ADDRESS_SIZE(പൂർണ്ണസംഖ്യ family) {
        തിരഞ്ഞെടുപ്പ് (family) {
                അവസ്ഥ AF_INET:
                        തിരികെ 4;
                അവസ്ഥ AF_INET6:
                        തിരികെ 16;
                സ്ഥിരസ്ഥിതി:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                പൂർണ്ണസംഖ്യ _f = (f);                                           \
                _f == AF_INET ? വലുപ്പം(ഘടന in_addr) :                \
                _f == AF_INET6 ? വലുപ്പം(ഘടന in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((സംഘം in_addr_union) { .in6 = {} })

ശൂന്യം in_addr_hash_func(സ്ഥിരം സംഘം in_addr_union *u, പൂർണ്ണസംഖ്യ family, ഘടന siphash *state);
ശൂന്യം in_addr_data_hash_func(സ്ഥിരം ഘടന in_addr_data *a, ഘടന siphash *state);
പൂർണ്ണസംഖ്യ in_addr_data_compare_func(സ്ഥിരം ഘടന in_addr_data *x, സ്ഥിരം ഘടന in_addr_data *y);
ശൂന്യം in6_addr_hash_func(സ്ഥിരം ഘടന in6_addr *addr, ഘടന siphash *state);
പൂർണ്ണസംഖ്യ in6_addr_compare_func(സ്ഥിരം ഘടന in6_addr *a, സ്ഥിരം ഘടന in6_addr *b);

ബാഹ്യ സ്ഥിരം ഘടന hash_ops in_addr_data_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops in_addr_data_hash_ops_free;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops in6_addr_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops in6_addr_hash_ops_free;

സ്ഥിതം വരിയിൽ ശൂന്യം PTR_TO_IN4_ADDR(സ്ഥിരം ശൂന്യം *p, ഘടന in_addr *ret) {
        പ്രതിജ്ഞാ(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* IN4_ADDR_TO_PTR(സ്ഥിരം ഘടന in_addr *a) {
        പ്രതിജ്ഞാ(a);
        തിരികെ (ശൂന്യം*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
