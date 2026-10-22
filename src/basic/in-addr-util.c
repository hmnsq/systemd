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

bool in4_addr_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ a->s_addr == 0;
}

bool in6_addr_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ eqzero(a->s6_addr32);
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_null(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಆದರೆ (family == AF_INET)
                ಹಿಂತಿರುಗಿ in4_addr_is_null(&u->ಒಳ);

        ಆದರೆ (family == AF_INET6)
                ಹಿಂತಿರುಗಿ in6_addr_is_null(&u->in6);

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಆದರೆ (!in4_addr_is_link_local(a))
                ಹಿಂತಿರುಗಿ false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ಹಿಂತಿರುಗಿ !ಒಳ_ಗಣ(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_link_local(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಆದರೆ (family == AF_INET)
                ಹಿಂತಿರುಗಿ in4_addr_is_link_local(&u->ಒಳ);

        ಆದರೆ (family == AF_INET6)
                ಹಿಂತಿರುಗಿ in6_addr_is_link_local(&u->in6);

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        /* ff02::1 */
        ಹಿಂತಿರುಗಿ be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ a->s6_addr[0] == 0xff;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_multicast(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಆದರೆ (family == AF_INET)
                ಹಿಂತಿರುಗಿ in4_addr_is_multicast(&u->ಒಳ);

        ಆದರೆ (family == AF_INET6)
                ಹಿಂತಿರುಗಿ in6_addr_is_multicast(&u->in6);

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        /* All of 127.x.x.x is localhost. */
        ಹಿಂತಿರುಗಿ (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ಹಿಂತಿರುಗಿ !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ಸ್ಥಿರ bool in6_addr_is_loopback(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) {
        ಹಿಂತಿರುಗಿ memcmp(a, &(ರಚನೆ in6_addr) IN6ADDR_LOOPBACK_INIT, ಗಾತ್ರ(ರಚನೆ in6_addr)) == 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_localhost(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಆದರೆ (family == AF_INET)
                ಹಿಂತಿರುಗಿ in4_addr_is_localhost(&u->ಒಳ);

        ಆದರೆ (family == AF_INET6)
                ಹಿಂತಿರುಗಿ in6_addr_is_loopback(&u->in6);

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_localhost_one(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) {
        ಪ್ರತಿಜ್ಞಾ(u);

        ಆದರೆ (family == AF_INET)
                /* 127.0.0.1 */
                ಹಿಂತಿರುಗಿ be32toh(u->ಒಳ.s_addr) == UINT32_C(0x7F000001);

        ಆದರೆ (family == AF_INET6)
                ಹಿಂತಿರುಗಿ in6_addr_is_loopback(&u->in6);

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_ವಿಳಾಸ(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ a->s_addr == b->s_addr;
}

bool in6_addr_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ memcmp(a, b, ಗಾತ್ರ(ರಚನೆ in6_addr)) == 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_equal(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಆದರೆ (family == AF_INET)
                ಹಿಂತಿರುಗಿ in4_addr_equal(&a->ಒಳ, &b->ಒಳ);

        ಆದರೆ (family == AF_INET6)
                ಹಿಂತಿರುಗಿ in6_addr_equal(&a->in6, &b->in6);

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

bool in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(
                ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a,
                ಚಿಹ್ನೆರಹಿತ aprefixlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *b,
                ಚಿಹ್ನೆರಹಿತ bprefixlen) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಚಿಹ್ನೆರಹಿತ m = MIN3(aprefixlen, bprefixlen, (ಚಿಹ್ನೆರಹಿತ) (ಗಾತ್ರ(ರಚನೆ in_addr) * 8));
        ಆದರೆ (m == 0)
                ಹಿಂತಿರುಗಿ true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ಹಿಂತಿರುಗಿ (x & n) == 0;
}

bool in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(
                ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a,
                ಚಿಹ್ನೆರಹಿತ aprefixlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *b,
                ಚಿಹ್ನೆರಹಿತ bprefixlen) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಚಿಹ್ನೆರಹಿತ m = MIN3(aprefixlen, bprefixlen, (ಚಿಹ್ನೆರಹಿತ) (ಗಾತ್ರ(ರಚನೆ in6_addr) * 8));
        ಆದರೆ (m == 0)
                ಹಿಂತಿರುಗಿ true;

        ಗಾಗಿ (size_t i = 0; i < ಗಾತ್ರ(ರಚನೆ in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                ಆದರೆ ((x & n) != 0)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (m <= 8)
                        ನಿಲ್ಲಿಸು;

                m -= 8;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a,
                ಚಿಹ್ನೆರಹಿತ aprefixlen,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *b,
                ಚಿಹ್ನೆರಹಿತ bprefixlen) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        /* Checks whether there are any addresses that are in both networks. */

        ಆದರೆ (family == AF_INET)
                ಹಿಂತಿರುಗಿ in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(&a->ಒಳ, aprefixlen, &b->ಒಳ, bprefixlen);

        ಆದರೆ (family == AF_INET6)
                ಹಿಂತಿರುಗಿ in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_random_ಪೂರ್ವಪ್ರತ್ಯಯ(
                ಪೂರ್ಣಾಂಕ family,
                ಒಕ್ಕೂಟ ಒಳ_addr_union *u,
                ಚಿಹ್ನೆರಹಿತ prefixlen_fixed_part,
                ಚಿಹ್ನೆರಹಿತ prefixlen) {

        ಪ್ರತಿಜ್ಞಾ(u);

        /* Random network part of an address by one. */

        ಆದರೆ (prefixlen <= 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (family == AF_INET) {
                uint32_t c, n;

                ಆದರೆ (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                ಆದರೆ (prefixlen > 32)
                        prefixlen = 32;
                ಆದರೆ (prefixlen_fixed_part >= prefixlen)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                c = be32toh(u->ಒಳ.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_ಬೈಟುಗಳು(&n, ಗಾತ್ರ(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->ಒಳ.s_addr = htobe32(n | c);
                ಹಿಂತಿರುಗಿ 1;
        }

        ಆದರೆ (family == AF_INET6) {
                ರಚನೆ in6_addr n;
                ಚಿಹ್ನೆರಹಿತ i, j;

                ಆದರೆ (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                ಆದರೆ (prefixlen > 128)
                        prefixlen = 128;
                ಆದರೆ (prefixlen_fixed_part >= prefixlen)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                random_ಬೈಟುಗಳು(&n, ಗಾತ್ರ(n));

                ಗಾಗಿ (i = 0; i < 16; i++) {
                        uint8_t ಮುಖವಾಡ_fixed_part = 0, ಮುಖವಾಡ_3 = 0;

                        ಆದರೆ (i < (prefixlen_fixed_part + 7) / 8) {
                                ಆದರೆ (i < prefixlen_fixed_part / 8)
                                        ಮುಖವಾಡ_fixed_part = 0xffu;
                                ಇಲ್ಲದಿದ್ದರೆ {
                                        j = prefixlen_fixed_part % 8;
                                        ಮುಖವಾಡ_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        ಆದರೆ (i < (prefixlen + 7) / 8) {
                                ಆದರೆ (i < prefixlen / 8)
                                        ಮುಖವಾಡ_3 = 0xffu ^ ಮುಖವಾಡ_fixed_part;
                                ಇಲ್ಲದಿದ್ದರೆ {
                                        j = prefixlen % 8;
                                        ಮುಖವಾಡ_3 = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ ಮುಖವಾಡ_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= ಮುಖವಾಡ_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & ಮುಖವಾಡ_3;
                }

                ಹಿಂತಿರುಗಿ 1;
        }

        ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_range(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ಒಳ,
                ಚಿಹ್ನೆರಹಿತ prefixlen,
                ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಆರಂಭ,
                ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಕೊನೆ) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳ);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        ಆಯ್ಕೆ (family) {
        ಸ್ಥಿತಿ AF_INET:
                ಆದರೆ (prefixlen > 32)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ AF_INET6:
                ಆದರೆ (prefixlen > 128)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }

        ಆದರೆ (prefixlen == 0) {
                /* shortcut for trivial case */
                ಆದರೆ (ret_ಆರಂಭ)
                        *ret_ಆರಂಭ = ಒಳ_ADDR_NULL;
                ಆದರೆ (ret_ಕೊನೆ)
                        *ret_ಕೊನೆ = ಒಳ_ADDR_NULL;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಒಕ್ಕೂಟ ಒಳ_addr_union ಆರಂಭ = *ಒಳ;
        r = ಒಳ_addr_ಮುಖವಾಡ(family, &ಆರಂಭ, prefixlen);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಒಕ್ಕೂಟ ಒಳ_addr_union ಕೊನೆ = ಆರಂಭ;
        uint8_t ಕೂಡಿಸು = UINT8_C(1) << ((FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(family) * 8 - prefixlen) % 8);
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = DIV_ಸುತ್ತು_ಮೇಲೆ(prefixlen, 8); i-- > 0;)
                ಆದರೆ (ಕೂಡಿಸು > UINT8_MAX - ಕೊನೆ.ಬೈಟುಗಳು[i]) {
                        ಕೊನೆ.ಬೈಟುಗಳು[i] = 0;
                        ಕೂಡಿಸು = 1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಕೊನೆ.ಬೈಟುಗಳು[i] += ಕೂಡಿಸು;
                        ನಿಲ್ಲಿಸು;
                }

        ಆದರೆ (ret_ಆರಂಭ)
                *ret_ಆರಂಭ = ಆರಂಭ;
        ಆದರೆ (ret_ಕೊನೆ)
                *ret_ಕೊನೆ = ಕೊನೆ;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *x = ಖಾಲಿ;
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (family == AF_INET)
                l = INET_ADDRSTRLEN;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        x = new(ಅಕ್ಷರ, l);
        ಆದರೆ (!x)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        errno = 0;
        ಆದರೆ (!typesafe_inet_ntop(family, u, x, l))
                ಹಿಂತಿರುಗಿ errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* typesafe_inet_ntop(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a, ಅಕ್ಷರ *buf, size_t len) {
        ಹಿಂತಿರುಗಿ inet_ntop(family, a, buf, len);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* typesafe_inet_ntop4(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a, ಅಕ್ಷರ *buf, size_t len) {
        ಹಿಂತಿರುಗಿ inet_ntop(AF_INET, a, buf, len);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* typesafe_inet_ntop6(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a, ಅಕ್ಷರ *buf, size_t len) {
        ಹಿಂತಿರುಗಿ inet_ntop(AF_INET6, a, buf, len);
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u,
                ಚಿಹ್ನೆರಹಿತ prefixlen,
                ಅಕ್ಷರ *buf,
                size_t buf_len) {

        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(buf);

        ಆದರೆ (!ಒಳ_ಗಣ(family, AF_INET, AF_INET6))
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        errno = 0;
        ಆದರೆ (!typesafe_inet_ntop(family, u, buf, buf_len))
                ಹಿಂತಿರುಗಿ errno_or_else(ENOSPC);

        size_t l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
        ಆದರೆ (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ಹಿಂತಿರುಗಿ -ENOSPC;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೋರ್ಟ್_ifindex_name_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, uint16_t port, ಪೂರ್ಣಾಂಕ ifindex, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *server_name, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ip_str = ಖಾಲಿ, *x = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(family, AF_INET, AF_INET6));
        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = ಒಳ_addr_ಗಮ್ಯ_string(family, u, &ip_str);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (family == AF_INET6) {
                r = ಒಳ_addr_is_link_local(family, u);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ifindex = 0;
        } ಇಲ್ಲದಿದ್ದರೆ
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        ಆದರೆ (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        ಆದರೆ (port > 0) {
                ಆದರೆ (family == AF_INET6) {
                        ಆದರೆ (ifindex > 0)
                                x = asprintf_ಸುರಕ್ಷಿತ("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        ಇಲ್ಲದಿದ್ದರೆ
                                x = asprintf_ಸುರಕ್ಷಿತ("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } ಇಲ್ಲದಿದ್ದರೆ
                        x = asprintf_ಸುರಕ್ಷಿತ("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (ifindex > 0)
                        x = asprintf_ಸುರಕ್ಷಿತ("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                ಇಲ್ಲದಿದ್ದರೆ
                        x = strjoin(ip_str, separator, server_name);
        }
        ಆದರೆ (!x)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = TAKE_PTR(x);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_from_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಒಕ್ಕೂಟ ಒಳ_addr_union *ret) {
        ಒಕ್ಕೂಟ ಒಳ_addr_union ಬಫರ್;
        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (!ಒಳ_ಗಣ(family, AF_INET, AF_INET6))
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        errno = 0;
        ಆದರೆ (inet_pton(family, s, ret ?: &ಬಫರ್) <= 0)
                ಹಿಂತಿರುಗಿ errno_or_else(EINVAL);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_from_string_auto(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *ret_family, ಒಕ್ಕೂಟ ಒಳ_addr_union *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ಒಳ_addr_from_string(AF_INET, s, ret);
        ಆದರೆ (r >= 0) {
                ಆದರೆ (ret_family)
                        *ret_family = AF_INET;
                ಹಿಂತಿರುಗಿ 0;
        }

        r = ಒಳ_addr_from_string(AF_INET6, s, ret);
        ಆದರೆ (r >= 0) {
                ಆದರೆ (ret_family)
                        *ret_family = AF_INET6;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ in4_addr_netmask_ಗಮ್ಯ_prefixlen(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ಹಿಂತಿರುಗಿ 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
ರಚನೆ in_addr* in4_addr_prefixlen_ಗಮ್ಯ_netmask(ರಚನೆ in_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        ಆದರೆ (prefixlen == 0)
                addr->s_addr = 0;
        ಇಲ್ಲದಿದ್ದರೆ
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ಹಿಂತಿರುಗಿ addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
ರಚನೆ in6_addr* in6_addr_prefixlen_ಗಮ್ಯ_netmask(ರಚನೆ in6_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(prefixlen <= 128);

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < 16; i++) {
                uint8_t ಮುಖವಾಡ_3;

                ಆದರೆ (prefixlen >= 8) {
                        ಮುಖವಾಡ_3 = 0xFF;
                        prefixlen -= 8;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prefixlen > 0) {
                        ಮುಖವಾಡ_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಪ್ರತಿಜ್ಞಾ(prefixlen == 0);
                        ಮುಖವಾಡ_3 = 0;
                }

                addr->s6_addr[i] = ಮುಖವಾಡ_3;
        }

        ಹಿಂತಿರುಗಿ addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
ಪೂರ್ಣಾಂಕ ಒಳ_addr_prefixlen_ಗಮ್ಯ_netmask(ಪೂರ್ಣಾಂಕ family, ಒಕ್ಕೂಟ ಒಳ_addr_union *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ಆಯ್ಕೆ (family) {
        ಸ್ಥಿತಿ AF_INET:
                in4_addr_prefixlen_ಗಮ್ಯ_netmask(&addr->ಒಳ, prefixlen);
                ಹಿಂತಿರುಗಿ 0;
        ಸ್ಥಿತಿ AF_INET6:
                in6_addr_prefixlen_ಗಮ್ಯ_netmask(&addr->in6, prefixlen);
                ಹಿಂತಿರುಗಿ 0;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ in4_addr_default_prefixlen(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(prefixlen);

        ಆದರೆ (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        ಇಲ್ಲದಿದ್ದರೆ
                /* class D or E, no default prefixlen */
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ in4_addr_default_subnet_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *addr, ರಚನೆ in_addr *ಮುಖವಾಡ_3) {
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(ಮುಖವಾಡ_3);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        in4_addr_prefixlen_ಗಮ್ಯ_netmask(ಮುಖವಾಡ_3, prefixlen);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ in4_addr_ಮುಖವಾಡ(ರಚನೆ in_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen) {
        ರಚನೆ in_addr ಮುಖವಾಡ_3;

        ಪ್ರತಿಜ್ಞಾ(addr);

        ಆದರೆ (!in4_addr_prefixlen_ಗಮ್ಯ_netmask(&ಮುಖವಾಡ_3, prefixlen))
                ಹಿಂತಿರುಗಿ -EINVAL;

        addr->s_addr &= ಮುಖವಾಡ_3.s_addr;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ in6_addr_ಮುಖವಾಡ(ರಚನೆ in6_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen) {
        ಚಿಹ್ನೆರಹಿತ i;

        ಪ್ರತಿಜ್ಞಾ(addr);

        ಗಾಗಿ (i = 0; i < 16; i++) {
                uint8_t ಮುಖವಾಡ_3;

                ಆದರೆ (prefixlen >= 8) {
                        ಮುಖವಾಡ_3 = 0xFF;
                        prefixlen -= 8;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prefixlen > 0) {
                        ಮುಖವಾಡ_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಪ್ರತಿಜ್ಞಾ(prefixlen == 0);
                        ಮುಖವಾಡ_3 = 0;
                }

                addr->s6_addr[i] &= ಮುಖವಾಡ_3;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಮುಖವಾಡ(ಪೂರ್ಣಾಂಕ family, ಒಕ್ಕೂಟ ಒಳ_addr_union *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ಆಯ್ಕೆ (family) {
        ಸ್ಥಿತಿ AF_INET:
                ಹಿಂತಿರುಗಿ in4_addr_ಮುಖವಾಡ(&addr->ಒಳ, prefixlen);
        ಸ್ಥಿತಿ AF_INET6:
                ಹಿಂತಿರುಗಿ in6_addr_ಮುಖವಾಡ(&addr->in6, prefixlen);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *ಪೂರ್ವಪ್ರತ್ಯಯ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *ವಿಳಾಸ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಿಳಾಸ_prefixlen) {

        ರಚನೆ in_addr masked_ಪೂರ್ವಪ್ರತ್ಯಯ, masked_ವಿಳಾಸ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಪ್ರತಿಜ್ಞಾ(ವಿಳಾಸ);

        ಆದರೆ (prefixlen > ವಿಳಾಸ_prefixlen)
                ಹಿಂತಿರುಗಿ false;

        masked_ಪೂರ್ವಪ್ರತ್ಯಯ = *ಪೂರ್ವಪ್ರತ್ಯಯ;
        r = in4_addr_ಮುಖವಾಡ(&masked_ಪೂರ್ವಪ್ರತ್ಯಯ, prefixlen);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        masked_ವಿಳಾಸ = *ವಿಳಾಸ;
        r = in4_addr_ಮುಖವಾಡ(&masked_ವಿಳಾಸ, prefixlen);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ in4_addr_equal(&masked_ಪೂರ್ವಪ್ರತ್ಯಯ, &masked_ವಿಳಾಸ);
}

ಪೂರ್ಣಾಂಕ in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *ಪೂರ್ವಪ್ರತ್ಯಯ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *ವಿಳಾಸ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಿಳಾಸ_prefixlen) {

        ರಚನೆ in6_addr masked_ಪೂರ್ವಪ್ರತ್ಯಯ, masked_ವಿಳಾಸ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಪ್ರತಿಜ್ಞಾ(ವಿಳಾಸ);

        ಆದರೆ (prefixlen > ವಿಳಾಸ_prefixlen)
                ಹಿಂತಿರುಗಿ false;

        masked_ಪೂರ್ವಪ್ರತ್ಯಯ = *ಪೂರ್ವಪ್ರತ್ಯಯ;
        r = in6_addr_ಮುಖವಾಡ(&masked_ಪೂರ್ವಪ್ರತ್ಯಯ, prefixlen);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        masked_ವಿಳಾಸ = *ವಿಳಾಸ;
        r = in6_addr_ಮುಖವಾಡ(&masked_ವಿಳಾಸ, prefixlen);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ in6_addr_equal(&masked_ಪೂರ್ವಪ್ರತ್ಯಯ, &masked_ವಿಳಾಸ);
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ಪೂರ್ವಪ್ರತ್ಯಯ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ವಿಳಾಸ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಿಳಾಸ_prefixlen) {

        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಪ್ರತಿಜ್ಞಾ(ವಿಳಾಸ);

        ಆಯ್ಕೆ (family) {
        ಸ್ಥಿತಿ AF_INET:
                ಹಿಂತಿರುಗಿ in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(&ಪೂರ್ವಪ್ರತ್ಯಯ->ಒಳ, prefixlen, &ವಿಳಾಸ->ಒಳ, ವಿಳಾಸ_prefixlen);
        ಸ್ಥಿತಿ AF_INET6:
                ಹಿಂತಿರುಗಿ in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(&ಪೂರ್ವಪ್ರತ್ಯಯ->in6, prefixlen, &ವಿಳಾಸ->in6, ವಿಳಾಸ_prefixlen);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_parse_prefixlen(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ret) {
        uint8_t u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!ಒಳ_ಗಣ(family, AF_INET, AF_INET6))
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        r = ಸುರಕ್ಷಿತ_atou8(p, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (u > FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(family) * 8)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_from_string(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p,
                ಪೂರ್ಣಾಂಕ family,
                ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಪೂರ್ವಪ್ರತ್ಯಯ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ret_prefixlen) {

        _cleanup_free_ ಅಕ್ಷರ *str = ಖಾಲಿ;
        ಒಕ್ಕೂಟ ಒಳ_addr_union ಬಫರ್;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, *l;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ k;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        ಆದರೆ (!ಒಳ_ಗಣ(family, AF_INET, AF_INET6))
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        e = strchr(p, '/');
        ಆದರೆ (e) {
                str = strndup(p, e - p);
                ಆದರೆ (!str)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                l = str;
        } ಇಲ್ಲದಿದ್ದರೆ
                l = p;

        r = ಒಳ_addr_from_string(family, l, &ಬಫರ್);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (e) {
                r = ಒಳ_addr_parse_prefixlen(family, e+1, &k);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        } ಇಲ್ಲದಿದ್ದರೆ
                k = FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(family) * 8;

        ಆದರೆ (ret_ಪೂರ್ವಪ್ರತ್ಯಯ)
                *ret_ಪೂರ್ವಪ್ರತ್ಯಯ = ಬಫರ್;
        ಆದರೆ (ret_prefixlen)
                *ret_prefixlen = k;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_from_string_auto_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p,
                ಒಳ_Addr_ಪೂರ್ವಪ್ರತ್ಯಯ_Len_ಬಹುಲಕ ಬಹುಲಕ,
                ಪೂರ್ಣಾಂಕ *ret_family,
                ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಪೂರ್ವಪ್ರತ್ಯಯ,
                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ret_prefixlen) {

        _cleanup_free_ ಅಕ್ಷರ *str = ಖಾಲಿ;
        ಒಕ್ಕೂಟ ಒಳ_addr_union ಬಫರ್;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, *l;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ k;
        ಪೂರ್ಣಾಂಕ family, r;

        ಪ್ರತಿಜ್ಞಾ(p);

        e = strchr(p, '/');
        ಆದರೆ (e) {
                str = strndup(p, e - p);
                ಆದರೆ (!str)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                l = str;
        } ಇಲ್ಲದಿದ್ದರೆ
                l = p;

        r = ಒಳ_addr_from_string_auto(l, &family, &ಬಫರ್);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (e) {
                r = ಒಳ_addr_parse_prefixlen(family, e+1, &k);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        } ಇಲ್ಲದಿದ್ದರೆ
                ಆಯ್ಕೆ (ಬಹುಲಕ) {
                ಸ್ಥಿತಿ PREFIXLEN_ತುಂಬಿದ:
                        k = FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(family) * 8;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ PREFIXLEN_REFUSE:
                        ಹಿಂತಿರುಗಿ -ENOANO; /* To distinguish this error from others. */
                ಪೂರ್ವನಿಯೋಜಿತ:
                        assert_not_reached();
                }

        ಆದರೆ (ret_family)
                *ret_family = family;
        ಆದರೆ (ret_ಪೂರ್ವಪ್ರತ್ಯಯ)
                *ret_ಪೂರ್ವಪ್ರತ್ಯಯ = ಬಫರ್;
        ಆದರೆ (ret_prefixlen)
                *ret_prefixlen = k;

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ ಒಳ_addr_hash_func(ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, ಪೂರ್ಣಾಂಕ family, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress(u->ಬೈಟುಗಳು, FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(family), state);
}

ಶೂನ್ಯ ಒಳ_addr_ದತ್ತಾಂಶ_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *a, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress_typesafe(a->family, state);
        ಒಳ_addr_hash_func(&a->ವಿಳಾಸ, a->family, state);
}

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ದತ್ತಾಂಶ_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *x, ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *y) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(y);

        r = CMP(x->family, y->family);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ memcmp(&x->ವಿಳಾಸ, &y->ವಿಳಾಸ, FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(x->family));
}

DEFINE_HASH_OPS(
        ಒಳ_addr_ದತ್ತಾಂಶ_hash_ops,
        ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ,
        ಒಳ_addr_ದತ್ತಾಂಶ_hash_func,
        ಒಳ_addr_ದತ್ತಾಂಶ_compare_func);

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
        ಒಳ_addr_ದತ್ತಾಂಶ_hash_ops_free,
        ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ,
        ಒಳ_addr_ದತ್ತಾಂಶ_hash_func,
        ಒಳ_addr_ದತ್ತಾಂಶ_compare_func,
        ಬಿಡುಗಡೆ);

ಶೂನ್ಯ in6_addr_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *addr, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress_typesafe(*addr, state);
}

ಪೂರ್ಣಾಂಕ in6_addr_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ memcmp(a, b, ಗಾತ್ರ(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        ರಚನೆ in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
        in6_addr_hash_ops_free,
        ರಚನೆ in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        ಬಿಡುಗಡೆ);
