/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <netinet/in.h>

#include "forward.h"

union in_addr_union {
        struct in_addr in;
        struct in6_addr in6;
        uint8_t bytes[CONST_MAX(sizeof(struct in_addr), sizeof(struct in6_addr))];
};

struct in_addr_data {
        ціле family;
        union in_addr_union address;
};

bool in4_addr_is_null(const struct in_addr *a) _pure_;
static inline bool in4_addr_is_set(const struct in_addr *a) {
        повернути !in4_addr_is_null(a);
}
bool in6_addr_is_null(const struct in6_addr *a) _pure_;
static inline bool in6_addr_is_set(const struct in6_addr *a) {
        повернути !in6_addr_is_null(a);
}
ціле in_addr_is_null(ціле family, const union in_addr_union *u) _pure_;
static inline bool in_addr_is_set(ціле family, const union in_addr_union *u) {
        повернути in_addr_is_null(family, u) == 0;
}
static inline ціле in_addr_data_is_null(const struct in_addr_data *a) {
        assert(a);
        повернути in_addr_is_null(a->family, &a->address);
}
static inline bool in_addr_data_is_set(const struct in_addr_data *a) {
        повернути in_addr_data_is_null(a);
}

bool in4_addr_is_multicast(const struct in_addr *a) _pure_;
bool in6_addr_is_multicast(const struct in6_addr *a) _pure_;
ціле in_addr_is_multicast(ціле family, const union in_addr_union *u) _pure_;

