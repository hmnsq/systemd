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

        ལོག a->s_addr == 0;
}

bool in6_addr_is_null(const sa_nracanaa in6_addr *a) {
        assert(a);

        ལོག eqzero(a->s6_addr32);
}

ཧྲིལ in_addr_is_null(ཧྲིལ family, const sa_ngha in_addr_union *u) {
        assert(u);

        གལ་སྲིད (family == AF_INET)
                ལོག in4_addr_is_null(&u->in);

        གལ་སྲིད (family == AF_INET6)
                ལོག in6_addr_is_null(&u->in6);

        ལོག -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(const sa_nracanaa in_addr *a) {
        assert(a);

        ལོག (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(const sa_nracanaa in_addr *a) {
        assert(a);

        གལ་སྲིད (!in4_addr_is_link_local(a))
                ལོག false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ལོག !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(const sa_nracanaa in6_addr *a) {
        assert(a);

        ལོག (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

ཧྲིལ in_addr_is_link_local(ཧྲིལ family, const sa_ngha in_addr_union *u) {
        assert(u);

        གལ་སྲིད (family == AF_INET)
                ལོག in4_addr_is_link_local(&u->in);

        གལ་སྲིད (family == AF_INET6)
                ལོག in6_addr_is_link_local(&u->in6);

        ལོག -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(const sa_nracanaa in6_addr *a) {
        assert(a);

        /* ff02::1 */
        ལོག be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(const sa_nracanaa in_addr *a) {
        assert(a);

        ལོག IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(const sa_nracanaa in6_addr *a) {
        assert(a);

        ལོག a->s6_addr[0] == 0xff;
}

ཧྲིལ in_addr_is_multicast(ཧྲིལ family, const sa_ngha in_addr_union *u) {
        assert(u);

        གལ་སྲིད (family == AF_INET)
                ལོག in4_addr_is_multicast(&u->in);

        གལ་སྲིད (family == AF_INET6)
                ལོག in6_addr_is_multicast(&u->in6);

        ལོག -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(const sa_nracanaa in_addr *a) {
        assert(a);

        ལོག (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(const sa_nracanaa in_addr *a) {
        assert(a);

        /* All of 127.x.x.x is localhost. */
        ལོག (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(const sa_nracanaa in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ལོག !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

static bool in6_addr_is_loopback(const sa_nracanaa in6_addr *a) {
        ལོག memcmp(a, &(sa_nracanaa in6_addr) IN6ADDR_LOOPBACK_INIT, parimaa_na(sa_nracanaa in6_addr)) == 0;
}

ཧྲིལ in_addr_is_localhost(ཧྲིལ family, const sa_ngha in_addr_union *u) {
        assert(u);

        གལ་སྲིད (family == AF_INET)
                ལོག in4_addr_is_localhost(&u->in);

        གལ་སྲིད (family == AF_INET6)
                ལོག in6_addr_is_loopback(&u->in6);

        ལོག -EAFNOSUPPORT;
}

ཧྲིལ in_addr_is_localhost_one(ཧྲིལ family, const sa_ngha in_addr_union *u) {
        assert(u);

        གལ་སྲིད (family == AF_INET)
                /* 127.0.0.1 */
                ལོག be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        གལ་སྲིད (family == AF_INET6)
                ལོག in6_addr_is_loopback(&u->in6);

        ལོག -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(const sa_nracanaa in6_addr *a) {
        assert(a);

        ལོག a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(const sa_nracanaa in_addr *a, const sa_nracanaa in_addr *b) {
        assert(a);
        assert(b);

        ལོག a->s_addr == b->s_addr;
}

bool in6_addr_equal(const sa_nracanaa in6_addr *a, const sa_nracanaa in6_addr *b) {
        assert(a);
        assert(b);

        ལོག memcmp(a, b, parimaa_na(sa_nracanaa in6_addr)) == 0;
}

ཧྲིལ in_addr_equal(ཧྲིལ family, const sa_ngha in_addr_union *a, const sa_ngha in_addr_union *b) {
        assert(a);
        assert(b);

        གལ་སྲིད (family == AF_INET)
                ལོག in4_addr_equal(&a->in, &b->in);

        གལ་སྲིད (family == AF_INET6)
                ལོག in6_addr_equal(&a->in6, &b->in6);

        ལོག -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                const sa_nracanaa in_addr *a,
                unsigned aprefixlen,
                const sa_nracanaa in_addr *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (parimaa_na(sa_nracanaa in_addr) * 8));
        གལ་སྲིད (m == 0)
                ལོག true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ལོག (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                const sa_nracanaa in6_addr *a,
                unsigned aprefixlen,
                const sa_nracanaa in6_addr *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (parimaa_na(sa_nracanaa in6_addr) * 8));
        གལ་སྲིད (m == 0)
                ལོག true;

        དོན་ལུ (size_t i = 0; i < parimaa_na(sa_nracanaa in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                གལ་སྲིད ((x & n) != 0)
                        ལོག false;

                གལ་སྲིད (m <= 8)
                        bha_nga;

                m -= 8;
        }

        ལོག true;
}

ཧྲིལ in_addr_prefix_intersect(
                ཧྲིལ family,
                const sa_ngha in_addr_union *a,
                unsigned aprefixlen,
                const sa_ngha in_addr_union *b,
                unsigned bprefixlen) {

        assert(a);
        assert(b);

        /* Checks whether there are any addresses that are in both networks. */

        གལ་སྲིད (family == AF_INET)
                ལོག in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        གལ་སྲིད (family == AF_INET6)
                ལོག in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ལོག -EAFNOSUPPORT;
}

ཧྲིལ in_addr_random_prefix(
                ཧྲིལ family,
                sa_ngha in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        assert(u);

        /* Random network part of an address by one. */

        གལ་སྲིད (prefixlen <= 0)
                ལོག 0;

        གལ་སྲིད (family == AF_INET) {
                uint32_t c, n;

                གལ་སྲིད (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                གལ་སྲིད (prefixlen > 32)
                        prefixlen = 32;
                གལ་སྲིད (prefixlen_fixed_part >= prefixlen)
                        ལོག -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, parimaa_na(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                ལོག 1;
        }

        གལ་སྲིད (family == AF_INET6) {
                sa_nracanaa in6_addr n;
                unsigned i, j;

                གལ་སྲིད (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                གལ་སྲིད (prefixlen > 128)
                        prefixlen = 128;
                གལ་སྲིད (prefixlen_fixed_part >= prefixlen)
                        ལོག -EINVAL;

                random_bytes(&n, parimaa_na(n));

                དོན་ལུ (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        གལ་སྲིད (i < (prefixlen_fixed_part + 7) / 8) {
                                གལ་སྲིད (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                དེ་མིན {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        གལ་སྲིད (i < (prefixlen + 7) / 8) {
                                གལ་སྲིད (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                དེ་མིན {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                ལོག 1;
        }

        ལོག -EAFNOSUPPORT;
}

ཧྲིལ in_addr_prefix_range(
                ཧྲིལ family,
                const sa_ngha in_addr_union *in,
                unsigned prefixlen,
                sa_ngha in_addr_union *ret_start,
                sa_ngha in_addr_union *ret_end) {

        ཧྲིལ r;

        assert(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        switch (family) {
        pak_sa AF_INET:
                གལ་སྲིད (prefixlen > 32)
                        ལོག -EINVAL;
                bha_nga;
        pak_sa AF_INET6:
                གལ་སྲིད (prefixlen > 128)
                        ལོག -EINVAL;
                bha_nga;
        default:
                ལོག -EAFNOSUPPORT;
        }

        གལ་སྲིད (prefixlen == 0) {
                /* shortcut for trivial case */
                གལ་སྲིད (ret_start)
                        *ret_start = IN_ADDR_NULL;
                གལ་སྲིད (ret_end)
                        *ret_end = IN_ADDR_NULL;
                ལོག 0;
        }

        sa_ngha in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        གལ་སྲིད (r < 0)
                ལོག r;

        sa_ngha in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        དོན་ལུ (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                གལ་སྲིད (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } དེ་མིན {
                        end.bytes[i] += add;
                        bha_nga;
                }

        གལ་སྲིད (ret_start)
                *ret_start = start;
        གལ་སྲིད (ret_end)
                *ret_end = end;

        ལོག 0;
}

ཧྲིལ in_addr_to_string(ཧྲིལ family, const sa_ngha in_addr_union *u, var_na **ret) {
        _cleanup_free_ var_na *x = NULL;
        size_t l;

        assert(u);
        assert(ret);

        གལ་སྲིད (family == AF_INET)
                l = INET_ADDRSTRLEN;
        དེ་མིན གལ་སྲིད (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        དེ་མིན
                ལོག -EAFNOSUPPORT;

        x = new(var_na, l);
        གལ་སྲིད (!x)
                ལོག -ENOMEM;

        errno = 0;
        གལ་སྲིད (!typesafe_inet_ntop(family, u, x, l))
                ལོག errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ལོག 0;
}

const var_na* typesafe_inet_ntop(ཧྲིལ family, const sa_ngha in_addr_union *a, var_na *buf, size_t len) {
        ལོག inet_ntop(family, a, buf, len);
}

const var_na* typesafe_inet_ntop4(const sa_nracanaa in_addr *a, var_na *buf, size_t len) {
        ལོག inet_ntop(AF_INET, a, buf, len);
}

const var_na* typesafe_inet_ntop6(const sa_nracanaa in6_addr *a, var_na *buf, size_t len) {
        ལོག inet_ntop(AF_INET6, a, buf, len);
}

ཧྲིལ in_addr_prefix_to_string(
                ཧྲིལ family,
                const sa_ngha in_addr_union *u,
                unsigned prefixlen,
                var_na *buf,
                size_t buf_len) {

        assert(u);
        assert(buf);

        གལ་སྲིད (!IN_SET(family, AF_INET, AF_INET6))
                ལོག -EAFNOSUPPORT;

        errno = 0;
        གལ་སྲིད (!typesafe_inet_ntop(family, u, buf, buf_len))
                ལོག errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        གལ་སྲིད (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ལོག -ENOSPC;
        ལོག 0;
}

ཧྲིལ in_addr_port_ifindex_name_to_string(ཧྲིལ family, const sa_ngha in_addr_union *u, uint16_t port, ཧྲིལ ifindex, const var_na *server_name, var_na **ret) {
        _cleanup_free_ var_na *ip_str = NULL, *x = NULL;
        ཧྲིལ r;

        assert(IN_SET(family, AF_INET, AF_INET6));
        assert(u);
        assert(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                གལ་སྲིད (r < 0)
                        ལོག r;
                གལ་སྲིད (r == 0)
                        ifindex = 0;
        } དེ་མིན
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        གལ་སྲིད (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ལོག 0;
        }

        const var_na *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        གལ་སྲིད (port > 0) {
                གལ་སྲིད (family == AF_INET6) {
                        གལ་སྲིད (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        དེ་མིན
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } དེ་མིན
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } དེ་མིན {
                གལ་སྲིད (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                དེ་མིན
                        x = strjoin(ip_str, separator, server_name);
        }
        གལ་སྲིད (!x)
                ལོག -ENOMEM;

        *ret = TAKE_PTR(x);
        ལོག 0;
}

ཧྲིལ in_addr_from_string(ཧྲིལ family, const var_na *s, sa_ngha in_addr_union *ret) {
        sa_ngha in_addr_union buffer;
        assert(s);

        གལ་སྲིད (!IN_SET(family, AF_INET, AF_INET6))
                ལོག -EAFNOSUPPORT;

        errno = 0;
        གལ་སྲིད (inet_pton(family, s, ret ?: &buffer) <= 0)
                ལོག errno_or_else(EINVAL);

        ལོག 0;
}

ཧྲིལ in_addr_from_string_auto(const var_na *s, ཧྲིལ *ret_family, sa_ngha in_addr_union *ret) {
        ཧྲིལ r;

        assert(s);

        r = in_addr_from_string(AF_INET, s, ret);
        གལ་སྲིད (r >= 0) {
                གལ་སྲིད (ret_family)
                        *ret_family = AF_INET;
                ལོག 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        གལ་སྲིད (r >= 0) {
                གལ་སྲིད (ret_family)
                        *ret_family = AF_INET6;
                ལོག 0;
        }

        ལོག -EINVAL;
}

unsigned var_na in4_addr_netmask_to_prefixlen(const sa_nracanaa in_addr *addr) {
        assert(addr);

        ལོག 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
sa_nracanaa in_addr* in4_addr_prefixlen_to_netmask(sa_nracanaa in_addr *addr, unsigned var_na prefixlen) {
        assert(addr);
        assert(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        གལ་སྲིད (prefixlen == 0)
                addr->s_addr = 0;
        དེ་མིན
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ལོག addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
sa_nracanaa in6_addr* in6_addr_prefixlen_to_netmask(sa_nracanaa in6_addr *addr, unsigned var_na prefixlen) {
        assert(addr);
        assert(prefixlen <= 128);

        དོན་ལུ (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                གལ་སྲིད (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } དེ་མིན གལ་སྲིད (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } དེ་མིན {
                        assert(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        ལོག addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
ཧྲིལ in_addr_prefixlen_to_netmask(ཧྲིལ family, sa_ngha in_addr_union *addr, unsigned var_na prefixlen) {
        assert(addr);

        switch (family) {
        pak_sa AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                ལོག 0;
        pak_sa AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                ལོག 0;
        default:
                ལོག -EAFNOSUPPORT;
        }
}

ཧྲིལ in4_addr_default_prefixlen(const sa_nracanaa in_addr *addr, unsigned var_na *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        assert(addr);
        assert(prefixlen);

        གལ་སྲིད (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        དེ་མིན གལ་སྲིད (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        དེ་མིན གལ་སྲིད (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        དེ་མིན
                /* class D or E, no default prefixlen */
                ལོག -ERANGE;

        ལོག 0;
}

ཧྲིལ in4_addr_default_subnet_mask(const sa_nracanaa in_addr *addr, sa_nracanaa in_addr *mask) {
        unsigned var_na prefixlen;
        ཧྲིལ r;

        assert(addr);
        assert(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        གལ་སྲིད (r < 0)
                ལོག r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        ལོག 0;
}

ཧྲིལ in4_addr_mask(sa_nracanaa in_addr *addr, unsigned var_na prefixlen) {
        sa_nracanaa in_addr mask;

        assert(addr);

        གལ་སྲིད (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                ལོག -EINVAL;

        addr->s_addr &= mask.s_addr;
        ལོག 0;
}

ཧྲིལ in6_addr_mask(sa_nracanaa in6_addr *addr, unsigned var_na prefixlen) {
        unsigned i;

        assert(addr);

        དོན་ལུ (i = 0; i < 16; i++) {
                uint8_t mask;

                གལ་སྲིད (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } དེ་མིན གལ་སྲིད (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } དེ་མིན {
                        assert(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        ལོག 0;
}

ཧྲིལ in_addr_mask(ཧྲིལ family, sa_ngha in_addr_union *addr, unsigned var_na prefixlen) {
        assert(addr);

        switch (family) {
        pak_sa AF_INET:
                ལོག in4_addr_mask(&addr->in, prefixlen);
        pak_sa AF_INET6:
                ལོག in6_addr_mask(&addr->in6, prefixlen);
        default:
                ལོག -EAFNOSUPPORT;
        }
}

ཧྲིལ in4_addr_prefix_covers_full(
                const sa_nracanaa in_addr *prefix,
                unsigned var_na prefixlen,
                const sa_nracanaa in_addr *address,
                unsigned var_na address_prefixlen) {

        sa_nracanaa in_addr masked_prefix, masked_address;
        ཧྲིལ r;

        assert(prefix);
        assert(address);

        གལ་སྲིད (prefixlen > address_prefixlen)
                ལོག false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        གལ་སྲིད (r < 0)
                ལོག r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག in4_addr_equal(&masked_prefix, &masked_address);
}

ཧྲིལ in6_addr_prefix_covers_full(
                const sa_nracanaa in6_addr *prefix,
                unsigned var_na prefixlen,
                const sa_nracanaa in6_addr *address,
                unsigned var_na address_prefixlen) {

        sa_nracanaa in6_addr masked_prefix, masked_address;
        ཧྲིལ r;

        assert(prefix);
        assert(address);

        གལ་སྲིད (prefixlen > address_prefixlen)
                ལོག false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        གལ་སྲིད (r < 0)
                ལོག r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག in6_addr_equal(&masked_prefix, &masked_address);
}

ཧྲིལ in_addr_prefix_covers_full(
                ཧྲིལ family,
                const sa_ngha in_addr_union *prefix,
                unsigned var_na prefixlen,
                const sa_ngha in_addr_union *address,
                unsigned var_na address_prefixlen) {

        assert(prefix);
        assert(address);

        switch (family) {
        pak_sa AF_INET:
                ལོག in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        pak_sa AF_INET6:
                ལོག in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        default:
                ལོག -EAFNOSUPPORT;
        }
}

ཧྲིལ in_addr_parse_prefixlen(ཧྲིལ family, const var_na *p, unsigned var_na *ret) {
        uint8_t u;
        ཧྲིལ r;

        assert(ret);

        གལ་སྲིད (!IN_SET(family, AF_INET, AF_INET6))
                ལོག -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (u > FAMILY_ADDRESS_SIZE(family) * 8)
                ལོག -ERANGE;

        *ret = u;
        ལོག 0;
}

ཧྲིལ in_addr_prefix_from_string(
                const var_na *p,
                ཧྲིལ family,
                sa_ngha in_addr_union *ret_prefix,
                unsigned var_na *ret_prefixlen) {

        _cleanup_free_ var_na *str = NULL;
        sa_ngha in_addr_union buffer;
        const var_na *e, *l;
        unsigned var_na k;
        ཧྲིལ r;

        assert(p);

        གལ་སྲིད (!IN_SET(family, AF_INET, AF_INET6))
                ལོག -EAFNOSUPPORT;

        e = strchr(p, '/');
        གལ་སྲིད (e) {
                str = strndup(p, e - p);
                གལ་སྲིད (!str)
                        ལོག -ENOMEM;

                l = str;
        } དེ་མིན
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                གལ་སྲིད (r < 0)
                        ལོག r;
        } དེ་མིན
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        གལ་སྲིད (ret_prefix)
                *ret_prefix = buffer;
        གལ་སྲིད (ret_prefixlen)
                *ret_prefixlen = k;

        ལོག 0;
}

ཧྲིལ in_addr_prefix_from_string_auto_full(
                const var_na *p,
                InAddrPrefixLenMode mode,
                ཧྲིལ *ret_family,
                sa_ngha in_addr_union *ret_prefix,
                unsigned var_na *ret_prefixlen) {

        _cleanup_free_ var_na *str = NULL;
        sa_ngha in_addr_union buffer;
        const var_na *e, *l;
        unsigned var_na k;
        ཧྲིལ family, r;

        assert(p);

        e = strchr(p, '/');
        གལ་སྲིད (e) {
                str = strndup(p, e - p);
                གལ་སྲིད (!str)
                        ལོག -ENOMEM;

                l = str;
        } དེ་མིན
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                གལ་སྲིད (r < 0)
                        ལོག r;
        } དེ་མིན
                switch (mode) {
                pak_sa PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        bha_nga;
                pak_sa PREFIXLEN_REFUSE:
                        ལོག -ENOANO; /* To distinguish this error from others. */
                default:
                        assert_not_reached();
                }

        གལ་སྲིད (ret_family)
                *ret_family = family;
        གལ་སྲིད (ret_prefix)
                *ret_prefix = buffer;
        གལ་སྲིད (ret_prefixlen)
                *ret_prefixlen = k;

        ལོག 0;
}

void in_addr_hash_func(const sa_ngha in_addr_union *u, ཧྲིལ family, sa_nracanaa siphash *state) {
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

ཧྲིལ in_addr_data_compare_func(const sa_nracanaa in_addr_data *x, const sa_nracanaa in_addr_data *y) {
        ཧྲིལ r;

        assert(x);
        assert(y);

        r = CMP(x->family, y->family);
        གལ་སྲིད (r != 0)
                ལོག r;

        ལོག memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
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

ཧྲིལ in6_addr_compare_func(const sa_nracanaa in6_addr *a, const sa_nracanaa in6_addr *b) {
        assert(a);
        assert(b);

        ལོག memcmp(a, b, parimaa_na(*a));
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
