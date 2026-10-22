/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಅನ್ತರ್ಭಾವ <endian.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "hash-funcs.h"
#ಅನ್ತರ್ಭಾವ "in-addr-util.h"
#ಅನ್ತರ್ಭಾವ "logarithm.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "random-util.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

bool in4_addr_is_null(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ a->s_addr == 0;
}

bool in6_addr_is_null(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ eqzero(a->s6_addr32);
}

ಪೂರ್ಣಾಙ್ಕ in_addr_is_null(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಯದಿ (family == AF_INET)
                ಪ್ರತ್ಯಾಗಮ in4_addr_is_null(&u->in);

        ಯದಿ (family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ in6_addr_is_null(&u->in6);

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಯದಿ (!in4_addr_is_link_local(a))
                ಪ್ರತ್ಯಾಗಮ false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ಪ್ರತ್ಯಾಗಮ !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

ಪೂರ್ಣಾಙ್ಕ in_addr_is_link_local(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಯದಿ (family == AF_INET)
                ಪ್ರತ್ಯಾಗಮ in4_addr_is_link_local(&u->in);

        ಯದಿ (family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ in6_addr_is_link_local(&u->in6);

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        /* ff02::1 */
        ಪ್ರತ್ಯಾಗಮ be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ a->s6_addr[0] == 0xff;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_is_multicast(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಯದಿ (family == AF_INET)
                ಪ್ರತ್ಯಾಗಮ in4_addr_is_multicast(&u->in);

        ಯದಿ (family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ in6_addr_is_multicast(&u->in6);

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        /* All of 127.x.x.x is localhost. */
        ಪ್ರತ್ಯಾಗಮ (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ಧ್ರುವ ಸಂರಚನಾ in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ಪ್ರತ್ಯಾಗಮ !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ಸ್ಥಿರ bool in6_addr_is_loopback(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) {
        ಪ್ರತ್ಯಾಗಮ memcmp(a, &(ಸಂರಚನಾ in6_addr) IN6ADDR_LOOPBACK_INIT, ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr)) == 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_is_localhost(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಯದಿ (family == AF_INET)
                ಪ್ರತ್ಯಾಗಮ in4_addr_is_localhost(&u->in);

        ಯದಿ (family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ in6_addr_is_loopback(&u->in6);

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_is_localhost_one(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಯದಿ (family == AF_INET)
                /* 127.0.0.1 */
                ಪ್ರತ್ಯಾಗಮ be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        ಯದಿ (family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ in6_addr_is_loopback(&u->in6);

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ಧ್ರುವ ಸಂರಚನಾ in_addr *a, ಧ್ರುವ ಸಂರಚನಾ in_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ a->s_addr == b->s_addr;
}

bool in6_addr_equal(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a, ಧ್ರುವ ಸಂರಚನಾ in6_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ memcmp(a, b, ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr)) == 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_equal(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *a, ಧ್ರುವ ಸಙ್ಘ in_addr_union *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಯದಿ (family == AF_INET)
                ಪ್ರತ್ಯಾಗಮ in4_addr_equal(&a->in, &b->in);

        ಯದಿ (family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ in6_addr_equal(&a->in6, &b->in6);

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                ಧ್ರುವ ಸಂರಚನಾ in_addr *a,
                unsigned aprefixlen,
                ಧ್ರುವ ಸಂರಚನಾ in_addr *b,
                unsigned bprefixlen) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr) * 8));
        ಯದಿ (m == 0)
                ಪ್ರತ್ಯಾಗಮ true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ಪ್ರತ್ಯಾಗಮ (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                ಧ್ರುವ ಸಂರಚನಾ in6_addr *a,
                unsigned aprefixlen,
                ಧ್ರುವ ಸಂರಚನಾ in6_addr *b,
                unsigned bprefixlen) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr) * 8));
        ಯದಿ (m == 0)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತಿ (size_t i = 0; i < ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                ಯದಿ ((x & n) != 0)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (m <= 8)
                        ಭಙ್ಗ;

                m -= 8;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_intersect(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *a,
                unsigned aprefixlen,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *b,
                unsigned bprefixlen) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        /* Checks whether there are any addresses that are in both networks. */

        ಯದಿ (family == AF_INET)
                ಪ್ರತ್ಯಾಗಮ in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        ಯದಿ (family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_random_prefix(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಸಙ್ಘ in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        ಪ್ರತಿಜ್ಞಾ(u);

        /* Random network part of an address by one. */

        ಯದಿ (prefixlen <= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (family == AF_INET) {
                uint32_t c, n;

                ಯದಿ (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                ಯದಿ (prefixlen > 32)
                        prefixlen = 32;
                ಯದಿ (prefixlen_fixed_part >= prefixlen)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, ಪರಿಮಾಣ(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                ಪ್ರತ್ಯಾಗಮ 1;
        }

        ಯದಿ (family == AF_INET6) {
                ಸಂರಚನಾ in6_addr n;
                unsigned i, j;

                ಯದಿ (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                ಯದಿ (prefixlen > 128)
                        prefixlen = 128;
                ಯದಿ (prefixlen_fixed_part >= prefixlen)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                random_bytes(&n, ಪರಿಮಾಣ(n));

                ಪ್ರತಿ (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        ಯದಿ (i < (prefixlen_fixed_part + 7) / 8) {
                                ಯದಿ (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                ಅನ್ಯಥಾ {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        ಯದಿ (i < (prefixlen + 7) / 8) {
                                ಯದಿ (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                ಅನ್ಯಥಾ {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                ಪ್ರತ್ಯಾಗಮ 1;
        }

        ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_range(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *in,
                unsigned prefixlen,
                ಸಙ್ಘ in_addr_union *ret_start,
                ಸಙ್ಘ in_addr_union *ret_end) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        ವಿಕಲ್ಪ (family) {
        ಪಕ್ಷ AF_INET:
                ಯದಿ (prefixlen > 32)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                ಭಙ್ಗ;
        ಪಕ್ಷ AF_INET6:
                ಯದಿ (prefixlen > 128)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                ಭಙ್ಗ;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }

        ಯದಿ (prefixlen == 0) {
                /* shortcut for trivial case */
                ಯದಿ (ret_start)
                        *ret_start = IN_ADDR_NULL;
                ಯದಿ (ret_end)
                        *ret_end = IN_ADDR_NULL;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಸಙ್ಘ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಸಙ್ಘ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        ಪ್ರತಿ (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                ಯದಿ (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } ಅನ್ಯಥಾ {
                        end.bytes[i] += add;
                        ಭಙ್ಗ;
                }

        ಯದಿ (ret_start)
                *ret_start = start;
        ಯದಿ (ret_end)
                *ret_end = end;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_to_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *x = ರಿಕ್ತ;
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (family == AF_INET)
                l = INET_ADDRSTRLEN;
        ಅನ್ಯಥಾ ಯದಿ (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        x = new(ವರ್ಣ, l);
        ಯದಿ (!x)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        errno = 0;
        ಯದಿ (!typesafe_inet_ntop(family, u, x, l))
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಧ್ರುವ ವರ್ಣ* typesafe_inet_ntop(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *a, ವರ್ಣ *buf, size_t len) {
        ಪ್ರತ್ಯಾಗಮ inet_ntop(family, a, buf, len);
}

ಧ್ರುವ ವರ್ಣ* typesafe_inet_ntop4(ಧ್ರುವ ಸಂರಚನಾ in_addr *a, ವರ್ಣ *buf, size_t len) {
        ಪ್ರತ್ಯಾಗಮ inet_ntop(AF_INET, a, buf, len);
}

ಧ್ರುವ ವರ್ಣ* typesafe_inet_ntop6(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a, ವರ್ಣ *buf, size_t len) {
        ಪ್ರತ್ಯಾಗಮ inet_ntop(AF_INET6, a, buf, len);
}

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_to_string(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *u,
                unsigned prefixlen,
                ವರ್ಣ *buf,
                size_t buf_len) {

        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(buf);

        ಯದಿ (!IN_SET(family, AF_INET, AF_INET6))
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        errno = 0;
        ಯದಿ (!typesafe_inet_ntop(family, u, buf, buf_len))
                ಪ್ರತ್ಯಾಗಮ errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        ಯದಿ (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ಪ್ರತ್ಯಾಗಮ -ENOSPC;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_port_ifindex_name_to_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, uint16_t port, ಪೂರ್ಣಾಙ್ಕ ifindex, ಧ್ರುವ ವರ್ಣ *server_name, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *ip_str = ರಿಕ್ತ, *x = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(IN_SET(family, AF_INET, AF_INET6));
        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ifindex = 0;
        } ಅನ್ಯಥಾ
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        ಯದಿ (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಧ್ರುವ ವರ್ಣ *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        ಯದಿ (port > 0) {
                ಯದಿ (family == AF_INET6) {
                        ಯದಿ (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        ಅನ್ಯಥಾ
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } ಅನ್ಯಥಾ
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } ಅನ್ಯಥಾ {
                ಯದಿ (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                ಅನ್ಯಥಾ
                        x = strjoin(ip_str, separator, server_name);
        }
        ಯದಿ (!x)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = TAKE_PTR(x);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_from_string(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ವರ್ಣ *s, ಸಙ್ಘ in_addr_union *ret) {
        ಸಙ್ಘ in_addr_union buffer;
        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (!IN_SET(family, AF_INET, AF_INET6))
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        errno = 0;
        ಯದಿ (inet_pton(family, s, ret ?: &buffer) <= 0)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EINVAL);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_from_string_auto(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret_family, ಸಙ್ಘ in_addr_union *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = in_addr_from_string(AF_INET, s, ret);
        ಯದಿ (r >= 0) {
                ಯದಿ (ret_family)
                        *ret_family = AF_INET;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        ಯದಿ (r >= 0) {
                ಯದಿ (ret_family)
                        *ret_family = AF_INET6;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

unsigned ವರ್ಣ in4_addr_netmask_to_prefixlen(ಧ್ರುವ ಸಂರಚನಾ in_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ಪ್ರತ್ಯಾಗಮ 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
ಸಂರಚನಾ in_addr* in4_addr_prefixlen_to_netmask(ಸಂರಚನಾ in_addr *addr, unsigned ವರ್ಣ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        ಯದಿ (prefixlen == 0)
                addr->s_addr = 0;
        ಅನ್ಯಥಾ
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ಪ್ರತ್ಯಾಗಮ addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
ಸಂರಚನಾ in6_addr* in6_addr_prefixlen_to_netmask(ಸಂರಚನಾ in6_addr *addr, unsigned ವರ್ಣ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(prefixlen <= 128);

        ಪ್ರತಿ (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                ಯದಿ (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } ಅನ್ಯಥಾ ಯದಿ (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ಅನ್ಯಥಾ {
                        ಪ್ರತಿಜ್ಞಾ(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        ಪ್ರತ್ಯಾಗಮ addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
ಪೂರ್ಣಾಙ್ಕ in_addr_prefixlen_to_netmask(ಪೂರ್ಣಾಙ್ಕ family, ಸಙ್ಘ in_addr_union *addr, unsigned ವರ್ಣ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ವಿಕಲ್ಪ (family) {
        ಪಕ್ಷ AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                ಪ್ರತ್ಯಾಗಮ 0;
        ಪಕ್ಷ AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                ಪ್ರತ್ಯಾಗಮ 0;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ in4_addr_default_prefixlen(ಧ್ರುವ ಸಂರಚನಾ in_addr *addr, unsigned ವರ್ಣ *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(prefixlen);

        ಯದಿ (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        ಅನ್ಯಥಾ ಯದಿ (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        ಅನ್ಯಥಾ ಯದಿ (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        ಅನ್ಯಥಾ
                /* class D or E, no default prefixlen */
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in4_addr_default_subnet_mask(ಧ್ರುವ ಸಂರಚನಾ in_addr *addr, ಸಂರಚನಾ in_addr *mask) {
        unsigned ವರ್ಣ prefixlen;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in4_addr_mask(ಸಂರಚನಾ in_addr *addr, unsigned ವರ್ಣ prefixlen) {
        ಸಂರಚನಾ in_addr mask;

        ಪ್ರತಿಜ್ಞಾ(addr);

        ಯದಿ (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        addr->s_addr &= mask.s_addr;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in6_addr_mask(ಸಂರಚನಾ in6_addr *addr, unsigned ವರ್ಣ prefixlen) {
        unsigned i;

        ಪ್ರತಿಜ್ಞಾ(addr);

        ಪ್ರತಿ (i = 0; i < 16; i++) {
                uint8_t mask;

                ಯದಿ (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } ಅನ್ಯಥಾ ಯದಿ (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ಅನ್ಯಥಾ {
                        ಪ್ರತಿಜ್ಞಾ(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_mask(ಪೂರ್ಣಾಙ್ಕ family, ಸಙ್ಘ in_addr_union *addr, unsigned ವರ್ಣ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ವಿಕಲ್ಪ (family) {
        ಪಕ್ಷ AF_INET:
                ಪ್ರತ್ಯಾಗಮ in4_addr_mask(&addr->in, prefixlen);
        ಪಕ್ಷ AF_INET6:
                ಪ್ರತ್ಯಾಗಮ in6_addr_mask(&addr->in6, prefixlen);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ in4_addr_prefix_covers_full(
                ಧ್ರುವ ಸಂರಚನಾ in_addr *prefix,
                unsigned ವರ್ಣ prefixlen,
                ಧ್ರುವ ಸಂರಚನಾ in_addr *address,
                unsigned ವರ್ಣ address_prefixlen) {

        ಸಂರಚನಾ in_addr masked_prefix, masked_address;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(prefix);
        ಪ್ರತಿಜ್ಞಾ(address);

        ಯದಿ (prefixlen > address_prefixlen)
                ಪ್ರತ್ಯಾಗಮ false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ in4_addr_equal(&masked_prefix, &masked_address);
}

ಪೂರ್ಣಾಙ್ಕ in6_addr_prefix_covers_full(
                ಧ್ರುವ ಸಂರಚನಾ in6_addr *prefix,
                unsigned ವರ್ಣ prefixlen,
                ಧ್ರುವ ಸಂರಚನಾ in6_addr *address,
                unsigned ವರ್ಣ address_prefixlen) {

        ಸಂರಚನಾ in6_addr masked_prefix, masked_address;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(prefix);
        ಪ್ರತಿಜ್ಞಾ(address);

        ಯದಿ (prefixlen > address_prefixlen)
                ಪ್ರತ್ಯಾಗಮ false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ in6_addr_equal(&masked_prefix, &masked_address);
}

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_covers_full(
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *prefix,
                unsigned ವರ್ಣ prefixlen,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *address,
                unsigned ವರ್ಣ address_prefixlen) {

        ಪ್ರತಿಜ್ಞಾ(prefix);
        ಪ್ರತಿಜ್ಞಾ(address);

        ವಿಕಲ್ಪ (family) {
        ಪಕ್ಷ AF_INET:
                ಪ್ರತ್ಯಾಗಮ in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        ಪಕ್ಷ AF_INET6:
                ಪ್ರತ್ಯಾಗಮ in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ in_addr_parse_prefixlen(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ವರ್ಣ *p, unsigned ವರ್ಣ *ret) {
        uint8_t u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!IN_SET(family, AF_INET, AF_INET6))
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (u > FAMILY_ADDRESS_SIZE(family) * 8)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_from_string(
                ಧ್ರುವ ವರ್ಣ *p,
                ಪೂರ್ಣಾಙ್ಕ family,
                ಸಙ್ಘ in_addr_union *ret_prefix,
                unsigned ವರ್ಣ *ret_prefixlen) {

        _cleanup_free_ ವರ್ಣ *str = ರಿಕ್ತ;
        ಸಙ್ಘ in_addr_union buffer;
        ಧ್ರುವ ವರ್ಣ *e, *l;
        unsigned ವರ್ಣ k;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        ಯದಿ (!IN_SET(family, AF_INET, AF_INET6))
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        e = strchr(p, '/');
        ಯದಿ (e) {
                str = strndup(p, e - p);
                ಯದಿ (!str)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                l = str;
        } ಅನ್ಯಥಾ
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        } ಅನ್ಯಥಾ
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        ಯದಿ (ret_prefix)
                *ret_prefix = buffer;
        ಯದಿ (ret_prefixlen)
                *ret_prefixlen = k;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ in_addr_prefix_from_string_auto_full(
                ಧ್ರುವ ವರ್ಣ *p,
                InAddrPrefixLenMode mode,
                ಪೂರ್ಣಾಙ್ಕ *ret_family,
                ಸಙ್ಘ in_addr_union *ret_prefix,
                unsigned ವರ್ಣ *ret_prefixlen) {

        _cleanup_free_ ವರ್ಣ *str = ರಿಕ್ತ;
        ಸಙ್ಘ in_addr_union buffer;
        ಧ್ರುವ ವರ್ಣ *e, *l;
        unsigned ವರ್ಣ k;
        ಪೂರ್ಣಾಙ್ಕ family, r;

        ಪ್ರತಿಜ್ಞಾ(p);

        e = strchr(p, '/');
        ಯದಿ (e) {
                str = strndup(p, e - p);
                ಯದಿ (!str)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                l = str;
        } ಅನ್ಯಥಾ
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        } ಅನ್ಯಥಾ
                ವಿಕಲ್ಪ (mode) {
                ಪಕ್ಷ PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        ಭಙ್ಗ;
                ಪಕ್ಷ PREFIXLEN_REFUSE:
                        ಪ್ರತ್ಯಾಗಮ -ENOANO; /* To distinguish this error from others. */
                ಸ್ವಭಾವ:
                        assert_not_reached();
                }

        ಯದಿ (ret_family)
                *ret_family = family;
        ಯದಿ (ret_prefix)
                *ret_prefix = buffer;
        ಯದಿ (ret_prefixlen)
                *ret_prefixlen = k;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ in_addr_hash_func(ಧ್ರುವ ಸಙ್ಘ in_addr_union *u, ಪೂರ್ಣಾಙ್ಕ family, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

ಶೂನ್ಯ in_addr_data_hash_func(ಧ್ರುವ ಸಂರಚನಾ in_addr_data *a, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

ಪೂರ್ಣಾಙ್ಕ in_addr_data_compare_func(ಧ್ರುವ ಸಂರಚನಾ in_addr_data *x, ಧ್ರುವ ಸಂರಚನಾ in_addr_data *y) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(y);

        r = CMP(x->family, y->family);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        ಸಂರಚನಾ in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        ಸಂರಚನಾ in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        ಮುಕ್ತ);

ಶೂನ್ಯ in6_addr_hash_func(ಧ್ರುವ ಸಂರಚನಾ in6_addr *addr, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress_typesafe(*addr, state);
}

ಪೂರ್ಣಾಙ್ಕ in6_addr_compare_func(ಧ್ರುವ ಸಂರಚನಾ in6_addr *a, ಧ್ರುವ ಸಂರಚನಾ in6_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ memcmp(a, b, ಪರಿಮಾಣ(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        ಸಂರಚನಾ in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
        in6_addr_hash_ops_free,
        ಸಂರಚನಾ in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        ಮುಕ್ತ);
