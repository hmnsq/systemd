/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <arpa/inet.h>
#અન્તર્ભાવ <endian.h>
#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "hash-funcs.h"
#અન્તર્ભાવ "in-addr-util.h"
#અન્તર્ભાવ "logarithm.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "random-util.h"
#અન્તર્ભાવ "siphash24.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"

bool in4_addr_is_null(અચલ સંરચના in_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા a->s_addr == 0;
}

bool in6_addr_is_null(અચલ સંરચના in6_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા eqzero(a->s6_addr32);
}

પૂર્ણાંક in_addr_is_null(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) {
        પ્રતિજ્ઞા(u);

        જો (family == AF_INET)
                પાછા in4_addr_is_null(&u->in);

        જો (family == AF_INET6)
                પાછા in6_addr_is_null(&u->in6);

        પાછા -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(અચલ સંરચના in_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(અચલ સંરચના in_addr *a) {
        પ્રતિજ્ઞા(a);

        જો (!in4_addr_is_link_local(a))
                પાછા false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        પાછા !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(અચલ સંરચના in6_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

પૂર્ણાંક in_addr_is_link_local(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) {
        પ્રતિજ્ઞા(u);

        જો (family == AF_INET)
                પાછા in4_addr_is_link_local(&u->in);

        જો (family == AF_INET6)
                પાછા in6_addr_is_link_local(&u->in6);

        પાછા -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(અચલ સંરચના in6_addr *a) {
        પ્રતિજ્ઞા(a);

        /* ff02::1 */
        પાછા be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(અચલ સંરચના in_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(અચલ સંરચના in6_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા a->s6_addr[0] == 0xff;
}

પૂર્ણાંક in_addr_is_multicast(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) {
        પ્રતિજ્ઞા(u);

        જો (family == AF_INET)
                પાછા in4_addr_is_multicast(&u->in);

        જો (family == AF_INET6)
                પાછા in6_addr_is_multicast(&u->in6);

        પાછા -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(અચલ સંરચના in_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(અચલ સંરચના in_addr *a) {
        પ્રતિજ્ઞા(a);

        /* All of 127.x.x.x is localhost. */
        પાછા (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(અચલ સંરચના in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        પાછા !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

સ્થિર bool in6_addr_is_loopback(અચલ સંરચના in6_addr *a) {
        પાછા memcmp(a, &(સંરચના in6_addr) IN6ADDR_LOOPBACK_INIT, કદ(સંરચના in6_addr)) == 0;
}

પૂર્ણાંક in_addr_is_localhost(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) {
        પ્રતિજ્ઞા(u);

        જો (family == AF_INET)
                પાછા in4_addr_is_localhost(&u->in);

        જો (family == AF_INET6)
                પાછા in6_addr_is_loopback(&u->in6);

        પાછા -EAFNOSUPPORT;
}

પૂર્ણાંક in_addr_is_localhost_one(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u) {
        પ્રતિજ્ઞા(u);

        જો (family == AF_INET)
                /* 127.0.0.1 */
                પાછા be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        જો (family == AF_INET6)
                પાછા in6_addr_is_loopback(&u->in6);

        પાછા -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(અચલ સંરચના in6_addr *a) {
        પ્રતિજ્ઞા(a);

        પાછા a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(અચલ સંરચના in_addr *a, અચલ સંરચના in_addr *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા a->s_addr == b->s_addr;
}

bool in6_addr_equal(અચલ સંરચના in6_addr *a, અચલ સંરચના in6_addr *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા memcmp(a, b, કદ(સંરચના in6_addr)) == 0;
}

પૂર્ણાંક in_addr_equal(પૂર્ણાંક family, અચલ સંઘ in_addr_union *a, અચલ સંઘ in_addr_union *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        જો (family == AF_INET)
                પાછા in4_addr_equal(&a->in, &b->in);

        જો (family == AF_INET6)
                પાછા in6_addr_equal(&a->in6, &b->in6);

        પાછા -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                અચલ સંરચના in_addr *a,
                unsigned aprefixlen,
                અચલ સંરચના in_addr *b,
                unsigned bprefixlen) {

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (કદ(સંરચના in_addr) * 8));
        જો (m == 0)
                પાછા true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        પાછા (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                અચલ સંરચના in6_addr *a,
                unsigned aprefixlen,
                અચલ સંરચના in6_addr *b,
                unsigned bprefixlen) {

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (કદ(સંરચના in6_addr) * 8));
        જો (m == 0)
                પાછા true;

        માટે (size_t i = 0; i < કદ(સંરચના in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                જો ((x & n) != 0)
                        પાછા false;

                જો (m <= 8)
                        તોડો;

                m -= 8;
        }

        પાછા true;
}

પૂર્ણાંક in_addr_prefix_intersect(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *a,
                unsigned aprefixlen,
                અચલ સંઘ in_addr_union *b,
                unsigned bprefixlen) {

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        /* Checks whether there are any addresses that are in both networks. */

        જો (family == AF_INET)
                પાછા in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        જો (family == AF_INET6)
                પાછા in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        પાછા -EAFNOSUPPORT;
}

પૂર્ણાંક in_addr_random_prefix(
                પૂર્ણાંક family,
                સંઘ in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        પ્રતિજ્ઞા(u);

        /* Random network part of an address by one. */

        જો (prefixlen <= 0)
                પાછા 0;

        જો (family == AF_INET) {
                uint32_t c, n;

                જો (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                જો (prefixlen > 32)
                        prefixlen = 32;
                જો (prefixlen_fixed_part >= prefixlen)
                        પાછા -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, કદ(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                પાછા 1;
        }

        જો (family == AF_INET6) {
                સંરચના in6_addr n;
                unsigned i, j;

                જો (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                જો (prefixlen > 128)
                        prefixlen = 128;
                જો (prefixlen_fixed_part >= prefixlen)
                        પાછા -EINVAL;

                random_bytes(&n, કદ(n));

                માટે (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        જો (i < (prefixlen_fixed_part + 7) / 8) {
                                જો (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                નહીંતર {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        જો (i < (prefixlen + 7) / 8) {
                                જો (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                નહીંતર {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                પાછા 1;
        }

        પાછા -EAFNOSUPPORT;
}

પૂર્ણાંક in_addr_prefix_range(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *in,
                unsigned prefixlen,
                સંઘ in_addr_union *ret_start,
                સંઘ in_addr_union *ret_end) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        પસંદગી (family) {
        સ્થિતિ AF_INET:
                જો (prefixlen > 32)
                        પાછા -EINVAL;
                તોડો;
        સ્થિતિ AF_INET6:
                જો (prefixlen > 128)
                        પાછા -EINVAL;
                તોડો;
        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }

        જો (prefixlen == 0) {
                /* shortcut for trivial case */
                જો (ret_start)
                        *ret_start = IN_ADDR_NULL;
                જો (ret_end)
                        *ret_end = IN_ADDR_NULL;
                પાછા 0;
        }

        સંઘ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        જો (r < 0)
                પાછા r;

        સંઘ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        માટે (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                જો (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } નહીંતર {
                        end.bytes[i] += add;
                        તોડો;
                }

        જો (ret_start)
                *ret_start = start;
        જો (ret_end)
                *ret_end = end;

        પાછા 0;
}

પૂર્ણાંક in_addr_to_string(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *x = ખાલી;
        size_t l;

        પ્રતિજ્ઞા(u);
        પ્રતિજ્ઞા(ret);

        જો (family == AF_INET)
                l = INET_ADDRSTRLEN;
        નહીંતર જો (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        નહીંતર
                પાછા -EAFNOSUPPORT;

        x = new(અક્ષર, l);
        જો (!x)
                પાછા -ENOMEM;

        errno = 0;
        જો (!typesafe_inet_ntop(family, u, x, l))
                પાછા errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        પાછા 0;
}

અચલ અક્ષર* typesafe_inet_ntop(પૂર્ણાંક family, અચલ સંઘ in_addr_union *a, અક્ષર *buf, size_t len) {
        પાછા inet_ntop(family, a, buf, len);
}

અચલ અક્ષર* typesafe_inet_ntop4(અચલ સંરચના in_addr *a, અક્ષર *buf, size_t len) {
        પાછા inet_ntop(AF_INET, a, buf, len);
}

અચલ અક્ષર* typesafe_inet_ntop6(અચલ સંરચના in6_addr *a, અક્ષર *buf, size_t len) {
        પાછા inet_ntop(AF_INET6, a, buf, len);
}

પૂર્ણાંક in_addr_prefix_to_string(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *u,
                unsigned prefixlen,
                અક્ષર *buf,
                size_t buf_len) {

        પ્રતિજ્ઞા(u);
        પ્રતિજ્ઞા(buf);

        જો (!IN_SET(family, AF_INET, AF_INET6))
                પાછા -EAFNOSUPPORT;

        errno = 0;
        જો (!typesafe_inet_ntop(family, u, buf, buf_len))
                પાછા errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        જો (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                પાછા -ENOSPC;
        પાછા 0;
}

પૂર્ણાંક in_addr_port_ifindex_name_to_string(પૂર્ણાંક family, અચલ સંઘ in_addr_union *u, uint16_t port, પૂર્ણાંક ifindex, અચલ અક્ષર *server_name, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *ip_str = ખાલી, *x = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(IN_SET(family, AF_INET, AF_INET6));
        પ્રતિજ્ઞા(u);
        પ્રતિજ્ઞા(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        જો (r < 0)
                પાછા r;

        જો (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        ifindex = 0;
        } નહીંતર
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        જો (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                પાછા 0;
        }

        અચલ અક્ષર *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        જો (port > 0) {
                જો (family == AF_INET6) {
                        જો (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        નહીંતર
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } નહીંતર
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } નહીંતર {
                જો (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                નહીંતર
                        x = strjoin(ip_str, separator, server_name);
        }
        જો (!x)
                પાછા -ENOMEM;

        *ret = TAKE_PTR(x);
        પાછા 0;
}

પૂર્ણાંક in_addr_from_string(પૂર્ણાંક family, અચલ અક્ષર *s, સંઘ in_addr_union *ret) {
        સંઘ in_addr_union buffer;
        પ્રતિજ્ઞા(s);

        જો (!IN_SET(family, AF_INET, AF_INET6))
                પાછા -EAFNOSUPPORT;

        errno = 0;
        જો (inet_pton(family, s, ret ?: &buffer) <= 0)
                પાછા errno_or_else(EINVAL);

        પાછા 0;
}

પૂર્ણાંક in_addr_from_string_auto(અચલ અક્ષર *s, પૂર્ણાંક *ret_family, સંઘ in_addr_union *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        r = in_addr_from_string(AF_INET, s, ret);
        જો (r >= 0) {
                જો (ret_family)
                        *ret_family = AF_INET;
                પાછા 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        જો (r >= 0) {
                જો (ret_family)
                        *ret_family = AF_INET6;
                પાછા 0;
        }

        પાછા -EINVAL;
}

unsigned અક્ષર in4_addr_netmask_to_prefixlen(અચલ સંરચના in_addr *addr) {
        પ્રતિજ્ઞા(addr);

        પાછા 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
સંરચના in_addr* in4_addr_prefixlen_to_netmask(સંરચના in_addr *addr, unsigned અક્ષર prefixlen) {
        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        જો (prefixlen == 0)
                addr->s_addr = 0;
        નહીંતર
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        પાછા addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
સંરચના in6_addr* in6_addr_prefixlen_to_netmask(સંરચના in6_addr *addr, unsigned અક્ષર prefixlen) {
        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(prefixlen <= 128);

        માટે (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                જો (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } નહીંતર જો (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } નહીંતર {
                        પ્રતિજ્ઞા(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        પાછા addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
પૂર્ણાંક in_addr_prefixlen_to_netmask(પૂર્ણાંક family, સંઘ in_addr_union *addr, unsigned અક્ષર prefixlen) {
        પ્રતિજ્ઞા(addr);

        પસંદગી (family) {
        સ્થિતિ AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                પાછા 0;
        સ્થિતિ AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                પાછા 0;
        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

પૂર્ણાંક in4_addr_default_prefixlen(અચલ સંરચના in_addr *addr, unsigned અક્ષર *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(prefixlen);

        જો (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        નહીંતર જો (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        નહીંતર જો (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        નહીંતર
                /* class D or E, no default prefixlen */
                પાછા -ERANGE;

        પાછા 0;
}

પૂર્ણાંક in4_addr_default_subnet_mask(અચલ સંરચના in_addr *addr, સંરચના in_addr *mask) {
        unsigned અક્ષર prefixlen;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        જો (r < 0)
                પાછા r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        પાછા 0;
}

પૂર્ણાંક in4_addr_mask(સંરચના in_addr *addr, unsigned અક્ષર prefixlen) {
        સંરચના in_addr mask;

        પ્રતિજ્ઞા(addr);

        જો (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                પાછા -EINVAL;

        addr->s_addr &= mask.s_addr;
        પાછા 0;
}

પૂર્ણાંક in6_addr_mask(સંરચના in6_addr *addr, unsigned અક્ષર prefixlen) {
        unsigned i;

        પ્રતિજ્ઞા(addr);

        માટે (i = 0; i < 16; i++) {
                uint8_t mask;

                જો (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } નહીંતર જો (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } નહીંતર {
                        પ્રતિજ્ઞા(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        પાછા 0;
}

પૂર્ણાંક in_addr_mask(પૂર્ણાંક family, સંઘ in_addr_union *addr, unsigned અક્ષર prefixlen) {
        પ્રતિજ્ઞા(addr);

        પસંદગી (family) {
        સ્થિતિ AF_INET:
                પાછા in4_addr_mask(&addr->in, prefixlen);
        સ્થિતિ AF_INET6:
                પાછા in6_addr_mask(&addr->in6, prefixlen);
        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

પૂર્ણાંક in4_addr_prefix_covers_full(
                અચલ સંરચના in_addr *prefix,
                unsigned અક્ષર prefixlen,
                અચલ સંરચના in_addr *address,
                unsigned અક્ષર address_prefixlen) {

        સંરચના in_addr masked_prefix, masked_address;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(prefix);
        પ્રતિજ્ઞા(address);

        જો (prefixlen > address_prefixlen)
                પાછા false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        જો (r < 0)
                પાછા r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        જો (r < 0)
                પાછા r;

        પાછા in4_addr_equal(&masked_prefix, &masked_address);
}

પૂર્ણાંક in6_addr_prefix_covers_full(
                અચલ સંરચના in6_addr *prefix,
                unsigned અક્ષર prefixlen,
                અચલ સંરચના in6_addr *address,
                unsigned અક્ષર address_prefixlen) {

        સંરચના in6_addr masked_prefix, masked_address;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(prefix);
        પ્રતિજ્ઞા(address);

        જો (prefixlen > address_prefixlen)
                પાછા false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        જો (r < 0)
                પાછા r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        જો (r < 0)
                પાછા r;

        પાછા in6_addr_equal(&masked_prefix, &masked_address);
}

પૂર્ણાંક in_addr_prefix_covers_full(
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *prefix,
                unsigned અક્ષર prefixlen,
                અચલ સંઘ in_addr_union *address,
                unsigned અક્ષર address_prefixlen) {

        પ્રતિજ્ઞા(prefix);
        પ્રતિજ્ઞા(address);

        પસંદગી (family) {
        સ્થિતિ AF_INET:
                પાછા in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        સ્થિતિ AF_INET6:
                પાછા in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

પૂર્ણાંક in_addr_parse_prefixlen(પૂર્ણાંક family, અચલ અક્ષર *p, unsigned અક્ષર *ret) {
        uint8_t u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        જો (!IN_SET(family, AF_INET, AF_INET6))
                પાછા -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        જો (r < 0)
                પાછા r;

        જો (u > FAMILY_ADDRESS_SIZE(family) * 8)
                પાછા -ERANGE;

        *ret = u;
        પાછા 0;
}

પૂર્ણાંક in_addr_prefix_from_string(
                અચલ અક્ષર *p,
                પૂર્ણાંક family,
                સંઘ in_addr_union *ret_prefix,
                unsigned અક્ષર *ret_prefixlen) {

        _cleanup_free_ અક્ષર *str = ખાલી;
        સંઘ in_addr_union buffer;
        અચલ અક્ષર *e, *l;
        unsigned અક્ષર k;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);

        જો (!IN_SET(family, AF_INET, AF_INET6))
                પાછા -EAFNOSUPPORT;

        e = strchr(p, '/');
        જો (e) {
                str = strndup(p, e - p);
                જો (!str)
                        પાછા -ENOMEM;

                l = str;
        } નહીંતર
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        જો (r < 0)
                પાછા r;

        જો (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                જો (r < 0)
                        પાછા r;
        } નહીંતર
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        જો (ret_prefix)
                *ret_prefix = buffer;
        જો (ret_prefixlen)
                *ret_prefixlen = k;

        પાછા 0;
}

પૂર્ણાંક in_addr_prefix_from_string_auto_full(
                અચલ અક્ષર *p,
                InAddrPrefixLenMode mode,
                પૂર્ણાંક *ret_family,
                સંઘ in_addr_union *ret_prefix,
                unsigned અક્ષર *ret_prefixlen) {

        _cleanup_free_ અક્ષર *str = ખાલી;
        સંઘ in_addr_union buffer;
        અચલ અક્ષર *e, *l;
        unsigned અક્ષર k;
        પૂર્ણાંક family, r;

        પ્રતિજ્ઞા(p);

        e = strchr(p, '/');
        જો (e) {
                str = strndup(p, e - p);
                જો (!str)
                        પાછા -ENOMEM;

                l = str;
        } નહીંતર
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        જો (r < 0)
                પાછા r;

        જો (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                જો (r < 0)
                        પાછા r;
        } નહીંતર
                પસંદગી (mode) {
                સ્થિતિ PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        તોડો;
                સ્થિતિ PREFIXLEN_REFUSE:
                        પાછા -ENOANO; /* To distinguish this error from others. */
                મૂળભૂત:
                        assert_not_reached();
                }

        જો (ret_family)
                *ret_family = family;
        જો (ret_prefix)
                *ret_prefix = buffer;
        જો (ret_prefixlen)
                *ret_prefixlen = k;

        પાછા 0;
}

શૂન્ય in_addr_hash_func(અચલ સંઘ in_addr_union *u, પૂર્ણાંક family, સંરચના siphash *state) {
        પ્રતિજ્ઞા(u);
        પ્રતિજ્ઞા(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

શૂન્ય in_addr_data_hash_func(અચલ સંરચના in_addr_data *a, સંરચના siphash *state) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

પૂર્ણાંક in_addr_data_compare_func(અચલ સંરચના in_addr_data *x, અચલ સંરચના in_addr_data *y) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(x);
        પ્રતિજ્ઞા(y);

        r = CMP(x->family, y->family);
        જો (r != 0)
                પાછા r;

        પાછા memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        સંરચના in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        સંરચના in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        મુક્ત);

શૂન્ય in6_addr_hash_func(અચલ સંરચના in6_addr *addr, સંરચના siphash *state) {
        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(state);

        siphash24_compress_typesafe(*addr, state);
}

પૂર્ણાંક in6_addr_compare_func(અચલ સંરચના in6_addr *a, અચલ સંરચના in6_addr *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા memcmp(a, b, કદ(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        સંરચના in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(
        in6_addr_hash_ops_free,
        સંરચના in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        મુક્ત);
