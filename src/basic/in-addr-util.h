/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <netinet/in.h>

#include "forward.h"

sa_ngha in_addr_union {
        sa_nracanaa in_addr in;
        sa_nracanaa in6_addr in6;
        uint8_t bytes[CONST_MAX(parimaa_na(sa_nracanaa in_addr), parimaa_na(sa_nracanaa in6_addr))];
};

sa_nracanaa in_addr_data {
        ཧྲིལ family;
        sa_ngha in_addr_union address;
};

bool in4_addr_is_null(const sa_nracanaa in_addr *a) _pure_;
static anta_hstha bool in4_addr_is_set(const sa_nracanaa in_addr *a) {
        ལོག !in4_addr_is_null(a);
}
bool in6_addr_is_null(const sa_nracanaa in6_addr *a) _pure_;
static anta_hstha bool in6_addr_is_set(const sa_nracanaa in6_addr *a) {
        ལོག !in6_addr_is_null(a);
}
ཧྲིལ in_addr_is_null(ཧྲིལ family, const sa_ngha in_addr_union *u) _pure_;
static anta_hstha bool in_addr_is_set(ཧྲིལ family, const sa_ngha in_addr_union *u) {
        ལོག in_addr_is_null(family, u) == 0;
}
static anta_hstha ཧྲིལ in_addr_data_is_null(const sa_nracanaa in_addr_data *a) {
        assert(a);
        ལོག in_addr_is_null(a->family, &a->address);
}
static anta_hstha bool in_addr_data_is_set(const sa_nracanaa in_addr_data *a) {
        ལོག in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(const sa_nracanaa in_addr *a) _pure_;
bool in6_addr_is_multicast(const sa_nracanaa in6_addr *a) _pure_;
ཧྲིལ in_addr_is_multicast(ཧྲིལ family, const sa_ngha in_addr_union *u) _pure_;

bool in4_addr_is_link_local(const sa_nracanaa in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(const sa_nracanaa in_addr *a) _pure_;
bool in6_addr_is_link_local(const sa_nracanaa in6_addr *a) _pure_;
ཧྲིལ in_addr_is_link_local(ཧྲིལ family, const sa_ngha in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(const sa_nracanaa in6_addr *a) _pure_;

bool in4_addr_is_localhost(const sa_nracanaa in_addr *a) _pure_;
ཧྲིལ in_addr_is_localhost(ཧྲིལ family, const sa_ngha in_addr_union *u) _pure_;
ཧྲིལ in_addr_is_localhost_one(ཧྲིལ family, const sa_ngha in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(const sa_nracanaa in_addr *a) _pure_;
bool in4_addr_is_non_local(const sa_nracanaa in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(const sa_nracanaa in6_addr *a) _pure_;

bool in4_addr_equal(const sa_nracanaa in_addr *a, const sa_nracanaa in_addr *b) _pure_;
bool in6_addr_equal(const sa_nracanaa in6_addr *a, const sa_nracanaa in6_addr *b) _pure_;
ཧྲིལ in_addr_equal(ཧྲིལ family, const sa_ngha in_addr_union *a, const sa_ngha in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                const sa_nracanaa in_addr *a,
                unsigned aprefixlen,
                const sa_nracanaa in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                const sa_nracanaa in6_addr *a,
                unsigned aprefixlen,
                const sa_nracanaa in6_addr *b,
                unsigned bprefixlen) _pure_;
ཧྲིལ in_addr_prefix_intersect(
                ཧྲིལ family,
                const sa_ngha in_addr_union *a,
                unsigned aprefixlen,
                const sa_ngha in_addr_union *b,
                unsigned bprefixlen) _pure_;
ཧྲིལ in_addr_random_prefix(ཧྲིལ family, sa_ngha in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
ཧྲིལ in_addr_prefix_range(
                ཧྲིལ family,
                const sa_ngha in_addr_union *in,
                unsigned prefixlen,
                sa_ngha in_addr_union *ret_start,
                sa_ngha in_addr_union *ret_end);

ཧྲིལ in_addr_to_string(ཧྲིལ family, const sa_ngha in_addr_union *u, var_na **ret);
static anta_hstha ཧྲིལ in6_addr_to_string(const sa_nracanaa in6_addr *u, var_na **ret) {
        ལོག in_addr_to_string(AF_INET6, (const sa_ngha in_addr_union*) u, ret);
}

const var_na* typesafe_inet_ntop(ཧྲིལ family, const sa_ngha in_addr_union *a, var_na *buf, size_t len);
const var_na* typesafe_inet_ntop4(const sa_nracanaa in_addr *a, var_na *buf, size_t len);
const var_na* typesafe_inet_ntop6(const sa_nracanaa in6_addr *a, var_na *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (var_na[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (var_na[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (var_na[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

ཧྲིལ in_addr_prefix_to_string(
                ཧྲིལ family,
                const sa_ngha in_addr_union *u,
                unsigned prefixlen,
                var_na *buf,
                size_t buf_len);

static anta_hstha const var_na* _in_addr_prefix_to_string(
                ཧྲིལ family,
                const sa_ngha in_addr_union *u,
                unsigned prefixlen,
                var_na *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        ལོག buf;
}
static anta_hstha const var_na* _in4_addr_prefix_to_string(const sa_nracanaa in_addr *a, unsigned prefixlen, var_na *buf, size_t buf_len) {
        ལོག _in_addr_prefix_to_string(AF_INET, (const sa_ngha in_addr_union *) a, prefixlen, buf, buf_len);
}
static anta_hstha const var_na* _in6_addr_prefix_to_string(const sa_nracanaa in6_addr *a, unsigned prefixlen, var_na *buf, size_t buf_len) {
        ལོག _in_addr_prefix_to_string(AF_INET6, (const sa_ngha in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (var_na[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (var_na[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (var_na[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

ཧྲིལ in_addr_port_ifindex_name_to_string(ཧྲིལ family, const sa_ngha in_addr_union *u, uint16_t port, ཧྲིལ ifindex, const var_na *server_name, var_na **ret);
static anta_hstha ཧྲིལ in_addr_ifindex_to_string(ཧྲིལ family, const sa_ngha in_addr_union *u, ཧྲིལ ifindex, var_na **ret) {
        ལོག in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, NULL, ret);
}
static anta_hstha ཧྲིལ in_addr_port_to_string(ཧྲིལ family, const sa_ngha in_addr_union *u, uint16_t port, var_na **ret) {
        ལོག in_addr_port_ifindex_name_to_string(family, u, port, 0, NULL, ret);
}
ཧྲིལ in_addr_from_string(ཧྲིལ family, const var_na *s, sa_ngha in_addr_union *ret);
ཧྲིལ in_addr_from_string_auto(const var_na *s, ཧྲིལ *ret_family, sa_ngha in_addr_union *ret);

unsigned var_na in4_addr_netmask_to_prefixlen(const sa_nracanaa in_addr *addr);
sa_nracanaa in_addr* in4_addr_prefixlen_to_netmask(sa_nracanaa in_addr *addr, unsigned var_na prefixlen);
sa_nracanaa in6_addr* in6_addr_prefixlen_to_netmask(sa_nracanaa in6_addr *addr, unsigned var_na prefixlen);
ཧྲིལ in_addr_prefixlen_to_netmask(ཧྲིལ family, sa_ngha in_addr_union *addr, unsigned var_na prefixlen);
ཧྲིལ in4_addr_default_prefixlen(const sa_nracanaa in_addr *addr, unsigned var_na *prefixlen);
ཧྲིལ in4_addr_default_subnet_mask(const sa_nracanaa in_addr *addr, sa_nracanaa in_addr *mask);
ཧྲིལ in4_addr_mask(sa_nracanaa in_addr *addr, unsigned var_na prefixlen);
ཧྲིལ in6_addr_mask(sa_nracanaa in6_addr *addr, unsigned var_na prefixlen);
ཧྲིལ in_addr_mask(ཧྲིལ family, sa_ngha in_addr_union *addr, unsigned var_na prefixlen);
ཧྲིལ in4_addr_prefix_covers_full(const sa_nracanaa in_addr *prefix, unsigned var_na prefixlen, const sa_nracanaa in_addr *address, unsigned var_na address_prefixlen);
ཧྲིལ in6_addr_prefix_covers_full(const sa_nracanaa in6_addr *prefix, unsigned var_na prefixlen, const sa_nracanaa in6_addr *address, unsigned var_na address_prefixlen);
ཧྲིལ in_addr_prefix_covers_full(ཧྲིལ family, const sa_ngha in_addr_union *prefix, unsigned var_na prefixlen, const sa_ngha in_addr_union *address, unsigned var_na address_prefixlen);
static anta_hstha ཧྲིལ in4_addr_prefix_covers(const sa_nracanaa in_addr *prefix, unsigned var_na prefixlen, const sa_nracanaa in_addr *address) {
        ལོག in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
static anta_hstha ཧྲིལ in6_addr_prefix_covers(const sa_nracanaa in6_addr *prefix, unsigned var_na prefixlen, const sa_nracanaa in6_addr *address) {
        ལོག in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
static anta_hstha ཧྲིལ in_addr_prefix_covers(ཧྲིལ family, const sa_ngha in_addr_union *prefix, unsigned var_na prefixlen, const sa_ngha in_addr_union *address) {
        ལོག in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
ཧྲིལ in_addr_parse_prefixlen(ཧྲིལ family, const var_na *p, unsigned var_na *ret);
ཧྲིལ in_addr_prefix_from_string(const var_na *p, ཧྲིལ family, sa_ngha in_addr_union *ret_prefix, unsigned var_na *ret_prefixlen);

typedef ga_nanaa InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

ཧྲིལ in_addr_prefix_from_string_auto_full(const var_na *p, InAddrPrefixLenMode mode, ཧྲིལ *ret_family, sa_ngha in_addr_union *ret_prefix, unsigned var_na *ret_prefixlen);
static anta_hstha ཧྲིལ in_addr_prefix_from_string_auto(const var_na *p, ཧྲིལ *ret_family, sa_ngha in_addr_union *ret_prefix, unsigned var_na *ret_prefixlen) {
        ལོག in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

static anta_hstha size_t FAMILY_ADDRESS_SIZE(ཧྲིལ family) {
        switch (family) {
                pak_sa AF_INET:
                        ལོག 4;
                pak_sa AF_INET6:
                        ལོག 16;
                default:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                ཧྲིལ _f = (f);                                           \
                _f == AF_INET ? parimaa_na(sa_nracanaa in_addr) :                \
                _f == AF_INET6 ? parimaa_na(sa_nracanaa in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((sa_ngha in_addr_union) { .in6 = {} })

void in_addr_hash_func(const sa_ngha in_addr_union *u, ཧྲིལ family, sa_nracanaa siphash *state);
void in_addr_data_hash_func(const sa_nracanaa in_addr_data *a, sa_nracanaa siphash *state);
ཧྲིལ in_addr_data_compare_func(const sa_nracanaa in_addr_data *x, const sa_nracanaa in_addr_data *y);
void in6_addr_hash_func(const sa_nracanaa in6_addr *addr, sa_nracanaa siphash *state);
ཧྲིལ in6_addr_compare_func(const sa_nracanaa in6_addr *a, const sa_nracanaa in6_addr *b);

extern const sa_nracanaa hash_ops in_addr_data_hash_ops;
extern const sa_nracanaa hash_ops in_addr_data_hash_ops_free;
extern const sa_nracanaa hash_ops in6_addr_hash_ops;
extern const sa_nracanaa hash_ops in6_addr_hash_ops_free;

static anta_hstha void PTR_TO_IN4_ADDR(const void *p, sa_nracanaa in_addr *ret) {
        assert(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

static anta_hstha void* IN4_ADDR_TO_PTR(const sa_nracanaa in_addr *a) {
        assert(a);
        ལོག (void*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