bool in4_addr_is_link_local(const struct in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(const struct in_addr *a) _pure_;
bool in6_addr_is_link_local(const struct in6_addr *a) _pure_;
ціле in_addr_is_link_local(ціле family, const union in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(const struct in6_addr *a) _pure_;

bool in4_addr_is_localhost(const struct in_addr *a) _pure_;
ціле in_addr_is_localhost(ціле family, const union in_addr_union *u) _pure_;
ціле in_addr_is_localhost_one(ціле family, const union in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(const struct in_addr *a) _pure_;
bool in4_addr_is_non_local(const struct in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(const struct in6_addr *a) _pure_;

bool in4_addr_equal(const struct in_addr *a, const struct in_addr *b) _pure_;
bool in6_addr_equal(const struct in6_addr *a, const struct in6_addr *b) _pure_;
ціле in_addr_equal(ціле family, const union in_addr_union *a, const union in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                const struct in_addr *a,
                unsigned aprefixlen,
                const struct in_addr *b,
                unsigned bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                const struct in6_addr *a,
                unsigned aprefixlen,
                const struct in6_addr *b,
                unsigned bprefixlen) _pure_;
ціле in_addr_prefix_intersect(
                ціле family,
                const union in_addr_union *a,
                unsigned aprefixlen,
                const union in_addr_union *b,
                unsigned bprefixlen) _pure_;
ціле in_addr_random_prefix(ціле family, union in_addr_union *u, unsigned prefixlen_fixed_part, unsigned prefixlen);
ціле in_addr_prefix_range(
                ціле family,
                const union in_addr_union *in,
                unsigned prefixlen,
                union in_addr_union *ret_start,
                union in_addr_union *ret_end);

ціле in_addr_to_string(ціле family, const union in_addr_union *u, char **ret);
static inline ціле in6_addr_to_string(const struct in6_addr *u, char **ret) {
        повернути in_addr_to_string(AF_INET6, (const union in_addr_union*) u, ret);
}

const char* typesafe_inet_ntop(ціле family, const union in_addr_union *a, char *buf, size_t len);
const char* typesafe_inet_ntop4(const struct in_addr *a, char *buf, size_t len);
const char* typesafe_inet_ntop6(const struct in6_addr *a, char *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#define IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (char[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#define IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (char[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#define IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (char[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

ціле in_addr_prefix_to_string(
                ціле family,
                const union in_addr_union *u,
                unsigned prefixlen,
                char *buf,
                size_t buf_len);

static inline const char* _in_addr_prefix_to_string(
                ціле family,
                const union in_addr_union *u,
                unsigned prefixlen,
                char *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        повернути buf;
}
static inline const char* _in4_addr_prefix_to_string(const struct in_addr *a, unsigned prefixlen, char *buf, size_t buf_len) {
        повернути _in_addr_prefix_to_string(AF_INET, (const union in_addr_union *) a, prefixlen, buf, buf_len);
}
static inline const char* _in6_addr_prefix_to_string(const struct in6_addr *a, unsigned prefixlen, char *buf, size_t buf_len) {
        повернути _in_addr_prefix_to_string(AF_INET6, (const union in_addr_union *) a, prefixlen, buf, buf_len);
}

#define PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(unsigned))
#define IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (char[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#define IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (char[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#define IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (char[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

ціле in_addr_port_ifindex_name_to_string(ціле family, const union in_addr_union *u, uint16_t port, ціле ifindex, const char *server_name, char **ret);
static inline ціле in_addr_ifindex_to_string(ціле family, const union in_addr_union *u, ціле ifindex, char **ret) {
        повернути in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, NULL, ret);
}
static inline ціле in_addr_port_to_string(ціле family, const union in_addr_union *u, uint16_t port, char **ret) {
        повернути in_addr_port_ifindex_name_to_string(family, u, port, 0, NULL, ret);
}
ціле in_addr_from_string(ціле family, const char *s, union in_addr_union *ret);
ціле in_addr_from_string_auto(const char *s, ціле *ret_family, union in_addr_union *ret);

unsigned char in4_addr_netmask_to_prefixlen(const struct in_addr *addr);
struct in_addr* in4_addr_prefixlen_to_netmask(struct in_addr *addr, unsigned char prefixlen);
struct in6_addr* in6_addr_prefixlen_to_netmask(struct in6_addr *addr, unsigned char prefixlen);
ціле in_addr_prefixlen_to_netmask(ціле family, union in_addr_union *addr, unsigned char prefixlen);
ціле in4_addr_default_prefixlen(const struct in_addr *addr, unsigned char *prefixlen);
ціле in4_addr_default_subnet_mask(const struct in_addr *addr, struct in_addr *mask);
ціле in4_addr_mask(struct in_addr *addr, unsigned char prefixlen);
ціле in6_addr_mask(struct in6_addr *addr, unsigned char prefixlen);
ціле in_addr_mask(ціле family, union in_addr_union *addr, unsigned char prefixlen);
ціле in4_addr_prefix_covers_full(const struct in_addr *prefix, unsigned char prefixlen, const struct in_addr *address, unsigned char address_prefixlen);
ціле in6_addr_prefix_covers_full(const struct in6_addr *prefix, unsigned char prefixlen, const struct in6_addr *address, unsigned char address_prefixlen);
ціле in_addr_prefix_covers_full(ціле family, const union in_addr_union *prefix, unsigned char prefixlen, const union in_addr_union *address, unsigned char address_prefixlen);
static inline ціле in4_addr_prefix_covers(const struct in_addr *prefix, unsigned char prefixlen, const struct in_addr *address) {
        повернути in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
static inline ціле in6_addr_prefix_covers(const struct in6_addr *prefix, unsigned char prefixlen, const struct in6_addr *address) {
        повернути in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
static inline ціле in_addr_prefix_covers(ціле family, const union in_addr_union *prefix, unsigned char prefixlen, const union in_addr_union *address) {
        повернути in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
ціле in_addr_parse_prefixlen(ціле family, const char *p, unsigned char *ret);
ціле in_addr_prefix_from_string(const char *p, ціле family, union in_addr_union *ret_prefix, unsigned char *ret_prefixlen);

typedef enum InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

ціле in_addr_prefix_from_string_auto_full(const char *p, InAddrPrefixLenMode mode, ціле *ret_family, union in_addr_union *ret_prefix, unsigned char *ret_prefixlen);
static inline ціле in_addr_prefix_from_string_auto(const char *p, ціле *ret_family, union in_addr_union *ret_prefix, unsigned char *ret_prefixlen) {
        повернути in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

static inline size_t FAMILY_ADDRESS_SIZE(ціле family) {
        switch (family) {
                case AF_INET:
                        повернути 4;
                case AF_INET6:
                        повернути 16;
                default:
                        assert_not_reached();
        }
}

#define FAMILY_ADDRESS_SIZE_SAFE(f)                                     \
        ({                                                              \
                ціле _f = (f);                                           \
                _f == AF_INET ? sizeof(struct in_addr) :                \
                _f == AF_INET6 ? sizeof(struct in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#define IN_ADDR_NULL ((union in_addr_union) { .in6 = {} })

void in_addr_hash_func(const union in_addr_union *u, ціле family, struct siphash *state);
void in_addr_data_hash_func(const struct in_addr_data *a, struct siphash *state);
ціле in_addr_data_compare_func(const struct in_addr_data *x, const struct in_addr_data *y);
void in6_addr_hash_func(const struct in6_addr *addr, struct siphash *state);
ціле in6_addr_compare_func(const struct in6_addr *a, const struct in6_addr *b);

extern const struct hash_ops in_addr_data_hash_ops;
extern const struct hash_ops in_addr_data_hash_ops_free;
extern const struct hash_ops in6_addr_hash_ops;
extern const struct hash_ops in6_addr_hash_ops_free;

static inline void PTR_TO_IN4_ADDR(const void *p, struct in_addr *ret) {
        assert(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

static inline void* IN4_ADDR_TO_PTR(const struct in_addr *a) {
        assert(a);
        повернути (void*) ((uintptr_t) a->s_addr);
}

#define IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#define IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
