/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <arpa/inet.h>
#include <endian.h>
#include <stdio.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "hash-funcs.h"
#include "in-addr-util.h"
#include "logarithm.h"
#include "memory-util.h"
#include "parse-util.h"
#include "random-util.h"
#include "siphash24.h"
#include "stdio-util.h"
#include "string-util.h"

bool in4_addr_is_null(const struct in_addr *a) {
        assert(a);

        врати a->s_addr == 0;
}

bool in6_addr_is_null(const struct in6_addr *a) {
        assert(a);

        врати eqzero(a->s6_addr32);
}

цео in_addr_is_null(цео family, const union in_addr_union *u) {
        assert(u);

        ако (family == AF_INET)
                врати in4_addr_is_null(&u->in);

        ако (family == AF_INET6)
                врати in6_addr_is_null(&u->in6);

        врати -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(const struct in_addr *a) {
        assert(a);

        врати (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(const struct in_addr *a) {
        assert(a);

        ако (!in4_addr_is_link_local(a))
                врати false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        врати !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(const struct in6_addr *a) {
        assert(a);

        врати (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

цео in_addr_is_link_local(цео family, const union in_addr_union *u) {
        assert(u);

        ако (family == AF_INET)
                врати in4_addr_is_link_local(&u->in);

        ако (family == AF_INET6)
                врати in6_addr_is_link_local(&u->in6);

        врати -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(const struct in6_addr *a) {
        assert(a);

        /* ff02::1 */
        врати be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(const struct in_addr *a) {
        assert(a);

        врати IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(const struct in6_addr *a) {
        assert(a);

        врати a->s6_addr[0] == 0xff;
}

цео in_addr_is_multicast(цео family, const union in_addr_union *u) {
        assert(u);

        ако (family == AF_INET)
                врати in4_addr_is_multicast(&u->in);

        ако (family == AF_INET6)
                врати in6_addr_is_multicast(&u->in6);

        врати -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(const struct in_addr *a) {
        assert(a);

        врати (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(const struct in_addr *a) {
        assert(a);

        /* All of 127.x.x.x is localhost. */
        врати (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(const struct in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        врати !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

static bool in6_addr_is_loopback(const struct in6_addr *a) {
        врати memcmp(a, &(struct in6_addr) IN6ADDR_LOOPBACK_INIT, sizeof(struct in6_addr)) == 0;
}

цео in_addr_is_localhost(цео family, const union in_addr_union *u) {
        assert(u);

        ако (family == AF_INET)
                врати in4_addr_is_localhost(&u->in);

        ако (family == AF_INET6)
                врати in6_addr_is_loopback(&u->in6);

        врати -EAFNOSUPPORT;
}

цео in_addr_is_localhost_one(цео family, const union in_addr_union *u) {
        assert(u);

        ако (family == AF_INET)
                /* 127.0.0.1 */
                врати be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        ако (family == AF_INET6)
                врати in6_addr_is_loopback(&u->in6);

        врати -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(const struct in6_addr *a) {
        assert(a);

        врати a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(const struct in_addr *a, const struct in_addr *b) {
        assert(a);
        assert(b);

        врати a->s_addr == b->s_addr;
}

bool in6_addr_equal(const struct in6_addr *a, const struct in6_addr *b) {
        assert(a);
        assert(b);

        врати memcmp(a, b, sizeof(struct in6_addr)) == 0;
}

цео in_addr_equal(цео family, const union in_addr_union *a, const union in_addr_union *b) {
        assert(a);
        assert(b);

        ако (family == AF_INET)
                врати in4_addr_equal(&a->in, &b->in);

        ако (family == AF_INET6)
                врати in6_addr_equal(&a->in6, &b->in6);

        врати -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                const struct in_addr *a,
                unsigned aprefixlen,
                const struct in_addr *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (sizeof(struct in_addr) * 8));
        ако (m == 0)
                врати true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        врати (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                const struct in6_addr *a,
                unsigned aprefixlen,
                const struct in6_addr *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (sizeof(struct in6_addr) * 8));
        ако (m == 0)
                врати true;

        за (size_t i = 0; i < sizeof(struct in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                ако ((x & n) != 0)
                        врати false;

                ако (m <= 8)
                        break;

                m -= 8;
        }

        врати true;
}

цео in_addr_prefix_intersect(
                цео family,
                const union in_addr_union *a,
                unsigned aprefixlen,
                const union in_addr_union *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        /* Checks whether there are any addresses that are in both networks. */

        ако (family == AF_INET)
                врати in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        ако (family == AF_INET6)
                врати in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        врати -EAFNOSUPPORT;
}

цео in_addr_random_prefix(
                цео family,
                union in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        assert(u);

        /* Random network part of an address by one. */

        ако (prefixlen <= 0)
                врати 0;

        ако (family == AF_INET) {
                uint32_t c, n;

                ако (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                ако (prefixlen > 32)
                        prefixlen = 32;
                ако (prefixlen_fixed_part >= prefixlen)
                        врати -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, sizeof(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                врати 1;
        }

        ако (family == AF_INET6) {
                struct in6_addr n;
                unsigned i, j;

                ако (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                ако (prefixlen > 128)
                        prefixlen = 128;
                ако (prefixlen_fixed_part >= prefixlen)
                        врати -EINVAL;

                random_bytes(&n, sizeof(n));

                за (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        ако (i < (prefixlen_fixed_part + 7) / 8) {
                                ако (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                инако {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        ако (i < (prefixlen + 7) / 8) {
                                ако (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                инако {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                врати 1;
        }

        врати -EAFNOSUPPORT;
}

цео in_addr_prefix_range(
                цео family,
                const union in_addr_union *in,
                unsigned prefixlen,
                union in_addr_union *ret_start,
                union in_addr_union *ret_end) {

        цео r;

        assert(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        switch (family) {
        case AF_INET:
                ако (prefixlen > 32)
                        врати -EINVAL;
                break;
        case AF_INET6:
                ако (prefixlen > 128)
                        врати -EINVAL;
                break;
        default:
                врати -EAFNOSUPPORT;
        }

        ако (prefixlen == 0) {
                /* shortcut for trivial case */
                ако (ret_start)
                        *ret_start = IN_ADDR_NULL;
                ако (ret_end)
                        *ret_end = IN_ADDR_NULL;
                врати 0;
        }

        union in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        ако (r < 0)
                врати r;

        union in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        за (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                ако (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } инако {
                        end.bytes[i] += add;
                        break;
                }

        ако (ret_start)
                *ret_start = start;
        ако (ret_end)
                *ret_end = end;

        врати 0;
}

цео in_addr_to_string(цео family, const union in_addr_union *u, char **ret) {
        _cleanup_free_ char *x = NULL;
        size_t l;

        assert(u);
        assert(ret);

        ако (family == AF_INET)
                l = INET_ADDRSTRLEN;
        инако ако (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        инако
                врати -EAFNOSUPPORT;

        x = new(char, l);
        ако (!x)
                врати -ENOMEM;

        errno = 0;
        ако (!typesafe_inet_ntop(family, u, x, l))
                врати errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        врати 0;
}

const char* typesafe_inet_ntop(цео family, const union in_addr_union *a, char *buf, size_t len) {
        врати inet_ntop(family, a, buf, len);
}

const char* typesafe_inet_ntop4(const struct in_addr *a, char *buf, size_t len) {
        врати inet_ntop(AF_INET, a, buf, len);
}

const char* typesafe_inet_ntop6(const struct in6_addr *a, char *buf, size_t len) {
        врати inet_ntop(AF_INET6, a, buf, len);
}

цео in_addr_prefix_to_string(
                цео family,
                const union in_addr_union *u,
                unsigned prefixlen,
                char *buf,
                size_t buf_len) {

        assert(u);
        assert(buf);

        ако (!IN_SET(family, AF_INET, AF_INET6))
                врати -EAFNOSUPPORT;

        errno = 0;
        ако (!typesafe_inet_ntop(family, u, buf, buf_len))
                врати errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        ако (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                врати -ENOSPC;
        врати 0;
}

цео in_addr_port_ifindex_name_to_string(цео family, const union in_addr_union *u, uint16_t port, цео ifindex, const char *server_name, char **ret) {
        _cleanup_free_ char *ip_str = NULL, *x = NULL;
        цео r;

        assert(IN_SET(family, AF_INET, AF_INET6));
        assert(u);
        assert(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        ако (r < 0)
                врати r;

        ако (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                ако (r < 0)
                        врати r;
                ако (r == 0)
                        ifindex = 0;
        } инако
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        ако (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                врати 0;
        }

        const char *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        ако (port > 0) {
                ако (family == AF_INET6) {
                        ако (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        инако
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } инако
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } инако {
                ако (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                инако
                        x = strjoin(ip_str, separator, server_name);
        }
        ако (!x)
                врати -ENOMEM;

        *ret = TAKE_PTR(x);
        врати 0;
}

цео in_addr_from_string(цео family, const char *s, union in_addr_union *ret) {
        union in_addr_union buffer;
        assert(s);

        ако (!IN_SET(family, AF_INET, AF_INET6))
                врати -EAFNOSUPPORT;

        errno = 0;
        ако (inet_pton(family, s, ret ?: &buffer) <= 0)
                врати errno_or_else(EINVAL);

        врати 0;
}

цео in_addr_from_string_auto(const char *s, цео *ret_family, union in_addr_union *ret) {
        цео r;

        assert(s);

        r = in_addr_from_string(AF_INET, s, ret);
        ако (r >= 0) {
                ако (ret_family)
                        *ret_family = AF_INET;
                врати 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        ако (r >= 0) {
                ако (ret_family)
                        *ret_family = AF_INET6;
                врати 0;
        }

        врати -EINVAL;
}

unsigned char in4_addr_netmask_to_prefixlen(const struct in_addr *addr) {
        assert(addr);

        врати 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
struct in_addr* in4_addr_prefixlen_to_netmask(struct in_addr *addr, unsigned char prefixlen) {
        assert(addr);
        assert(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        ако (prefixlen == 0)
                addr->s_addr = 0;
        инако
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        врати addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
struct in6_addr* in6_addr_prefixlen_to_netmask(struct in6_addr *addr, unsigned char prefixlen) {
        assert(addr);
        assert(prefixlen <= 128);

        за (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                ако (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } инако ако (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } инако {
                        assert(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        врати addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
цео in_addr_prefixlen_to_netmask(цео family, union in_addr_union *addr, unsigned char prefixlen) {
        assert(addr);

        switch (family) {
        case AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                врати 0;
        case AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                врати 0;
        default:
                врати -EAFNOSUPPORT;
        }
}

цео in4_addr_default_prefixlen(const struct in_addr *addr, unsigned char *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        assert(addr);
        assert(prefixlen);

        ако (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        инако ако (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        инако ако (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        инако
                /* class D or E, no default prefixlen */
                врати -ERANGE;

        врати 0;
}

цео in4_addr_default_subnet_mask(const struct in_addr *addr, struct in_addr *mask) {
        unsigned char prefixlen;
        цео r;

        assert(addr);
        assert(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        ако (r < 0)
                врати r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        врати 0;
}

цео in4_addr_mask(struct in_addr *addr, unsigned char prefixlen) {
        struct in_addr mask;

        assert(addr);

        ако (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                врати -EINVAL;

        addr->s_addr &= mask.s_addr;
        врати 0;
}

цео in6_addr_mask(struct in6_addr *addr, unsigned char prefixlen) {
        unsigned i;

        assert(addr);

        за (i = 0; i < 16; i++) {
                uint8_t mask;

                ако (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } инако ако (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } инако {
                        assert(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        врати 0;
}

цео in_addr_mask(цео family, union in_addr_union *addr, unsigned char prefixlen) {
        assert(addr);

        switch (family) {
        case AF_INET:
                врати in4_addr_mask(&addr->in, prefixlen);
        case AF_INET6:
                врати in6_addr_mask(&addr->in6, prefixlen);
        default:
                врати -EAFNOSUPPORT;
        }
}

цео in4_addr_prefix_covers_full(
                const struct in_addr *prefix,
                unsigned char prefixlen,
                const struct in_addr *address,
                unsigned char address_prefixlen) {

        struct in_addr masked_prefix, masked_address;
        цео r;

        assert(prefix);
        assert(address);

        ако (prefixlen > address_prefixlen)
                врати false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        ако (r < 0)
                врати r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        ако (r < 0)
                врати r;

        врати in4_addr_equal(&masked_prefix, &masked_address);
}

цео in6_addr_prefix_covers_full(
                const struct in6_addr *prefix,
                unsigned char prefixlen,
                const struct in6_addr *address,
                unsigned char address_prefixlen) {

        struct in6_addr masked_prefix, masked_address;
        цео r;

        assert(prefix);
        assert(address);

        ако (prefixlen > address_prefixlen)
                врати false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        ако (r < 0)
                врати r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        ако (r < 0)
                врати r;

        врати in6_addr_equal(&masked_prefix, &masked_address);
}

цео in_addr_prefix_covers_full(
                цео family,
                const union in_addr_union *prefix,
                unsigned char prefixlen,
                const union in_addr_union *address,
                unsigned char address_prefixlen) {

        assert(prefix);
        assert(address);

        switch (family) {
        case AF_INET:
                врати in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        case AF_INET6:
                врати in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        default:
                врати -EAFNOSUPPORT;
        }
}

цео in_addr_parse_prefixlen(цео family, const char *p, unsigned char *ret) {
        uint8_t u;
        цео r;

        assert(ret);

        ако (!IN_SET(family, AF_INET, AF_INET6))
                врати -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        ако (r < 0)
                врати r;

        ако (u > FAMILY_ADDRESS_SIZE(family) * 8)
                врати -ERANGE;

        *ret = u;
        врати 0;
}

цео in_addr_prefix_from_string(
                const char *p,
                цео family,
                union in_addr_union *ret_prefix,
                unsigned char *ret_prefixlen) {

        _cleanup_free_ char *str = NULL;
        union in_addr_union buffer;
        const char *e, *l;
        unsigned char k;
        цео r;

        assert(p);

        ако (!IN_SET(family, AF_INET, AF_INET6))
                врати -EAFNOSUPPORT;

        e = strchr(p, '/');
        ако (e) {
                str = strndup(p, e - p);
                ако (!str)
                        врати -ENOMEM;

                l = str;
        } инако
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        ако (r < 0)
                врати r;

        ако (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ако (r < 0)
                        врати r;
        } инако
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        ако (ret_prefix)
                *ret_prefix = buffer;
        ако (ret_prefixlen)
                *ret_prefixlen = k;

        врати 0;
}

цео in_addr_prefix_from_string_auto_full(
                const char *p,
                InAddrPrefixLenMode mode,
                цео *ret_family,
                union in_addr_union *ret_prefix,
                unsigned char *ret_prefixlen) {

        _cleanup_free_ char *str = NULL;
        union in_addr_union buffer;
        const char *e, *l;
        unsigned char k;
        цео family, r;

        assert(p);

        e = strchr(p, '/');
        ако (e) {
                str = strndup(p, e - p);
                ако (!str)
                        врати -ENOMEM;

                l = str;
        } инако
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        ако (r < 0)
                врати r;

        ако (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ако (r < 0)
                        врати r;
        } инако
                switch (mode) {
                case PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        break;
                case PREFIXLEN_REFUSE:
                        врати -ENOANO; /* To distinguish this error from others. */
                default:
                        assert_not_reached();
                }

        ако (ret_family)
                *ret_family = family;
        ако (ret_prefix)
                *ret_prefix = buffer;
        ако (ret_prefixlen)
                *ret_prefixlen = k;

        врати 0;
}

void in_addr_hash_func(const union in_addr_union *u, цео family, struct siphash *state) {
        assert(u);
        assert(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

void in_addr_data_hash_func(const struct in_addr_data *a, struct siphash *state) {
        assert(a);
        assert(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

цео in_addr_data_compare_func(const struct in_addr_data *x, const struct in_addr_data *y) {
        цео r;

        assert(x);
        assert(y);

        r = CMP(x->family, y->family);
        ако (r != 0)
                врати r;

        врати memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        struct in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        struct in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        free);

void in6_addr_hash_func(const struct in6_addr *addr, struct siphash *state) {
        assert(addr);
        assert(state);

        siphash24_compress_typesafe(*addr, state);
}

цео in6_addr_compare_func(const struct in6_addr *a, const struct in6_addr *b) {
        assert(a);
        assert(b);

        врати memcmp(a, b, sizeof(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        struct in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
        in6_addr_hash_ops_free,
        struct in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        free);
