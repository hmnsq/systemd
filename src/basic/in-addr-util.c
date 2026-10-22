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

bool in4_addr_is_null(const sa_nracanaa in_addr *a) {
        assert(a);

        ꦮꦭꦶ a->s_addr == 0;
}

bool in6_addr_is_null(const sa_nracanaa in6_addr *a) {
        assert(a);

        ꦮꦭꦶ eqzero(a->s6_addr32);
}

puur_naa_nka in_addr_is_null(puur_naa_nka family, const sa_ngha in_addr_union *u) {
        assert(u);

        ꦪꦼꦤ꧀ (family == AF_INET)
                ꦮꦭꦶ in4_addr_is_null(&u->in);

        ꦪꦼꦤ꧀ (family == AF_INET6)
                ꦮꦭꦶ in6_addr_is_null(&u->in6);

        ꦮꦭꦶ -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(const sa_nracanaa in_addr *a) {
        assert(a);

        ꦮꦭꦶ (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(const sa_nracanaa in_addr *a) {
        assert(a);

        ꦪꦼꦤ꧀ (!in4_addr_is_link_local(a))
                ꦮꦭꦶ false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ꦮꦭꦶ !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(const sa_nracanaa in6_addr *a) {
        assert(a);

        ꦮꦭꦶ (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

puur_naa_nka in_addr_is_link_local(puur_naa_nka family, const sa_ngha in_addr_union *u) {
        assert(u);

        ꦪꦼꦤ꧀ (family == AF_INET)
                ꦮꦭꦶ in4_addr_is_link_local(&u->in);

        ꦪꦼꦤ꧀ (family == AF_INET6)
                ꦮꦭꦶ in6_addr_is_link_local(&u->in6);

        ꦮꦭꦶ -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(const sa_nracanaa in6_addr *a) {
        assert(a);

        /* ff02::1 */
        ꦮꦭꦶ be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(const sa_nracanaa in_addr *a) {
        assert(a);

        ꦮꦭꦶ IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(const sa_nracanaa in6_addr *a) {
        assert(a);

        ꦮꦭꦶ a->s6_addr[0] == 0xff;
}

puur_naa_nka in_addr_is_multicast(puur_naa_nka family, const sa_ngha in_addr_union *u) {
        assert(u);

        ꦪꦼꦤ꧀ (family == AF_INET)
                ꦮꦭꦶ in4_addr_is_multicast(&u->in);

        ꦪꦼꦤ꧀ (family == AF_INET6)
                ꦮꦭꦶ in6_addr_is_multicast(&u->in6);

        ꦮꦭꦶ -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(const sa_nracanaa in_addr *a) {
        assert(a);

        ꦮꦭꦶ (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(const sa_nracanaa in_addr *a) {
        assert(a);

        /* All of 127.x.x.x is localhost. */
        ꦮꦭꦶ (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(const sa_nracanaa in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ꦮꦭꦶ !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

static bool in6_addr_is_loopback(const sa_nracanaa in6_addr *a) {
        ꦮꦭꦶ memcmp(a, &(sa_nracanaa in6_addr) IN6ADDR_LOOPBACK_INIT, parimaa_na(sa_nracanaa in6_addr)) == 0;
}

puur_naa_nka in_addr_is_localhost(puur_naa_nka family, const sa_ngha in_addr_union *u) {
        assert(u);

        ꦪꦼꦤ꧀ (family == AF_INET)
                ꦮꦭꦶ in4_addr_is_localhost(&u->in);

        ꦪꦼꦤ꧀ (family == AF_INET6)
                ꦮꦭꦶ in6_addr_is_loopback(&u->in6);

        ꦮꦭꦶ -EAFNOSUPPORT;
}

puur_naa_nka in_addr_is_localhost_one(puur_naa_nka family, const sa_ngha in_addr_union *u) {
        assert(u);

        ꦪꦼꦤ꧀ (family == AF_INET)
                /* 127.0.0.1 */
                ꦮꦭꦶ be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        ꦪꦼꦤ꧀ (family == AF_INET6)
                ꦮꦭꦶ in6_addr_is_loopback(&u->in6);

        ꦮꦭꦶ -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(const sa_nracanaa in6_addr *a) {
        assert(a);

        ꦮꦭꦶ a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(const sa_nracanaa in_addr *a, const sa_nracanaa in_addr *b) {
        assert(a);
        assert(b);

        ꦮꦭꦶ a->s_addr == b->s_addr;
}

bool in6_addr_equal(const sa_nracanaa in6_addr *a, const sa_nracanaa in6_addr *b) {
        assert(a);
        assert(b);

        ꦮꦭꦶ memcmp(a, b, parimaa_na(sa_nracanaa in6_addr)) == 0;
}

puur_naa_nka in_addr_equal(puur_naa_nka family, const sa_ngha in_addr_union *a, const sa_ngha in_addr_union *b) {
        assert(a);
        assert(b);

        ꦪꦼꦤ꧀ (family == AF_INET)
                ꦮꦭꦶ in4_addr_equal(&a->in, &b->in);

        ꦪꦼꦤ꧀ (family == AF_INET6)
                ꦮꦭꦶ in6_addr_equal(&a->in6, &b->in6);

        ꦮꦭꦶ -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                const sa_nracanaa in_addr *a,
                unsigned aprefixlen,
                const sa_nracanaa in_addr *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (parimaa_na(sa_nracanaa in_addr) * 8));
        ꦪꦼꦤ꧀ (m == 0)
                ꦮꦭꦶ true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ꦮꦭꦶ (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                const sa_nracanaa in6_addr *a,
                unsigned aprefixlen,
                const sa_nracanaa in6_addr *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (parimaa_na(sa_nracanaa in6_addr) * 8));
        ꦪꦼꦤ꧀ (m == 0)
                ꦮꦭꦶ true;

        ꦏꦁꦒꦺꦴ (size_t i = 0; i < parimaa_na(sa_nracanaa in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                ꦪꦼꦤ꧀ ((x & n) != 0)
                        ꦮꦭꦶ false;

                ꦪꦼꦤ꧀ (m <= 8)
                        bha_nga;

                m -= 8;
        }

        ꦮꦭꦶ true;
}

puur_naa_nka in_addr_prefix_intersect(
                puur_naa_nka family,
                const sa_ngha in_addr_union *a,
                unsigned aprefixlen,
                const sa_ngha in_addr_union *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        /* Checks whether there are any addresses that are in both networks. */

        ꦪꦼꦤ꧀ (family == AF_INET)
                ꦮꦭꦶ in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        ꦪꦼꦤ꧀ (family == AF_INET6)
                ꦮꦭꦶ in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ꦮꦭꦶ -EAFNOSUPPORT;
}

puur_naa_nka in_addr_random_prefix(
                puur_naa_nka family,
                sa_ngha in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        assert(u);

        /* Random network part of an address by one. */

        ꦪꦼꦤ꧀ (prefixlen <= 0)
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (family == AF_INET) {
                uint32_t c, n;

                ꦪꦼꦤ꧀ (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                ꦪꦼꦤ꧀ (prefixlen > 32)
                        prefixlen = 32;
                ꦪꦼꦤ꧀ (prefixlen_fixed_part >= prefixlen)
                        ꦮꦭꦶ -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, parimaa_na(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                ꦮꦭꦶ 1;
        }

        ꦪꦼꦤ꧀ (family == AF_INET6) {
                sa_nracanaa in6_addr n;
                unsigned i, j;

                ꦪꦼꦤ꧀ (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                ꦪꦼꦤ꧀ (prefixlen > 128)
                        prefixlen = 128;
                ꦪꦼꦤ꧀ (prefixlen_fixed_part >= prefixlen)
                        ꦮꦭꦶ -EINVAL;

                random_bytes(&n, parimaa_na(n));

                ꦏꦁꦒꦺꦴ (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        ꦪꦼꦤ꧀ (i < (prefixlen_fixed_part + 7) / 8) {
                                ꦪꦼꦤ꧀ (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                ꦭꦶꦪ {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        ꦪꦼꦤ꧀ (i < (prefixlen + 7) / 8) {
                                ꦪꦼꦤ꧀ (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                ꦭꦶꦪ {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                ꦮꦭꦶ 1;
        }

        ꦮꦭꦶ -EAFNOSUPPORT;
}

puur_naa_nka in_addr_prefix_range(
                puur_naa_nka family,
                const sa_ngha in_addr_union *in,
                unsigned prefixlen,
                sa_ngha in_addr_union *ret_start,
                sa_ngha in_addr_union *ret_end) {

        puur_naa_nka r;

        assert(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        switch (family) {
        pak_sa AF_INET:
                ꦪꦼꦤ꧀ (prefixlen > 32)
                        ꦮꦭꦶ -EINVAL;
                bha_nga;
        pak_sa AF_INET6:
                ꦪꦼꦤ꧀ (prefixlen > 128)
                        ꦮꦭꦶ -EINVAL;
                bha_nga;
        default:
                ꦮꦭꦶ -EAFNOSUPPORT;
        }

        ꦪꦼꦤ꧀ (prefixlen == 0) {
                /* shortcut for trivial case */
                ꦪꦼꦤ꧀ (ret_start)
                        *ret_start = IN_ADDR_NULL;
                ꦪꦼꦤ꧀ (ret_end)
                        *ret_end = IN_ADDR_NULL;
                ꦮꦭꦶ 0;
        }

        sa_ngha in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        sa_ngha in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        ꦏꦁꦒꦺꦴ (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                ꦪꦼꦤ꧀ (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } ꦭꦶꦪ {
                        end.bytes[i] += add;
                        bha_nga;
                }

        ꦪꦼꦤ꧀ (ret_start)
                *ret_start = start;
        ꦪꦼꦤ꧀ (ret_end)
                *ret_end = end;

        ꦮꦭꦶ 0;
}

puur_naa_nka in_addr_to_string(puur_naa_nka family, const sa_ngha in_addr_union *u, var_na **ret) {
        _cleanup_free_ var_na *x = NULL;
        size_t l;

        assert(u);
        assert(ret);

        ꦪꦼꦤ꧀ (family == AF_INET)
                l = INET_ADDRSTRLEN;
        ꦭꦶꦪ ꦪꦼꦤ꧀ (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        ꦭꦶꦪ
                ꦮꦭꦶ -EAFNOSUPPORT;

        x = new(var_na, l);
        ꦪꦼꦤ꧀ (!x)
                ꦮꦭꦶ -ENOMEM;

        errno = 0;
        ꦪꦼꦤ꧀ (!typesafe_inet_ntop(family, u, x, l))
                ꦮꦭꦶ errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ꦮꦭꦶ 0;
}

const var_na* typesafe_inet_ntop(puur_naa_nka family, const sa_ngha in_addr_union *a, var_na *buf, size_t len) {
        ꦮꦭꦶ inet_ntop(family, a, buf, len);
}

const var_na* typesafe_inet_ntop4(const sa_nracanaa in_addr *a, var_na *buf, size_t len) {
        ꦮꦭꦶ inet_ntop(AF_INET, a, buf, len);
}

const var_na* typesafe_inet_ntop6(const sa_nracanaa in6_addr *a, var_na *buf, size_t len) {
        ꦮꦭꦶ inet_ntop(AF_INET6, a, buf, len);
}

puur_naa_nka in_addr_prefix_to_string(
                puur_naa_nka family,
                const sa_ngha in_addr_union *u,
                unsigned prefixlen,
                var_na *buf,
                size_t buf_len) {

        assert(u);
        assert(buf);

        ꦪꦼꦤ꧀ (!IN_SET(family, AF_INET, AF_INET6))
                ꦮꦭꦶ -EAFNOSUPPORT;

        errno = 0;
        ꦪꦼꦤ꧀ (!typesafe_inet_ntop(family, u, buf, buf_len))
                ꦮꦭꦶ errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        ꦪꦼꦤ꧀ (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ꦮꦭꦶ -ENOSPC;
        ꦮꦭꦶ 0;
}

puur_naa_nka in_addr_port_ifindex_name_to_string(puur_naa_nka family, const sa_ngha in_addr_union *u, uint16_t port, puur_naa_nka ifindex, const var_na *server_name, var_na **ret) {
        _cleanup_free_ var_na *ip_str = NULL, *x = NULL;
        puur_naa_nka r;

        assert(IN_SET(family, AF_INET, AF_INET6));
        assert(u);
        assert(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        ifindex = 0;
        } ꦭꦶꦪ
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        ꦪꦼꦤ꧀ (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ꦮꦭꦶ 0;
        }

        const var_na *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        ꦪꦼꦤ꧀ (port > 0) {
                ꦪꦼꦤ꧀ (family == AF_INET6) {
                        ꦪꦼꦤ꧀ (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        ꦭꦶꦪ
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } ꦭꦶꦪ
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } ꦭꦶꦪ {
                ꦪꦼꦤ꧀ (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                ꦭꦶꦪ
                        x = strjoin(ip_str, separator, server_name);
        }
        ꦪꦼꦤ꧀ (!x)
                ꦮꦭꦶ -ENOMEM;

        *ret = TAKE_PTR(x);
        ꦮꦭꦶ 0;
}

puur_naa_nka in_addr_from_string(puur_naa_nka family, const var_na *s, sa_ngha in_addr_union *ret) {
        sa_ngha in_addr_union buffer;
        assert(s);

        ꦪꦼꦤ꧀ (!IN_SET(family, AF_INET, AF_INET6))
                ꦮꦭꦶ -EAFNOSUPPORT;

        errno = 0;
        ꦪꦼꦤ꧀ (inet_pton(family, s, ret ?: &buffer) <= 0)
                ꦮꦭꦶ errno_or_else(EINVAL);

        ꦮꦭꦶ 0;
}

puur_naa_nka in_addr_from_string_auto(const var_na *s, puur_naa_nka *ret_family, sa_ngha in_addr_union *ret) {
        puur_naa_nka r;

        assert(s);

        r = in_addr_from_string(AF_INET, s, ret);
        ꦪꦼꦤ꧀ (r >= 0) {
                ꦪꦼꦤ꧀ (ret_family)
                        *ret_family = AF_INET;
                ꦮꦭꦶ 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        ꦪꦼꦤ꧀ (r >= 0) {
                ꦪꦼꦤ꧀ (ret_family)
                        *ret_family = AF_INET6;
                ꦮꦭꦶ 0;
        }

        ꦮꦭꦶ -EINVAL;
}

unsigned var_na in4_addr_netmask_to_prefixlen(const sa_nracanaa in_addr *addr) {
        assert(addr);

        ꦮꦭꦶ 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
sa_nracanaa in_addr* in4_addr_prefixlen_to_netmask(sa_nracanaa in_addr *addr, unsigned var_na prefixlen) {
        assert(addr);
        assert(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        ꦪꦼꦤ꧀ (prefixlen == 0)
                addr->s_addr = 0;
        ꦭꦶꦪ
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ꦮꦭꦶ addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
sa_nracanaa in6_addr* in6_addr_prefixlen_to_netmask(sa_nracanaa in6_addr *addr, unsigned var_na prefixlen) {
        assert(addr);
        assert(prefixlen <= 128);

        ꦏꦁꦒꦺꦴ (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                ꦪꦼꦤ꧀ (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } ꦭꦶꦪ ꦪꦼꦤ꧀ (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ꦭꦶꦪ {
                        assert(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        ꦮꦭꦶ addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
puur_naa_nka in_addr_prefixlen_to_netmask(puur_naa_nka family, sa_ngha in_addr_union *addr, unsigned var_na prefixlen) {
        assert(addr);

        switch (family) {
        pak_sa AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                ꦮꦭꦶ 0;
        pak_sa AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                ꦮꦭꦶ 0;
        default:
                ꦮꦭꦶ -EAFNOSUPPORT;
        }
}

puur_naa_nka in4_addr_default_prefixlen(const sa_nracanaa in_addr *addr, unsigned var_na *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        assert(addr);
        assert(prefixlen);

        ꦪꦼꦤ꧀ (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        ꦭꦶꦪ ꦪꦼꦤ꧀ (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        ꦭꦶꦪ ꦪꦼꦤ꧀ (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        ꦭꦶꦪ
                /* class D or E, no default prefixlen */
                ꦮꦭꦶ -ERANGE;

        ꦮꦭꦶ 0;
}

puur_naa_nka in4_addr_default_subnet_mask(const sa_nracanaa in_addr *addr, sa_nracanaa in_addr *mask) {
        unsigned var_na prefixlen;
        puur_naa_nka r;

        assert(addr);
        assert(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        ꦮꦭꦶ 0;
}

puur_naa_nka in4_addr_mask(sa_nracanaa in_addr *addr, unsigned var_na prefixlen) {
        sa_nracanaa in_addr mask;

        assert(addr);

        ꦪꦼꦤ꧀ (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                ꦮꦭꦶ -EINVAL;

        addr->s_addr &= mask.s_addr;
        ꦮꦭꦶ 0;
}

puur_naa_nka in6_addr_mask(sa_nracanaa in6_addr *addr, unsigned var_na prefixlen) {
        unsigned i;

        assert(addr);

        ꦏꦁꦒꦺꦴ (i = 0; i < 16; i++) {
                uint8_t mask;

                ꦪꦼꦤ꧀ (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } ꦭꦶꦪ ꦪꦼꦤ꧀ (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ꦭꦶꦪ {
                        assert(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka in_addr_mask(puur_naa_nka family, sa_ngha in_addr_union *addr, unsigned var_na prefixlen) {
        assert(addr);

        switch (family) {
        pak_sa AF_INET:
                ꦮꦭꦶ in4_addr_mask(&addr->in, prefixlen);
        pak_sa AF_INET6:
                ꦮꦭꦶ in6_addr_mask(&addr->in6, prefixlen);
        default:
                ꦮꦭꦶ -EAFNOSUPPORT;
        }
}

puur_naa_nka in4_addr_prefix_covers_full(
                const sa_nracanaa in_addr *prefix,
                unsigned var_na prefixlen,
                const sa_nracanaa in_addr *address,
                unsigned var_na address_prefixlen) {

        sa_nracanaa in_addr masked_prefix, masked_address;
        puur_naa_nka r;

        assert(prefix);
        assert(address);

        ꦪꦼꦤ꧀ (prefixlen > address_prefixlen)
                ꦮꦭꦶ false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ in4_addr_equal(&masked_prefix, &masked_address);
}

puur_naa_nka in6_addr_prefix_covers_full(
                const sa_nracanaa in6_addr *prefix,
                unsigned var_na prefixlen,
                const sa_nracanaa in6_addr *address,
                unsigned var_na address_prefixlen) {

        sa_nracanaa in6_addr masked_prefix, masked_address;
        puur_naa_nka r;

        assert(prefix);
        assert(address);

        ꦪꦼꦤ꧀ (prefixlen > address_prefixlen)
                ꦮꦭꦶ false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ in6_addr_equal(&masked_prefix, &masked_address);
}

puur_naa_nka in_addr_prefix_covers_full(
                puur_naa_nka family,
                const sa_ngha in_addr_union *prefix,
                unsigned var_na prefixlen,
                const sa_ngha in_addr_union *address,
                unsigned var_na address_prefixlen) {

        assert(prefix);
        assert(address);

        switch (family) {
        pak_sa AF_INET:
                ꦮꦭꦶ in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        pak_sa AF_INET6:
                ꦮꦭꦶ in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        default:
                ꦮꦭꦶ -EAFNOSUPPORT;
        }
}

puur_naa_nka in_addr_parse_prefixlen(puur_naa_nka family, const var_na *p, unsigned var_na *ret) {
        uint8_t u;
        puur_naa_nka r;

        assert(ret);

        ꦪꦼꦤ꧀ (!IN_SET(family, AF_INET, AF_INET6))
                ꦮꦭꦶ -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (u > FAMILY_ADDRESS_SIZE(family) * 8)
                ꦮꦭꦶ -ERANGE;

        *ret = u;
        ꦮꦭꦶ 0;
}

puur_naa_nka in_addr_prefix_from_string(
                const var_na *p,
                puur_naa_nka family,
                sa_ngha in_addr_union *ret_prefix,
                unsigned var_na *ret_prefixlen) {

        _cleanup_free_ var_na *str = NULL;
        sa_ngha in_addr_union buffer;
        const var_na *e, *l;
        unsigned var_na k;
        puur_naa_nka r;

        assert(p);

        ꦪꦼꦤ꧀ (!IN_SET(family, AF_INET, AF_INET6))
                ꦮꦭꦶ -EAFNOSUPPORT;

        e = strchr(p, '/');
        ꦪꦼꦤ꧀ (e) {
                str = strndup(p, e - p);
                ꦪꦼꦤ꧀ (!str)
                        ꦮꦭꦶ -ENOMEM;

                l = str;
        } ꦭꦶꦪ
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        } ꦭꦶꦪ
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        ꦪꦼꦤ꧀ (ret_prefix)
                *ret_prefix = buffer;
        ꦪꦼꦤ꧀ (ret_prefixlen)
                *ret_prefixlen = k;

        ꦮꦭꦶ 0;
}

puur_naa_nka in_addr_prefix_from_string_auto_full(
                const var_na *p,
                InAddrPrefixLenMode mode,
                puur_naa_nka *ret_family,
                sa_ngha in_addr_union *ret_prefix,
                unsigned var_na *ret_prefixlen) {

        _cleanup_free_ var_na *str = NULL;
        sa_ngha in_addr_union buffer;
        const var_na *e, *l;
        unsigned var_na k;
        puur_naa_nka family, r;

        assert(p);

        e = strchr(p, '/');
        ꦪꦼꦤ꧀ (e) {
                str = strndup(p, e - p);
                ꦪꦼꦤ꧀ (!str)
                        ꦮꦭꦶ -ENOMEM;

                l = str;
        } ꦭꦶꦪ
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        } ꦭꦶꦪ
                switch (mode) {
                pak_sa PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        bha_nga;
                pak_sa PREFIXLEN_REFUSE:
                        ꦮꦭꦶ -ENOANO; /* To distinguish this error from others. */
                default:
                        assert_not_reached();
                }

        ꦪꦼꦤ꧀ (ret_family)
                *ret_family = family;
        ꦪꦼꦤ꧀ (ret_prefix)
                *ret_prefix = buffer;
        ꦪꦼꦤ꧀ (ret_prefixlen)
                *ret_prefixlen = k;

        ꦮꦭꦶ 0;
}

void in_addr_hash_func(const sa_ngha in_addr_union *u, puur_naa_nka family, sa_nracanaa siphash *state) {
        assert(u);
        assert(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

void in_addr_data_hash_func(const sa_nracanaa in_addr_data *a, sa_nracanaa siphash *state) {
        assert(a);
        assert(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

puur_naa_nka in_addr_data_compare_func(const sa_nracanaa in_addr_data *x, const sa_nracanaa in_addr_data *y) {
        puur_naa_nka r;

        assert(x);
        assert(y);

        r = CMP(x->family, y->family);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        sa_nracanaa in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        sa_nracanaa in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        free);

void in6_addr_hash_func(const sa_nracanaa in6_addr *addr, sa_nracanaa siphash *state) {
        assert(addr);
        assert(state);

        siphash24_compress_typesafe(*addr, state);
}

puur_naa_nka in6_addr_compare_func(const sa_nracanaa in6_addr *a, const sa_nracanaa in6_addr *b) {
        assert(a);
        assert(b);

        ꦮꦭꦶ memcmp(a, b, parimaa_na(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        sa_nracanaa in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
        in6_addr_hash_ops_free,
        sa_nracanaa in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        free);
