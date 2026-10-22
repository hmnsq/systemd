/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <arpa/inet.h>
#ضمن <endian.h>
#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "hash-funcs.h"
#ضمن "in-addr-util.h"
#ضمن "logarithm.h"
#ضمن "memory-util.h"
#ضمن "parse-util.h"
#ضمن "random-util.h"
#ضمن "siphash24.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

bool in4_addr_is_null(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع a->s_addr == 0;
}

bool in6_addr_is_null(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع eqzero(a->s6_addr32);
}

صحيح in_addr_is_null(صحيح family, ثابت اتحاد in_addr_union *u) {
        أكد(u);

        𐤀𐤌 (family == AF_INET)
                أرجع in4_addr_is_null(&u->in);

        𐤀𐤌 (family == AF_INET6)
                أرجع in6_addr_is_null(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ثابت بنية in_addr *a) {
        أكد(a);

        𐤀𐤌 (!in4_addr_is_link_local(a))
                أرجع false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        أرجع !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

صحيح in_addr_is_link_local(صحيح family, ثابت اتحاد in_addr_union *u) {
        أكد(u);

        𐤀𐤌 (family == AF_INET)
                أرجع in4_addr_is_link_local(&u->in);

        𐤀𐤌 (family == AF_INET6)
                أرجع in6_addr_is_link_local(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ثابت بنية in6_addr *a) {
        أكد(a);

        /* ff02::1 */
        أرجع be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع a->s6_addr[0] == 0xff;
}

صحيح in_addr_is_multicast(صحيح family, ثابت اتحاد in_addr_union *u) {
        أكد(u);

        𐤀𐤌 (family == AF_INET)
                أرجع in4_addr_is_multicast(&u->in);

        𐤀𐤌 (family == AF_INET6)
                أرجع in6_addr_is_multicast(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ثابت بنية in_addr *a) {
        أكد(a);

        /* All of 127.x.x.x is localhost. */
        أرجع (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ثابت بنية in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        أرجع !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ساكن bool in6_addr_is_loopback(ثابت بنية in6_addr *a) {
        أرجع memcmp(a, &(بنية in6_addr) IN6ADDR_LOOPBACK_INIT, حجم(بنية in6_addr)) == 0;
}

صحيح in_addr_is_localhost(صحيح family, ثابت اتحاد in_addr_union *u) {
        أكد(u);

        𐤀𐤌 (family == AF_INET)
                أرجع in4_addr_is_localhost(&u->in);

        𐤀𐤌 (family == AF_INET6)
                أرجع in6_addr_is_loopback(&u->in6);

        أرجع -EAFNOSUPPORT;
}

صحيح in_addr_is_localhost_one(صحيح family, ثابت اتحاد in_addr_union *u) {
        أكد(u);

        𐤀𐤌 (family == AF_INET)
                /* 127.0.0.1 */
                أرجع be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        𐤀𐤌 (family == AF_INET6)
                أرجع in6_addr_is_loopback(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ثابت بنية in_addr *a, ثابت بنية in_addr *b) {
        أكد(a);
        أكد(b);

        أرجع a->s_addr == b->s_addr;
}

bool in6_addr_equal(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) {
        أكد(a);
        أكد(b);

        أرجع memcmp(a, b, حجم(بنية in6_addr)) == 0;
}

صحيح in_addr_equal(صحيح family, ثابت اتحاد in_addr_union *a, ثابت اتحاد in_addr_union *b) {
        أكد(a);
        أكد(b);

        𐤀𐤌 (family == AF_INET)
                أرجع in4_addr_equal(&a->in, &b->in);

        𐤀𐤌 (family == AF_INET6)
                أرجع in6_addr_equal(&a->in6, &b->in6);

        أرجع -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                ثابت بنية in_addr *a,
                unsigned aprefixlen,
                ثابت بنية in_addr *b,
                unsigned bprefixlen) {

        أكد(a);
        أكد(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (حجم(بنية in_addr) * 8));
        𐤀𐤌 (m == 0)
                أرجع true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        أرجع (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                ثابت بنية in6_addr *a,
                unsigned aprefixlen,
                ثابت بنية in6_addr *b,
                unsigned bprefixlen) {

        أكد(a);
        أكد(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (حجم(بنية in6_addr) * 8));
        𐤀𐤌 (m == 0)
                أرجع true;

        لكل (size_t i = 0; i < حجم(بنية in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                𐤀𐤌 ((x & n) != 0)
                        أرجع false;

                𐤀𐤌 (m <= 8)
                        اكسر;

                m -= 8;
        }

        أرجع true;
}

صحيح in_addr_prefix_intersect(
                صحيح family,
                ثابت اتحاد in_addr_union *a,
                unsigned aprefixlen,
                ثابت اتحاد in_addr_union *b,
                unsigned bprefixlen) {

        أكد(a);
        أكد(b);

        /* Checks whether there are any addresses that are in both networks. */

        𐤀𐤌 (family == AF_INET)
                أرجع in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        𐤀𐤌 (family == AF_INET6)
                أرجع in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        أرجع -EAFNOSUPPORT;
}

صحيح in_addr_random_prefix(
                صحيح family,
                اتحاد in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        أكد(u);

        /* Random network part of an address by one. */

        𐤀𐤌 (prefixlen <= 0)
                أرجع 0;

        𐤀𐤌 (family == AF_INET) {
                uint32_t c, n;

                𐤀𐤌 (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                𐤀𐤌 (prefixlen > 32)
                        prefixlen = 32;
                𐤀𐤌 (prefixlen_fixed_part >= prefixlen)
                        أرجع -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, حجم(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                أرجع 1;
        }

        𐤀𐤌 (family == AF_INET6) {
                بنية in6_addr n;
                unsigned i, j;

                𐤀𐤌 (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                𐤀𐤌 (prefixlen > 128)
                        prefixlen = 128;
                𐤀𐤌 (prefixlen_fixed_part >= prefixlen)
                        أرجع -EINVAL;

                random_bytes(&n, حجم(n));

                لكل (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        𐤀𐤌 (i < (prefixlen_fixed_part + 7) / 8) {
                                𐤀𐤌 (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                وإلا {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        𐤀𐤌 (i < (prefixlen + 7) / 8) {
                                𐤀𐤌 (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                وإلا {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                أرجع 1;
        }

        أرجع -EAFNOSUPPORT;
}

صحيح in_addr_prefix_range(
                صحيح family,
                ثابت اتحاد in_addr_union *in,
                unsigned prefixlen,
                اتحاد in_addr_union *ret_start,
                اتحاد in_addr_union *ret_end) {

        صحيح r;

        أكد(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        بدل (family) {
        حالة AF_INET:
                𐤀𐤌 (prefixlen > 32)
                        أرجع -EINVAL;
                اكسر;
        حالة AF_INET6:
                𐤀𐤌 (prefixlen > 128)
                        أرجع -EINVAL;
                اكسر;
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }

        𐤀𐤌 (prefixlen == 0) {
                /* shortcut for trivial case */
                𐤀𐤌 (ret_start)
                        *ret_start = IN_ADDR_NULL;
                𐤀𐤌 (ret_end)
                        *ret_end = IN_ADDR_NULL;
                أرجع 0;
        }

        اتحاد in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        𐤀𐤌 (r < 0)
                أرجع r;

        اتحاد in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        لكل (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                𐤀𐤌 (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } وإلا {
                        end.bytes[i] += add;
                        اكسر;
                }

        𐤀𐤌 (ret_start)
                *ret_start = start;
        𐤀𐤌 (ret_end)
                *ret_end = end;

        أرجع 0;
}

صحيح in_addr_to_string(صحيح family, ثابت اتحاد in_addr_union *u, حرف **ret) {
        _cleanup_free_ حرف *x = عدم;
        size_t l;

        أكد(u);
        أكد(ret);

        𐤀𐤌 (family == AF_INET)
                l = INET_ADDRSTRLEN;
        وإلا 𐤀𐤌 (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        وإلا
                أرجع -EAFNOSUPPORT;

        x = new(حرف, l);
        𐤀𐤌 (!x)
                أرجع -ENOMEM;

        errno = 0;
        𐤀𐤌 (!typesafe_inet_ntop(family, u, x, l))
                أرجع errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        أرجع 0;
}

ثابت حرف* typesafe_inet_ntop(صحيح family, ثابت اتحاد in_addr_union *a, حرف *buf, size_t len) {
        أرجع inet_ntop(family, a, buf, len);
}

ثابت حرف* typesafe_inet_ntop4(ثابت بنية in_addr *a, حرف *buf, size_t len) {
        أرجع inet_ntop(AF_INET, a, buf, len);
}

ثابت حرف* typesafe_inet_ntop6(ثابت بنية in6_addr *a, حرف *buf, size_t len) {
        أرجع inet_ntop(AF_INET6, a, buf, len);
}

صحيح in_addr_prefix_to_string(
                صحيح family,
                ثابت اتحاد in_addr_union *u,
                unsigned prefixlen,
                حرف *buf,
                size_t buf_len) {

        أكد(u);
        أكد(buf);

        𐤀𐤌 (!IN_SET(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        errno = 0;
        𐤀𐤌 (!typesafe_inet_ntop(family, u, buf, buf_len))
                أرجع errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        𐤀𐤌 (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                أرجع -ENOSPC;
        أرجع 0;
}

صحيح in_addr_port_ifindex_name_to_string(صحيح family, ثابت اتحاد in_addr_union *u, uint16_t port, صحيح ifindex, ثابت حرف *server_name, حرف **ret) {
        _cleanup_free_ حرف *ip_str = عدم, *x = عدم;
        صحيح r;

        أكد(IN_SET(family, AF_INET, AF_INET6));
        أكد(u);
        أكد(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                𐤀𐤌 (r < 0)
                        أرجع r;
                𐤀𐤌 (r == 0)
                        ifindex = 0;
        } وإلا
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        𐤀𐤌 (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                أرجع 0;
        }

        ثابت حرف *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        𐤀𐤌 (port > 0) {
                𐤀𐤌 (family == AF_INET6) {
                        𐤀𐤌 (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        وإلا
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } وإلا
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } وإلا {
                𐤀𐤌 (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                وإلا
                        x = strjoin(ip_str, separator, server_name);
        }
        𐤀𐤌 (!x)
                أرجع -ENOMEM;

        *ret = TAKE_PTR(x);
        أرجع 0;
}

صحيح in_addr_from_string(صحيح family, ثابت حرف *s, اتحاد in_addr_union *ret) {
        اتحاد in_addr_union buffer;
        أكد(s);

        𐤀𐤌 (!IN_SET(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        errno = 0;
        𐤀𐤌 (inet_pton(family, s, ret ?: &buffer) <= 0)
                أرجع errno_or_else(EINVAL);

        أرجع 0;
}

صحيح in_addr_from_string_auto(ثابت حرف *s, صحيح *ret_family, اتحاد in_addr_union *ret) {
        صحيح r;

        أكد(s);

        r = in_addr_from_string(AF_INET, s, ret);
        𐤀𐤌 (r >= 0) {
                𐤀𐤌 (ret_family)
                        *ret_family = AF_INET;
                أرجع 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        𐤀𐤌 (r >= 0) {
                𐤀𐤌 (ret_family)
                        *ret_family = AF_INET6;
                أرجع 0;
        }

        أرجع -EINVAL;
}

unsigned حرف in4_addr_netmask_to_prefixlen(ثابت بنية in_addr *addr) {
        أكد(addr);

        أرجع 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
بنية in_addr* in4_addr_prefixlen_to_netmask(بنية in_addr *addr, unsigned حرف prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        𐤀𐤌 (prefixlen == 0)
                addr->s_addr = 0;
        وإلا
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        أرجع addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
بنية in6_addr* in6_addr_prefixlen_to_netmask(بنية in6_addr *addr, unsigned حرف prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 128);

        لكل (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                𐤀𐤌 (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } وإلا 𐤀𐤌 (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } وإلا {
                        أكد(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        أرجع addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
صحيح in_addr_prefixlen_to_netmask(صحيح family, اتحاد in_addr_union *addr, unsigned حرف prefixlen) {
        أكد(addr);

        بدل (family) {
        حالة AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                أرجع 0;
        حالة AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                أرجع 0;
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح in4_addr_default_prefixlen(ثابت بنية in_addr *addr, unsigned حرف *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        أكد(addr);
        أكد(prefixlen);

        𐤀𐤌 (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        وإلا 𐤀𐤌 (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        وإلا 𐤀𐤌 (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        وإلا
                /* class D or E, no default prefixlen */
                أرجع -ERANGE;

        أرجع 0;
}

صحيح in4_addr_default_subnet_mask(ثابت بنية in_addr *addr, بنية in_addr *mask) {
        unsigned حرف prefixlen;
        صحيح r;

        أكد(addr);
        أكد(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        𐤀𐤌 (r < 0)
                أرجع r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        أرجع 0;
}

صحيح in4_addr_mask(بنية in_addr *addr, unsigned حرف prefixlen) {
        بنية in_addr mask;

        أكد(addr);

        𐤀𐤌 (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                أرجع -EINVAL;

        addr->s_addr &= mask.s_addr;
        أرجع 0;
}

صحيح in6_addr_mask(بنية in6_addr *addr, unsigned حرف prefixlen) {
        unsigned i;

        أكد(addr);

        لكل (i = 0; i < 16; i++) {
                uint8_t mask;

                𐤀𐤌 (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } وإلا 𐤀𐤌 (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } وإلا {
                        أكد(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        أرجع 0;
}

صحيح in_addr_mask(صحيح family, اتحاد in_addr_union *addr, unsigned حرف prefixlen) {
        أكد(addr);

        بدل (family) {
        حالة AF_INET:
                أرجع in4_addr_mask(&addr->in, prefixlen);
        حالة AF_INET6:
                أرجع in6_addr_mask(&addr->in6, prefixlen);
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح in4_addr_prefix_covers_full(
                ثابت بنية in_addr *prefix,
                unsigned حرف prefixlen,
                ثابت بنية in_addr *address,
                unsigned حرف address_prefixlen) {

        بنية in_addr masked_prefix, masked_address;
        صحيح r;

        أكد(prefix);
        أكد(address);

        𐤀𐤌 (prefixlen > address_prefixlen)
                أرجع false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        𐤀𐤌 (r < 0)
                أرجع r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        𐤀𐤌 (r < 0)
                أرجع r;

        أرجع in4_addr_equal(&masked_prefix, &masked_address);
}

صحيح in6_addr_prefix_covers_full(
                ثابت بنية in6_addr *prefix,
                unsigned حرف prefixlen,
                ثابت بنية in6_addr *address,
                unsigned حرف address_prefixlen) {

        بنية in6_addr masked_prefix, masked_address;
        صحيح r;

        أكد(prefix);
        أكد(address);

        𐤀𐤌 (prefixlen > address_prefixlen)
                أرجع false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        𐤀𐤌 (r < 0)
                أرجع r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        𐤀𐤌 (r < 0)
                أرجع r;

        أرجع in6_addr_equal(&masked_prefix, &masked_address);
}

صحيح in_addr_prefix_covers_full(
                صحيح family,
                ثابت اتحاد in_addr_union *prefix,
                unsigned حرف prefixlen,
                ثابت اتحاد in_addr_union *address,
                unsigned حرف address_prefixlen) {

        أكد(prefix);
        أكد(address);

        بدل (family) {
        حالة AF_INET:
                أرجع in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        حالة AF_INET6:
                أرجع in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح in_addr_parse_prefixlen(صحيح family, ثابت حرف *p, unsigned حرف *ret) {
        uint8_t u;
        صحيح r;

        أكد(ret);

        𐤀𐤌 (!IN_SET(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (u > FAMILY_ADDRESS_SIZE(family) * 8)
                أرجع -ERANGE;

        *ret = u;
        أرجع 0;
}

صحيح in_addr_prefix_from_string(
                ثابت حرف *p,
                صحيح family,
                اتحاد in_addr_union *ret_prefix,
                unsigned حرف *ret_prefixlen) {

        _cleanup_free_ حرف *str = عدم;
        اتحاد in_addr_union buffer;
        ثابت حرف *e, *l;
        unsigned حرف k;
        صحيح r;

        أكد(p);

        𐤀𐤌 (!IN_SET(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        e = strchr(p, '/');
        𐤀𐤌 (e) {
                str = strndup(p, e - p);
                𐤀𐤌 (!str)
                        أرجع -ENOMEM;

                l = str;
        } وإلا
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                𐤀𐤌 (r < 0)
                        أرجع r;
        } وإلا
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        𐤀𐤌 (ret_prefix)
                *ret_prefix = buffer;
        𐤀𐤌 (ret_prefixlen)
                *ret_prefixlen = k;

        أرجع 0;
}

صحيح in_addr_prefix_from_string_auto_full(
                ثابت حرف *p,
                InAddrPrefixLenMode mode,
                صحيح *ret_family,
                اتحاد in_addr_union *ret_prefix,
                unsigned حرف *ret_prefixlen) {

        _cleanup_free_ حرف *str = عدم;
        اتحاد in_addr_union buffer;
        ثابت حرف *e, *l;
        unsigned حرف k;
        صحيح family, r;

        أكد(p);

        e = strchr(p, '/');
        𐤀𐤌 (e) {
                str = strndup(p, e - p);
                𐤀𐤌 (!str)
                        أرجع -ENOMEM;

                l = str;
        } وإلا
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                𐤀𐤌 (r < 0)
                        أرجع r;
        } وإلا
                بدل (mode) {
                حالة PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        اكسر;
                حالة PREFIXLEN_REFUSE:
                        أرجع -ENOANO; /* To distinguish this error from others. */
                افتراضي:
                        assert_not_reached();
                }

        𐤀𐤌 (ret_family)
                *ret_family = family;
        𐤀𐤌 (ret_prefix)
                *ret_prefix = buffer;
        𐤀𐤌 (ret_prefixlen)
                *ret_prefixlen = k;

        أرجع 0;
}

فراغ in_addr_hash_func(ثابت اتحاد in_addr_union *u, صحيح family, بنية siphash *state) {
        أكد(u);
        أكد(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

فراغ in_addr_data_hash_func(ثابت بنية in_addr_data *a, بنية siphash *state) {
        أكد(a);
        أكد(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

صحيح in_addr_data_compare_func(ثابت بنية in_addr_data *x, ثابت بنية in_addr_data *y) {
        صحيح r;

        أكد(x);
        أكد(y);

        r = CMP(x->family, y->family);
        𐤀𐤌 (r != 0)
                أرجع r;

        أرجع memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        بنية in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        بنية in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        حرر);

فراغ in6_addr_hash_func(ثابت بنية in6_addr *addr, بنية siphash *state) {
        أكد(addr);
        أكد(state);

        siphash24_compress_typesafe(*addr, state);
}

صحيح in6_addr_compare_func(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) {
        أكد(a);
        أكد(b);

        أرجع memcmp(a, b, حجم(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        بنية in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
        in6_addr_hash_ops_free,
        بنية in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        حرر);
