/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <arpa/inet.h>
#অন্তর্ভাৱ <endian.h>
#অন্তর্ভাৱ <stdio.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "hash-funcs.h"
#অন্তর্ভাৱ "in-addr-util.h"
#অন্তর্ভাৱ "logarithm.h"
#অন্তর্ভাৱ "memory-util.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "random-util.h"
#অন্তর্ভাৱ "siphash24.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-util.h"

bool in4_addr_is_null(ধ্রুৱ সংরচনা in_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই a->s_addr == 0;
}

bool in6_addr_is_null(ধ্রুৱ সংরচনা in6_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই eqzero(a->s6_addr32);
}

পূর্ণাঙ্ক in_addr_is_null(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ঘূৰাই in4_addr_is_null(&u->in);

        যদি (family == AF_INET6)
                ঘূৰাই in6_addr_is_null(&u->in6);

        ঘূৰাই -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ধ্রুৱ সংরচনা in_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ধ্রুৱ সংরচনা in_addr *a) {
        প্রতিজ্ঞা(a);

        যদি (!in4_addr_is_link_local(a))
                ঘূৰাই false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ঘূৰাই !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ধ্রুৱ সংরচনা in6_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

পূর্ণাঙ্ক in_addr_is_link_local(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ঘূৰাই in4_addr_is_link_local(&u->in);

        যদি (family == AF_INET6)
                ঘূৰাই in6_addr_is_link_local(&u->in6);

        ঘূৰাই -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ধ্রুৱ সংরচনা in6_addr *a) {
        প্রতিজ্ঞা(a);

        /* ff02::1 */
        ঘূৰাই be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ধ্রুৱ সংরচনা in_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ধ্রুৱ সংরচনা in6_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই a->s6_addr[0] == 0xff;
}

পূর্ণাঙ্ক in_addr_is_multicast(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ঘূৰাই in4_addr_is_multicast(&u->in);

        যদি (family == AF_INET6)
                ঘূৰাই in6_addr_is_multicast(&u->in6);

        ঘূৰাই -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ধ্রুৱ সংরচনা in_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ধ্রুৱ সংরচনা in_addr *a) {
        প্রতিজ্ঞা(a);

        /* All of 127.x.x.x is localhost. */
        ঘূৰাই (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ধ্রুৱ সংরচনা in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ঘূৰাই !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

স্থির bool in6_addr_is_loopback(ধ্রুৱ সংরচনা in6_addr *a) {
        ঘূৰাই memcmp(a, &(সংরচনা in6_addr) IN6ADDR_LOOPBACK_INIT, পরিমাণ(সংরচনা in6_addr)) == 0;
}

পূর্ণাঙ্ক in_addr_is_localhost(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ঘূৰাই in4_addr_is_localhost(&u->in);

        যদি (family == AF_INET6)
                ঘূৰাই in6_addr_is_loopback(&u->in6);

        ঘূৰাই -EAFNOSUPPORT;
}

পূর্ণাঙ্ক in_addr_is_localhost_one(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                /* 127.0.0.1 */
                ঘূৰাই be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        যদি (family == AF_INET6)
                ঘূৰাই in6_addr_is_loopback(&u->in6);

        ঘূৰাই -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(ধ্রুৱ সংরচনা in6_addr *a) {
        প্রতিজ্ঞা(a);

        ঘূৰাই a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ধ্রুৱ সংরচনা in_addr *a, ধ্রুৱ সংরচনা in_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ঘূৰাই a->s_addr == b->s_addr;
}

bool in6_addr_equal(ধ্রুৱ সংরচনা in6_addr *a, ধ্রুৱ সংরচনা in6_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ঘূৰাই memcmp(a, b, পরিমাণ(সংরচনা in6_addr)) == 0;
}

পূর্ণাঙ্ক in_addr_equal(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *a, ধ্রুৱ সঙ্ঘ in_addr_union *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (family == AF_INET)
                ঘূৰাই in4_addr_equal(&a->in, &b->in);

        যদি (family == AF_INET6)
                ঘূৰাই in6_addr_equal(&a->in6, &b->in6);

        ঘূৰাই -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                ধ্রুৱ সংরচনা in_addr *a,
                unsigned aprefixlen,
                ধ্রুৱ সংরচনা in_addr *b,
                unsigned bprefixlen) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (পরিমাণ(সংরচনা in_addr) * 8));
        যদি (m == 0)
                ঘূৰাই true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ঘূৰাই (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                ধ্রুৱ সংরচনা in6_addr *a,
                unsigned aprefixlen,
                ধ্রুৱ সংরচনা in6_addr *b,
                unsigned bprefixlen) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (পরিমাণ(সংরচনা in6_addr) * 8));
        যদি (m == 0)
                ঘূৰাই true;

        বাবে (size_t i = 0; i < পরিমাণ(সংরচনা in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                যদি ((x & n) != 0)
                        ঘূৰাই false;

                যদি (m <= 8)
                        ভঙ্গ;

                m -= 8;
        }

        ঘূৰাই true;
}

পূর্ণাঙ্ক in_addr_prefix_intersect(
                পূর্ণাঙ্ক family,
                ধ্রুৱ সঙ্ঘ in_addr_union *a,
                unsigned aprefixlen,
                ধ্রুৱ সঙ্ঘ in_addr_union *b,
                unsigned bprefixlen) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        /* Checks whether there are any addresses that are in both networks. */

        যদি (family == AF_INET)
                ঘূৰাই in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        যদি (family == AF_INET6)
                ঘূৰাই in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ঘূৰাই -EAFNOSUPPORT;
}

পূর্ণাঙ্ক in_addr_random_prefix(
                পূর্ণাঙ্ক family,
                সঙ্ঘ in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        প্রতিজ্ঞা(u);

        /* Random network part of an address by one. */

        যদি (prefixlen <= 0)
                ঘূৰাই 0;

        যদি (family == AF_INET) {
                uint32_t c, n;

                যদি (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                যদি (prefixlen > 32)
                        prefixlen = 32;
                যদি (prefixlen_fixed_part >= prefixlen)
                        ঘূৰাই -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, পরিমাণ(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                ঘূৰাই 1;
        }

        যদি (family == AF_INET6) {
                সংরচনা in6_addr n;
                unsigned i, j;

                যদি (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                যদি (prefixlen > 128)
                        prefixlen = 128;
                যদি (prefixlen_fixed_part >= prefixlen)
                        ঘূৰাই -EINVAL;

                random_bytes(&n, পরিমাণ(n));

                বাবে (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        যদি (i < (prefixlen_fixed_part + 7) / 8) {
                                যদি (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                অন্যথা {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        যদি (i < (prefixlen + 7) / 8) {
                                যদি (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                অন্যথা {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                ঘূৰাই 1;
        }

        ঘূৰাই -EAFNOSUPPORT;
}

পূর্ণাঙ্ক in_addr_prefix_range(
                পূর্ণাঙ্ক family,
                ধ্রুৱ সঙ্ঘ in_addr_union *in,
                unsigned prefixlen,
                সঙ্ঘ in_addr_union *ret_start,
                সঙ্ঘ in_addr_union *ret_end) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        ৱিকল্প (family) {
        পক্ষ AF_INET:
                যদি (prefixlen > 32)
                        ঘূৰাই -EINVAL;
                ভঙ্গ;
        পক্ষ AF_INET6:
                যদি (prefixlen > 128)
                        ঘূৰাই -EINVAL;
                ভঙ্গ;
        স্ৱভাৱ:
                ঘূৰাই -EAFNOSUPPORT;
        }

        যদি (prefixlen == 0) {
                /* shortcut for trivial case */
                যদি (ret_start)
                        *ret_start = IN_ADDR_NULL;
                যদি (ret_end)
                        *ret_end = IN_ADDR_NULL;
                ঘূৰাই 0;
        }

        সঙ্ঘ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        যদি (r < 0)
                ঘূৰাই r;

        সঙ্ঘ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        বাবে (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                যদি (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } অন্যথা {
                        end.bytes[i] += add;
                        ভঙ্গ;
                }

        যদি (ret_start)
                *ret_start = start;
        যদি (ret_end)
                *ret_end = end;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক in_addr_to_string(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *u, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *x = রিক্ত;
        size_t l;

        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(ret);

        যদি (family == AF_INET)
                l = INET_ADDRSTRLEN;
        অন্যথা যদি (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        অন্যথা
                ঘূৰাই -EAFNOSUPPORT;

        x = new(ৱর্ণ, l);
        যদি (!x)
                ঘূৰাই -ENOMEM;

        errno = 0;
        যদি (!typesafe_inet_ntop(family, u, x, l))
                ঘূৰাই errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ঘূৰাই 0;
}

ধ্রুৱ ৱর্ণ* typesafe_inet_ntop(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *a, ৱর্ণ *buf, size_t len) {
        ঘূৰাই inet_ntop(family, a, buf, len);
}

ধ্রুৱ ৱর্ণ* typesafe_inet_ntop4(ধ্রুৱ সংরচনা in_addr *a, ৱর্ণ *buf, size_t len) {
        ঘূৰাই inet_ntop(AF_INET, a, buf, len);
}

ধ্রুৱ ৱর্ণ* typesafe_inet_ntop6(ধ্রুৱ সংরচনা in6_addr *a, ৱর্ণ *buf, size_t len) {
        ঘূৰাই inet_ntop(AF_INET6, a, buf, len);
}

পূর্ণাঙ্ক in_addr_prefix_to_string(
                পূর্ণাঙ্ক family,
                ধ্রুৱ সঙ্ঘ in_addr_union *u,
                unsigned prefixlen,
                ৱর্ণ *buf,
                size_t buf_len) {

        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(buf);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ঘূৰাই -EAFNOSUPPORT;

        errno = 0;
        যদি (!typesafe_inet_ntop(family, u, buf, buf_len))
                ঘূৰাই errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        যদি (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ঘূৰাই -ENOSPC;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক in_addr_port_ifindex_name_to_string(পূর্ণাঙ্ক family, ধ্রুৱ সঙ্ঘ in_addr_union *u, uint16_t port, পূর্ণাঙ্ক ifindex, ধ্রুৱ ৱর্ণ *server_name, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *ip_str = রিক্ত, *x = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(IN_SET(family, AF_INET, AF_INET6));
        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        ifindex = 0;
        } অন্যথা
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        যদি (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ঘূৰাই 0;
        }

        ধ্রুৱ ৱর্ণ *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        যদি (port > 0) {
                যদি (family == AF_INET6) {
                        যদি (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        অন্যথা
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } অন্যথা
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } অন্যথা {
                যদি (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                অন্যথা
                        x = strjoin(ip_str, separator, server_name);
        }
        যদি (!x)
                ঘূৰাই -ENOMEM;

        *ret = TAKE_PTR(x);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক in_addr_from_string(পূর্ণাঙ্ক family, ধ্রুৱ ৱর্ণ *s, সঙ্ঘ in_addr_union *ret) {
        সঙ্ঘ in_addr_union buffer;
        প্রতিজ্ঞা(s);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ঘূৰাই -EAFNOSUPPORT;

        errno = 0;
        যদি (inet_pton(family, s, ret ?: &buffer) <= 0)
                ঘূৰাই errno_or_else(EINVAL);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক in_addr_from_string_auto(ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক *ret_family, সঙ্ঘ in_addr_union *ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        r = in_addr_from_string(AF_INET, s, ret);
        যদি (r >= 0) {
                যদি (ret_family)
                        *ret_family = AF_INET;
                ঘূৰাই 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        যদি (r >= 0) {
                যদি (ret_family)
                        *ret_family = AF_INET6;
                ঘূৰাই 0;
        }

        ঘূৰাই -EINVAL;
}

unsigned ৱর্ণ in4_addr_netmask_to_prefixlen(ধ্রুৱ সংরচনা in_addr *addr) {
        প্রতিজ্ঞা(addr);

        ঘূৰাই 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
সংরচনা in_addr* in4_addr_prefixlen_to_netmask(সংরচনা in_addr *addr, unsigned ৱর্ণ prefixlen) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        যদি (prefixlen == 0)
                addr->s_addr = 0;
        অন্যথা
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ঘূৰাই addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
সংরচনা in6_addr* in6_addr_prefixlen_to_netmask(সংরচনা in6_addr *addr, unsigned ৱর্ণ prefixlen) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(prefixlen <= 128);

        বাবে (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                যদি (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } অন্যথা যদি (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } অন্যথা {
                        প্রতিজ্ঞা(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        ঘূৰাই addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
পূর্ণাঙ্ক in_addr_prefixlen_to_netmask(পূর্ণাঙ্ক family, সঙ্ঘ in_addr_union *addr, unsigned ৱর্ণ prefixlen) {
        প্রতিজ্ঞা(addr);

        ৱিকল্প (family) {
        পক্ষ AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                ঘূৰাই 0;
        পক্ষ AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                ঘূৰাই 0;
        স্ৱভাৱ:
                ঘূৰাই -EAFNOSUPPORT;
        }
}

পূর্ণাঙ্ক in4_addr_default_prefixlen(ধ্রুৱ সংরচনা in_addr *addr, unsigned ৱর্ণ *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(prefixlen);

        যদি (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        অন্যথা যদি (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        অন্যথা যদি (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        অন্যথা
                /* class D or E, no default prefixlen */
                ঘূৰাই -ERANGE;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক in4_addr_default_subnet_mask(ধ্রুৱ সংরচনা in_addr *addr, সংরচনা in_addr *mask) {
        unsigned ৱর্ণ prefixlen;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        যদি (r < 0)
                ঘূৰাই r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক in4_addr_mask(সংরচনা in_addr *addr, unsigned ৱর্ণ prefixlen) {
        সংরচনা in_addr mask;

        প্রতিজ্ঞা(addr);

        যদি (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                ঘূৰাই -EINVAL;

        addr->s_addr &= mask.s_addr;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক in6_addr_mask(সংরচনা in6_addr *addr, unsigned ৱর্ণ prefixlen) {
        unsigned i;

        প্রতিজ্ঞা(addr);

        বাবে (i = 0; i < 16; i++) {
                uint8_t mask;

                যদি (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } অন্যথা যদি (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } অন্যথা {
                        প্রতিজ্ঞা(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক in_addr_mask(পূর্ণাঙ্ক family, সঙ্ঘ in_addr_union *addr, unsigned ৱর্ণ prefixlen) {
        প্রতিজ্ঞা(addr);

        ৱিকল্প (family) {
        পক্ষ AF_INET:
                ঘূৰাই in4_addr_mask(&addr->in, prefixlen);
        পক্ষ AF_INET6:
                ঘূৰাই in6_addr_mask(&addr->in6, prefixlen);
        স্ৱভাৱ:
                ঘূৰাই -EAFNOSUPPORT;
        }
}

পূর্ণাঙ্ক in4_addr_prefix_covers_full(
                ধ্রুৱ সংরচনা in_addr *prefix,
                unsigned ৱর্ণ prefixlen,
                ধ্রুৱ সংরচনা in_addr *address,
                unsigned ৱর্ণ address_prefixlen) {

        সংরচনা in_addr masked_prefix, masked_address;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(address);

        যদি (prefixlen > address_prefixlen)
                ঘূৰাই false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        যদি (r < 0)
                ঘূৰাই r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই in4_addr_equal(&masked_prefix, &masked_address);
}

পূর্ণাঙ্ক in6_addr_prefix_covers_full(
                ধ্রুৱ সংরচনা in6_addr *prefix,
                unsigned ৱর্ণ prefixlen,
                ধ্রুৱ সংরচনা in6_addr *address,
                unsigned ৱর্ণ address_prefixlen) {

        সংরচনা in6_addr masked_prefix, masked_address;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(address);

        যদি (prefixlen > address_prefixlen)
                ঘূৰাই false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        যদি (r < 0)
                ঘূৰাই r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই in6_addr_equal(&masked_prefix, &masked_address);
}

পূর্ণাঙ্ক in_addr_prefix_covers_full(
                পূর্ণাঙ্ক family,
                ধ্রুৱ সঙ্ঘ in_addr_union *prefix,
                unsigned ৱর্ণ prefixlen,
                ধ্রুৱ সঙ্ঘ in_addr_union *address,
                unsigned ৱর্ণ address_prefixlen) {

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(address);

        ৱিকল্প (family) {
        পক্ষ AF_INET:
                ঘূৰাই in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        পক্ষ AF_INET6:
                ঘূৰাই in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        স্ৱভাৱ:
                ঘূৰাই -EAFNOSUPPORT;
        }
}

পূর্ণাঙ্ক in_addr_parse_prefixlen(পূর্ণাঙ্ক family, ধ্রুৱ ৱর্ণ *p, unsigned ৱর্ণ *ret) {
        uint8_t u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ঘূৰাই -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (u > FAMILY_ADDRESS_SIZE(family) * 8)
                ঘূৰাই -ERANGE;

        *ret = u;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক in_addr_prefix_from_string(
                ধ্রুৱ ৱর্ণ *p,
                পূর্ণাঙ্ক family,
                সঙ্ঘ in_addr_union *ret_prefix,
                unsigned ৱর্ণ *ret_prefixlen) {

        _cleanup_free_ ৱর্ণ *str = রিক্ত;
        সঙ্ঘ in_addr_union buffer;
        ধ্রুৱ ৱর্ণ *e, *l;
        unsigned ৱর্ণ k;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ঘূৰাই -EAFNOSUPPORT;

        e = strchr(p, '/');
        যদি (e) {
                str = strndup(p, e - p);
                যদি (!str)
                        ঘূৰাই -ENOMEM;

                l = str;
        } অন্যথা
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                যদি (r < 0)
                        ঘূৰাই r;
        } অন্যথা
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        যদি (ret_prefix)
                *ret_prefix = buffer;
        যদি (ret_prefixlen)
                *ret_prefixlen = k;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক in_addr_prefix_from_string_auto_full(
                ধ্রুৱ ৱর্ণ *p,
                InAddrPrefixLenMode mode,
                পূর্ণাঙ্ক *ret_family,
                সঙ্ঘ in_addr_union *ret_prefix,
                unsigned ৱর্ণ *ret_prefixlen) {

        _cleanup_free_ ৱর্ণ *str = রিক্ত;
        সঙ্ঘ in_addr_union buffer;
        ধ্রুৱ ৱর্ণ *e, *l;
        unsigned ৱর্ণ k;
        পূর্ণাঙ্ক family, r;

        প্রতিজ্ঞা(p);

        e = strchr(p, '/');
        যদি (e) {
                str = strndup(p, e - p);
                যদি (!str)
                        ঘূৰাই -ENOMEM;

                l = str;
        } অন্যথা
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                যদি (r < 0)
                        ঘূৰাই r;
        } অন্যথা
                ৱিকল্প (mode) {
                পক্ষ PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        ভঙ্গ;
                পক্ষ PREFIXLEN_REFUSE:
                        ঘূৰাই -ENOANO; /* To distinguish this error from others. */
                স্ৱভাৱ:
                        assert_not_reached();
                }

        যদি (ret_family)
                *ret_family = family;
        যদি (ret_prefix)
                *ret_prefix = buffer;
        যদি (ret_prefixlen)
                *ret_prefixlen = k;

        ঘূৰাই 0;
}

শূন্য in_addr_hash_func(ধ্রুৱ সঙ্ঘ in_addr_union *u, পূর্ণাঙ্ক family, সংরচনা siphash *state) {
        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

শূন্য in_addr_data_hash_func(ধ্রুৱ সংরচনা in_addr_data *a, সংরচনা siphash *state) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

পূর্ণাঙ্ক in_addr_data_compare_func(ধ্রুৱ সংরচনা in_addr_data *x, ধ্রুৱ সংরচনা in_addr_data *y) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(y);

        r = CMP(x->family, y->family);
        যদি (r != 0)
                ঘূৰাই r;

        ঘূৰাই memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        সংরচনা in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        সংরচনা in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        মুক্ত);

শূন্য in6_addr_hash_func(ধ্রুৱ সংরচনা in6_addr *addr, সংরচনা siphash *state) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(state);

        siphash24_compress_typesafe(*addr, state);
}

পূর্ণাঙ্ক in6_addr_compare_func(ধ্রুৱ সংরচনা in6_addr *a, ধ্রুৱ সংরচনা in6_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ঘূৰাই memcmp(a, b, পরিমাণ(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        সংরচনা in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(
        in6_addr_hash_ops_free,
        সংরচনা in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        মুক্ত);
