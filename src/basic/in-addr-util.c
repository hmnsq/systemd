/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <arpa/inet.h>
#অন্তর্ভাব <endian.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "hash-funcs.h"
#অন্তর্ভাব "in-addr-util.h"
#অন্তর্ভাব "logarithm.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "random-util.h"
#অন্তর্ভাব "siphash24.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"

bool in4_addr_is_null(ধ্রুবক কাঠামো in_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত a->s_addr == 0;
}

bool in6_addr_is_null(ধ্রুবক কাঠামো in6_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত eqzero(a->s6_addr32);
}

পূর্ণসংখ্যা in_addr_is_null(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ফেরত in4_addr_is_null(&u->in);

        যদি (family == AF_INET6)
                ফেরত in6_addr_is_null(&u->in6);

        ফেরত -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ধ্রুবক কাঠামো in_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ধ্রুবক কাঠামো in_addr *a) {
        প্রতিজ্ঞা(a);

        যদি (!in4_addr_is_link_local(a))
                ফেরত false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ফেরত !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ধ্রুবক কাঠামো in6_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

পূর্ণসংখ্যা in_addr_is_link_local(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ফেরত in4_addr_is_link_local(&u->in);

        যদি (family == AF_INET6)
                ফেরত in6_addr_is_link_local(&u->in6);

        ফেরত -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ধ্রুবক কাঠামো in6_addr *a) {
        প্রতিজ্ঞা(a);

        /* ff02::1 */
        ফেরত be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ধ্রুবক কাঠামো in_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ধ্রুবক কাঠামো in6_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত a->s6_addr[0] == 0xff;
}

পূর্ণসংখ্যা in_addr_is_multicast(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ফেরত in4_addr_is_multicast(&u->in);

        যদি (family == AF_INET6)
                ফেরত in6_addr_is_multicast(&u->in6);

        ফেরত -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ধ্রুবক কাঠামো in_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ধ্রুবক কাঠামো in_addr *a) {
        প্রতিজ্ঞা(a);

        /* All of 127.x.x.x is localhost. */
        ফেরত (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ধ্রুবক কাঠামো in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ফেরত !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

স্থির bool in6_addr_is_loopback(ধ্রুবক কাঠামো in6_addr *a) {
        ফেরত memcmp(a, &(কাঠামো in6_addr) IN6ADDR_LOOPBACK_INIT, আকার(কাঠামো in6_addr)) == 0;
}

পূর্ণসংখ্যা in_addr_is_localhost(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                ফেরত in4_addr_is_localhost(&u->in);

        যদি (family == AF_INET6)
                ফেরত in6_addr_is_loopback(&u->in6);

        ফেরত -EAFNOSUPPORT;
}

পূর্ণসংখ্যা in_addr_is_localhost_one(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u) {
        প্রতিজ্ঞা(u);

        যদি (family == AF_INET)
                /* 127.0.0.1 */
                ফেরত be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        যদি (family == AF_INET6)
                ফেরত in6_addr_is_loopback(&u->in6);

        ফেরত -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(ধ্রুবক কাঠামো in6_addr *a) {
        প্রতিজ্ঞা(a);

        ফেরত a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ধ্রুবক কাঠামো in_addr *a, ধ্রুবক কাঠামো in_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ফেরত a->s_addr == b->s_addr;
}

bool in6_addr_equal(ধ্রুবক কাঠামো in6_addr *a, ধ্রুবক কাঠামো in6_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ফেরত memcmp(a, b, আকার(কাঠামো in6_addr)) == 0;
}

পূর্ণসংখ্যা in_addr_equal(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *a, ধ্রুবক সংঘ in_addr_union *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (family == AF_INET)
                ফেরত in4_addr_equal(&a->in, &b->in);

        যদি (family == AF_INET6)
                ফেরত in6_addr_equal(&a->in6, &b->in6);

        ফেরত -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                ধ্রুবক কাঠামো in_addr *a,
                unsigned aprefixlen,
                ধ্রুবক কাঠামো in_addr *b,
                unsigned bprefixlen) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (আকার(কাঠামো in_addr) * 8));
        যদি (m == 0)
                ফেরত true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ফেরত (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                ধ্রুবক কাঠামো in6_addr *a,
                unsigned aprefixlen,
                ধ্রুবক কাঠামো in6_addr *b,
                unsigned bprefixlen) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (আকার(কাঠামো in6_addr) * 8));
        যদি (m == 0)
                ফেরত true;

        জন্য (size_t i = 0; i < আকার(কাঠামো in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                যদি ((x & n) != 0)
                        ফেরত false;

                যদি (m <= 8)
                        বিরতি;

                m -= 8;
        }

        ফেরত true;
}

পূর্ণসংখ্যা in_addr_prefix_intersect(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *a,
                unsigned aprefixlen,
                ধ্রুবক সংঘ in_addr_union *b,
                unsigned bprefixlen) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        /* Checks whether there are any addresses that are in both networks. */

        যদি (family == AF_INET)
                ফেরত in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        যদি (family == AF_INET6)
                ফেরত in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ফেরত -EAFNOSUPPORT;
}

পূর্ণসংখ্যা in_addr_random_prefix(
                পূর্ণসংখ্যা family,
                সংঘ in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        প্রতিজ্ঞা(u);

        /* Random network part of an address by one. */

        যদি (prefixlen <= 0)
                ফেরত 0;

        যদি (family == AF_INET) {
                uint32_t c, n;

                যদি (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                যদি (prefixlen > 32)
                        prefixlen = 32;
                যদি (prefixlen_fixed_part >= prefixlen)
                        ফেরত -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, আকার(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                ফেরত 1;
        }

        যদি (family == AF_INET6) {
                কাঠামো in6_addr n;
                unsigned i, j;

                যদি (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                যদি (prefixlen > 128)
                        prefixlen = 128;
                যদি (prefixlen_fixed_part >= prefixlen)
                        ফেরত -EINVAL;

                random_bytes(&n, আকার(n));

                জন্য (i = 0; i < 16; i++) {
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

                ফেরত 1;
        }

        ফেরত -EAFNOSUPPORT;
}

পূর্ণসংখ্যা in_addr_prefix_range(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *in,
                unsigned prefixlen,
                সংঘ in_addr_union *ret_start,
                সংঘ in_addr_union *ret_end) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        নির্বাচন (family) {
        ক্ষেত্র AF_INET:
                যদি (prefixlen > 32)
                        ফেরত -EINVAL;
                বিরতি;
        ক্ষেত্র AF_INET6:
                যদি (prefixlen > 128)
                        ফেরত -EINVAL;
                বিরতি;
        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }

        যদি (prefixlen == 0) {
                /* shortcut for trivial case */
                যদি (ret_start)
                        *ret_start = IN_ADDR_NULL;
                যদি (ret_end)
                        *ret_end = IN_ADDR_NULL;
                ফেরত 0;
        }

        সংঘ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        যদি (r < 0)
                ফেরত r;

        সংঘ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_আকার(family) * 8 - prefixlen) % 8);
        জন্য (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                যদি (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } অন্যথা {
                        end.bytes[i] += add;
                        বিরতি;
                }

        যদি (ret_start)
                *ret_start = start;
        যদি (ret_end)
                *ret_end = end;

        ফেরত 0;
}

পূর্ণসংখ্যা in_addr_to_string(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *x = ফাঁকা;
        size_t l;

        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(ret);

        যদি (family == AF_INET)
                l = INET_ADDRSTRLEN;
        অন্যথা যদি (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        অন্যথা
                ফেরত -EAFNOSUPPORT;

        x = new(অক্ষর, l);
        যদি (!x)
                ফেরত -ENOMEM;

        errno = 0;
        যদি (!typesafe_inet_ntop(family, u, x, l))
                ফেরত errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ফেরত 0;
}

ধ্রুবক অক্ষর* typesafe_inet_ntop(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *a, অক্ষর *buf, size_t len) {
        ফেরত inet_ntop(family, a, buf, len);
}

ধ্রুবক অক্ষর* typesafe_inet_ntop4(ধ্রুবক কাঠামো in_addr *a, অক্ষর *buf, size_t len) {
        ফেরত inet_ntop(AF_INET, a, buf, len);
}

ধ্রুবক অক্ষর* typesafe_inet_ntop6(ধ্রুবক কাঠামো in6_addr *a, অক্ষর *buf, size_t len) {
        ফেরত inet_ntop(AF_INET6, a, buf, len);
}

পূর্ণসংখ্যা in_addr_prefix_to_string(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *u,
                unsigned prefixlen,
                অক্ষর *buf,
                size_t buf_len) {

        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(buf);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ফেরত -EAFNOSUPPORT;

        errno = 0;
        যদি (!typesafe_inet_ntop(family, u, buf, buf_len))
                ফেরত errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        যদি (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ফেরত -ENOSPC;
        ফেরত 0;
}

পূর্ণসংখ্যা in_addr_port_ifindex_name_to_string(পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *u, uint16_t port, পূর্ণসংখ্যা ifindex, ধ্রুবক অক্ষর *server_name, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *ip_str = ফাঁকা, *x = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(IN_SET(family, AF_INET, AF_INET6));
        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        যদি (r < 0)
                ফেরত r;

        যদি (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ifindex = 0;
        } অন্যথা
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        যদি (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ফেরত 0;
        }

        ধ্রুবক অক্ষর *separator = isempty(server_name) ? "" : "#";
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
                ফেরত -ENOMEM;

        *ret = TAKE_PTR(x);
        ফেরত 0;
}

পূর্ণসংখ্যা in_addr_from_string(পূর্ণসংখ্যা family, ধ্রুবক অক্ষর *s, সংঘ in_addr_union *ret) {
        সংঘ in_addr_union বাফার;
        প্রতিজ্ঞা(s);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ফেরত -EAFNOSUPPORT;

        errno = 0;
        যদি (inet_pton(family, s, ret ?: &বাফার) <= 0)
                ফেরত errno_or_else(EINVAL);

        ফেরত 0;
}

পূর্ণসংখ্যা in_addr_from_string_auto(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret_family, সংঘ in_addr_union *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        r = in_addr_from_string(AF_INET, s, ret);
        যদি (r >= 0) {
                যদি (ret_family)
                        *ret_family = AF_INET;
                ফেরত 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        যদি (r >= 0) {
                যদি (ret_family)
                        *ret_family = AF_INET6;
                ফেরত 0;
        }

        ফেরত -EINVAL;
}

unsigned অক্ষর in4_addr_netmask_to_prefixlen(ধ্রুবক কাঠামো in_addr *addr) {
        প্রতিজ্ঞা(addr);

        ফেরত 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
কাঠামো in_addr* in4_addr_prefixlen_to_netmask(কাঠামো in_addr *addr, unsigned অক্ষর prefixlen) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        যদি (prefixlen == 0)
                addr->s_addr = 0;
        অন্যথা
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ফেরত addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
কাঠামো in6_addr* in6_addr_prefixlen_to_netmask(কাঠামো in6_addr *addr, unsigned অক্ষর prefixlen) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(prefixlen <= 128);

        জন্য (unsigned i = 0; i < 16; i++) {
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

        ফেরত addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
পূর্ণসংখ্যা in_addr_prefixlen_to_netmask(পূর্ণসংখ্যা family, সংঘ in_addr_union *addr, unsigned অক্ষর prefixlen) {
        প্রতিজ্ঞা(addr);

        নির্বাচন (family) {
        ক্ষেত্র AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                ফেরত 0;
        ক্ষেত্র AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                ফেরত 0;
        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

পূর্ণসংখ্যা in4_addr_default_prefixlen(ধ্রুবক কাঠামো in_addr *addr, unsigned অক্ষর *prefixlen) {
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
                ফেরত -ERANGE;

        ফেরত 0;
}

পূর্ণসংখ্যা in4_addr_default_subnet_mask(ধ্রুবক কাঠামো in_addr *addr, কাঠামো in_addr *mask) {
        unsigned অক্ষর prefixlen;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        যদি (r < 0)
                ফেরত r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        ফেরত 0;
}

পূর্ণসংখ্যা in4_addr_mask(কাঠামো in_addr *addr, unsigned অক্ষর prefixlen) {
        কাঠামো in_addr mask;

        প্রতিজ্ঞা(addr);

        যদি (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                ফেরত -EINVAL;

        addr->s_addr &= mask.s_addr;
        ফেরত 0;
}

পূর্ণসংখ্যা in6_addr_mask(কাঠামো in6_addr *addr, unsigned অক্ষর prefixlen) {
        unsigned i;

        প্রতিজ্ঞা(addr);

        জন্য (i = 0; i < 16; i++) {
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

        ফেরত 0;
}

পূর্ণসংখ্যা in_addr_mask(পূর্ণসংখ্যা family, সংঘ in_addr_union *addr, unsigned অক্ষর prefixlen) {
        প্রতিজ্ঞা(addr);

        নির্বাচন (family) {
        ক্ষেত্র AF_INET:
                ফেরত in4_addr_mask(&addr->in, prefixlen);
        ক্ষেত্র AF_INET6:
                ফেরত in6_addr_mask(&addr->in6, prefixlen);
        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

পূর্ণসংখ্যা in4_addr_prefix_covers_full(
                ধ্রুবক কাঠামো in_addr *prefix,
                unsigned অক্ষর prefixlen,
                ধ্রুবক কাঠামো in_addr *address,
                unsigned অক্ষর address_prefixlen) {

        কাঠামো in_addr masked_prefix, masked_address;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(address);

        যদি (prefixlen > address_prefixlen)
                ফেরত false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        যদি (r < 0)
                ফেরত r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        যদি (r < 0)
                ফেরত r;

        ফেরত in4_addr_equal(&masked_prefix, &masked_address);
}

পূর্ণসংখ্যা in6_addr_prefix_covers_full(
                ধ্রুবক কাঠামো in6_addr *prefix,
                unsigned অক্ষর prefixlen,
                ধ্রুবক কাঠামো in6_addr *address,
                unsigned অক্ষর address_prefixlen) {

        কাঠামো in6_addr masked_prefix, masked_address;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(address);

        যদি (prefixlen > address_prefixlen)
                ফেরত false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        যদি (r < 0)
                ফেরত r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        যদি (r < 0)
                ফেরত r;

        ফেরত in6_addr_equal(&masked_prefix, &masked_address);
}

পূর্ণসংখ্যা in_addr_prefix_covers_full(
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *prefix,
                unsigned অক্ষর prefixlen,
                ধ্রুবক সংঘ in_addr_union *address,
                unsigned অক্ষর address_prefixlen) {

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(address);

        নির্বাচন (family) {
        ক্ষেত্র AF_INET:
                ফেরত in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        ক্ষেত্র AF_INET6:
                ফেরত in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

পূর্ণসংখ্যা in_addr_parse_prefixlen(পূর্ণসংখ্যা family, ধ্রুবক অক্ষর *p, unsigned অক্ষর *ret) {
        uint8_t u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ফেরত -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        যদি (r < 0)
                ফেরত r;

        যদি (u > FAMILY_ADDRESS_আকার(family) * 8)
                ফেরত -ERANGE;

        *ret = u;
        ফেরত 0;
}

পূর্ণসংখ্যা in_addr_prefix_from_string(
                ধ্রুবক অক্ষর *p,
                পূর্ণসংখ্যা family,
                সংঘ in_addr_union *ret_prefix,
                unsigned অক্ষর *ret_prefixlen) {

        _cleanup_free_ অক্ষর *str = ফাঁকা;
        সংঘ in_addr_union বাফার;
        ধ্রুবক অক্ষর *e, *l;
        unsigned অক্ষর k;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(p);

        যদি (!IN_SET(family, AF_INET, AF_INET6))
                ফেরত -EAFNOSUPPORT;

        e = strchr(p, '/');
        যদি (e) {
                str = strndup(p, e - p);
                যদি (!str)
                        ফেরত -ENOMEM;

                l = str;
        } অন্যথা
                l = p;

        r = in_addr_from_string(family, l, &বাফার);
        যদি (r < 0)
                ফেরত r;

        যদি (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                যদি (r < 0)
                        ফেরত r;
        } অন্যথা
                k = FAMILY_ADDRESS_আকার(family) * 8;

        যদি (ret_prefix)
                *ret_prefix = বাফার;
        যদি (ret_prefixlen)
                *ret_prefixlen = k;

        ফেরত 0;
}

পূর্ণসংখ্যা in_addr_prefix_from_string_auto_full(
                ধ্রুবক অক্ষর *p,
                InAddrPrefixLenMode mode,
                পূর্ণসংখ্যা *ret_family,
                সংঘ in_addr_union *ret_prefix,
                unsigned অক্ষর *ret_prefixlen) {

        _cleanup_free_ অক্ষর *str = ফাঁকা;
        সংঘ in_addr_union বাফার;
        ধ্রুবক অক্ষর *e, *l;
        unsigned অক্ষর k;
        পূর্ণসংখ্যা family, r;

        প্রতিজ্ঞা(p);

        e = strchr(p, '/');
        যদি (e) {
                str = strndup(p, e - p);
                যদি (!str)
                        ফেরত -ENOMEM;

                l = str;
        } অন্যথা
                l = p;

        r = in_addr_from_string_auto(l, &family, &বাফার);
        যদি (r < 0)
                ফেরত r;

        যদি (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                যদি (r < 0)
                        ফেরত r;
        } অন্যথা
                নির্বাচন (mode) {
                ক্ষেত্র PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_আকার(family) * 8;
                        বিরতি;
                ক্ষেত্র PREFIXLEN_REFUSE:
                        ফেরত -ENOANO; /* To distinguish this error from others. */
                পূর্বনির্ধারিত:
                        assert_not_reached();
                }

        যদি (ret_family)
                *ret_family = family;
        যদি (ret_prefix)
                *ret_prefix = বাফার;
        যদি (ret_prefixlen)
                *ret_prefixlen = k;

        ফেরত 0;
}

শূন্য in_addr_hash_func(ধ্রুবক সংঘ in_addr_union *u, পূর্ণসংখ্যা family, কাঠামো siphash *state) {
        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_আকার(family), state);
}

শূন্য in_addr_তথ্য_hash_func(ধ্রুবক কাঠামো in_addr_তথ্য *a, কাঠামো siphash *state) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

পূর্ণসংখ্যা in_addr_তথ্য_compare_func(ধ্রুবক কাঠামো in_addr_তথ্য *x, ধ্রুবক কাঠামো in_addr_তথ্য *y) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(y);

        r = CMP(x->family, y->family);
        যদি (r != 0)
                ফেরত r;

        ফেরত memcmp(&x->address, &y->address, FAMILY_ADDRESS_আকার(x->family));
}

DEFINE_HASH_OPS(
        in_addr_তথ্য_hash_ops,
        কাঠামো in_addr_তথ্য,
        in_addr_তথ্য_hash_func,
        in_addr_তথ্য_compare_func);

DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(
        in_addr_তথ্য_hash_ops_free,
        কাঠামো in_addr_তথ্য,
        in_addr_তথ্য_hash_func,
        in_addr_তথ্য_compare_func,
        মুক্ত);

শূন্য in6_addr_hash_func(ধ্রুবক কাঠামো in6_addr *addr, কাঠামো siphash *state) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(state);

        siphash24_compress_typesafe(*addr, state);
}

পূর্ণসংখ্যা in6_addr_compare_func(ধ্রুবক কাঠামো in6_addr *a, ধ্রুবক কাঠামো in6_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ফেরত memcmp(a, b, আকার(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        কাঠামো in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(
        in6_addr_hash_ops_free,
        কাঠামো in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        মুক্ত);
