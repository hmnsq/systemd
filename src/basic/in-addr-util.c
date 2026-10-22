/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <endian.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash-funcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "in-addr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "logarithm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "random-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

bool in4_addr_is_null(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ a->s_addr == 0;
}

bool in6_addr_is_null(ਅਚਲ ਬਣਤਰ in6_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ eqzero(a->s6_addr32);
}

ਪੂਰਨ ਵਿੱਚ_addr_is_null(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(u);

        ਜੇ (family == AF_INET)
                ਵਾਪਸ in4_addr_is_null(&u->ਵਿੱਚ);

        ਜੇ (family == AF_INET6)
                ਵਾਪਸ in6_addr_is_null(&u->in6);

        ਵਾਪਸ -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਜੇ (!in4_addr_is_link_local(a))
                ਵਾਪਸ false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ਵਾਪਸ !ਵਿੱਚ_ਸੈੱਟ(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ਅਚਲ ਬਣਤਰ in6_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

ਪੂਰਨ ਵਿੱਚ_addr_is_link_local(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(u);

        ਜੇ (family == AF_INET)
                ਵਾਪਸ in4_addr_is_link_local(&u->ਵਿੱਚ);

        ਜੇ (family == AF_INET6)
                ਵਾਪਸ in6_addr_is_link_local(&u->in6);

        ਵਾਪਸ -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ਅਚਲ ਬਣਤਰ in6_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        /* ff02::1 */
        ਵਾਪਸ be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ਅਚਲ ਬਣਤਰ in6_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ a->s6_addr[0] == 0xff;
}

ਪੂਰਨ ਵਿੱਚ_addr_is_multicast(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(u);

        ਜੇ (family == AF_INET)
                ਵਾਪਸ in4_addr_is_multicast(&u->ਵਿੱਚ);

        ਜੇ (family == AF_INET6)
                ਵਾਪਸ in6_addr_is_multicast(&u->in6);

        ਵਾਪਸ -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        /* All of 127.x.x.x is localhost. */
        ਵਾਪਸ (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ਅਚਲ ਬਣਤਰ in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ਵਾਪਸ !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ਸਥਿਰ bool in6_addr_is_loopback(ਅਚਲ ਬਣਤਰ in6_addr *a) {
        ਵਾਪਸ memcmp(a, &(ਬਣਤਰ in6_addr) IN6ADDR_LOOPBACK_INIT, ਆਕਾਰ(ਬਣਤਰ in6_addr)) == 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_is_localhost(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(u);

        ਜੇ (family == AF_INET)
                ਵਾਪਸ in4_addr_is_localhost(&u->ਵਿੱਚ);

        ਜੇ (family == AF_INET6)
                ਵਾਪਸ in6_addr_is_loopback(&u->in6);

        ਵਾਪਸ -EAFNOSUPPORT;
}

ਪੂਰਨ ਵਿੱਚ_addr_is_localhost_one(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(u);

        ਜੇ (family == AF_INET)
                /* 127.0.0.1 */
                ਵਾਪਸ be32toh(u->ਵਿੱਚ.s_addr) == UINT32_C(0x7F000001);

        ਜੇ (family == AF_INET6)
                ਵਾਪਸ in6_addr_is_loopback(&u->in6);

        ਵਾਪਸ -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_ਪਤਾ(ਅਚਲ ਬਣਤਰ in6_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ਅਚਲ ਬਣਤਰ in_addr *a, ਅਚਲ ਬਣਤਰ in_addr *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ a->s_addr == b->s_addr;
}

bool in6_addr_equal(ਅਚਲ ਬਣਤਰ in6_addr *a, ਅਚਲ ਬਣਤਰ in6_addr *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ memcmp(a, b, ਆਕਾਰ(ਬਣਤਰ in6_addr)) == 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_equal(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਜੇ (family == AF_INET)
                ਵਾਪਸ in4_addr_equal(&a->ਵਿੱਚ, &b->ਵਿੱਚ);

        ਜੇ (family == AF_INET6)
                ਵਾਪਸ in6_addr_equal(&a->in6, &b->in6);

        ਵਾਪਸ -EAFNOSUPPORT;
}

bool in4_addr_ਅਗੇਤਰ_intersect(
                ਅਚਲ ਬਣਤਰ in_addr *a,
                ਬਿਨਾਨਿਸ਼ਾਨੀ aprefixlen,
                ਅਚਲ ਬਣਤਰ in_addr *b,
                ਬਿਨਾਨਿਸ਼ਾਨੀ bprefixlen) {

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਬਿਨਾਨਿਸ਼ਾਨੀ m = MIN3(aprefixlen, bprefixlen, (ਬਿਨਾਨਿਸ਼ਾਨੀ) (ਆਕਾਰ(ਬਣਤਰ in_addr) * 8));
        ਜੇ (m == 0)
                ਵਾਪਸ true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ਵਾਪਸ (x & n) == 0;
}

bool in6_addr_ਅਗੇਤਰ_intersect(
                ਅਚਲ ਬਣਤਰ in6_addr *a,
                ਬਿਨਾਨਿਸ਼ਾਨੀ aprefixlen,
                ਅਚਲ ਬਣਤਰ in6_addr *b,
                ਬਿਨਾਨਿਸ਼ਾਨੀ bprefixlen) {

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਬਿਨਾਨਿਸ਼ਾਨੀ m = MIN3(aprefixlen, bprefixlen, (ਬਿਨਾਨਿਸ਼ਾਨੀ) (ਆਕਾਰ(ਬਣਤਰ in6_addr) * 8));
        ਜੇ (m == 0)
                ਵਾਪਸ true;

        ਲਈ (size_t i = 0; i < ਆਕਾਰ(ਬਣਤਰ in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                ਜੇ ((x & n) != 0)
                        ਵਾਪਸ false;

                ਜੇ (m <= 8)
                        ਰੋਕੋ;

                m -= 8;
        }

        ਵਾਪਸ true;
}

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_intersect(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a,
                ਬਿਨਾਨਿਸ਼ਾਨੀ aprefixlen,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *b,
                ਬਿਨਾਨਿਸ਼ਾਨੀ bprefixlen) {

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        /* Checks whether there are any addresses that are in both networks. */

        ਜੇ (family == AF_INET)
                ਵਾਪਸ in4_addr_ਅਗੇਤਰ_intersect(&a->ਵਿੱਚ, aprefixlen, &b->ਵਿੱਚ, bprefixlen);

        ਜੇ (family == AF_INET6)
                ਵਾਪਸ in6_addr_ਅਗੇਤਰ_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ਵਾਪਸ -EAFNOSUPPORT;
}

ਪੂਰਨ ਵਿੱਚ_addr_random_ਅਗੇਤਰ(
                ਪੂਰਨ family,
                ਸੰਘ ਵਿੱਚ_addr_union *u,
                ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen_fixed_part,
                ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen) {

        ਪ੍ਰਤਿਜ੍ਞਾ(u);

        /* Random network part of an address by one. */

        ਜੇ (prefixlen <= 0)
                ਵਾਪਸ 0;

        ਜੇ (family == AF_INET) {
                uint32_t c, n;

                ਜੇ (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                ਜੇ (prefixlen > 32)
                        prefixlen = 32;
                ਜੇ (prefixlen_fixed_part >= prefixlen)
                        ਵਾਪਸ -EINVAL;

                c = be32toh(u->ਵਿੱਚ.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_ਬਾਈਟਾਂ(&n, ਆਕਾਰ(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->ਵਿੱਚ.s_addr = htobe32(n | c);
                ਵਾਪਸ 1;
        }

        ਜੇ (family == AF_INET6) {
                ਬਣਤਰ in6_addr n;
                ਬਿਨਾਨਿਸ਼ਾਨੀ i, j;

                ਜੇ (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                ਜੇ (prefixlen > 128)
                        prefixlen = 128;
                ਜੇ (prefixlen_fixed_part >= prefixlen)
                        ਵਾਪਸ -EINVAL;

                random_ਬਾਈਟਾਂ(&n, ਆਕਾਰ(n));

                ਲਈ (i = 0; i < 16; i++) {
                        uint8_t ਨਕਾਬ_fixed_part = 0, ਨਕਾਬ_3 = 0;

                        ਜੇ (i < (prefixlen_fixed_part + 7) / 8) {
                                ਜੇ (i < prefixlen_fixed_part / 8)
                                        ਨਕਾਬ_fixed_part = 0xffu;
                                ਨਹੀਂਤਾਂ {
                                        j = prefixlen_fixed_part % 8;
                                        ਨਕਾਬ_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        ਜੇ (i < (prefixlen + 7) / 8) {
                                ਜੇ (i < prefixlen / 8)
                                        ਨਕਾਬ_3 = 0xffu ^ ਨਕਾਬ_fixed_part;
                                ਨਹੀਂਤਾਂ {
                                        j = prefixlen % 8;
                                        ਨਕਾਬ_3 = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ ਨਕਾਬ_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= ਨਕਾਬ_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & ਨਕਾਬ_3;
                }

                ਵਾਪਸ 1;
        }

        ਵਾਪਸ -EAFNOSUPPORT;
}

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_range(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਵਿੱਚ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen,
                ਸੰਘ ਵਿੱਚ_addr_union *ret_ਸ਼ੁਰੂ,
                ਸੰਘ ਵਿੱਚ_addr_union *ret_ਸਿਰਾ) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        ਚੋਣ (family) {
        ਹਾਲਤ AF_INET:
                ਜੇ (prefixlen > 32)
                        ਵਾਪਸ -EINVAL;
                ਰੋਕੋ;
        ਹਾਲਤ AF_INET6:
                ਜੇ (prefixlen > 128)
                        ਵਾਪਸ -EINVAL;
                ਰੋਕੋ;
        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }

        ਜੇ (prefixlen == 0) {
                /* shortcut for trivial case */
                ਜੇ (ret_ਸ਼ੁਰੂ)
                        *ret_ਸ਼ੁਰੂ = ਵਿੱਚ_ADDR_NULL;
                ਜੇ (ret_ਸਿਰਾ)
                        *ret_ਸਿਰਾ = ਵਿੱਚ_ADDR_NULL;
                ਵਾਪਸ 0;
        }

        ਸੰਘ ਵਿੱਚ_addr_union ਸ਼ੁਰੂ = *ਵਿੱਚ;
        r = ਵਿੱਚ_addr_ਨਕਾਬ(family, &ਸ਼ੁਰੂ, prefixlen);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਸੰਘ ਵਿੱਚ_addr_union ਸਿਰਾ = ਸ਼ੁਰੂ;
        uint8_t ਜੋੜੋ = UINT8_C(1) << ((FAMILY_ਪਤਾ_ਮਾਪ(family) * 8 - prefixlen) % 8);
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = DIV_ਗੇੜਾ_ਉੱਪਰ(prefixlen, 8); i-- > 0;)
                ਜੇ (ਜੋੜੋ > UINT8_MAX - ਸਿਰਾ.ਬਾਈਟਾਂ[i]) {
                        ਸਿਰਾ.ਬਾਈਟਾਂ[i] = 0;
                        ਜੋੜੋ = 1;
                } ਨਹੀਂਤਾਂ {
                        ਸਿਰਾ.ਬਾਈਟਾਂ[i] += ਜੋੜੋ;
                        ਰੋਕੋ;
                }

        ਜੇ (ret_ਸ਼ੁਰੂ)
                *ret_ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
        ਜੇ (ret_ਸਿਰਾ)
                *ret_ਸਿਰਾ = ਸਿਰਾ;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *x = ਨਿਰਾ;
        size_t l;

        ਪ੍ਰਤਿਜ੍ਞਾ(u);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (family == AF_INET)
                l = INET_ADDRSTRLEN;
        ਨਹੀਂਤਾਂ ਜੇ (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ -EAFNOSUPPORT;

        x = new(ਅੱਖਰ, l);
        ਜੇ (!x)
                ਵਾਪਸ -ENOMEM;

        errno = 0;
        ਜੇ (!typesafe_inet_ntop(family, u, x, l))
                ਵਾਪਸ errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ਵਾਪਸ 0;
}

ਅਚਲ ਅੱਖਰ* typesafe_inet_ntop(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a, ਅੱਖਰ *buf, size_t len) {
        ਵਾਪਸ inet_ntop(family, a, buf, len);
}

ਅਚਲ ਅੱਖਰ* typesafe_inet_ntop4(ਅਚਲ ਬਣਤਰ in_addr *a, ਅੱਖਰ *buf, size_t len) {
        ਵਾਪਸ inet_ntop(AF_INET, a, buf, len);
}

ਅਚਲ ਅੱਖਰ* typesafe_inet_ntop6(ਅਚਲ ਬਣਤਰ in6_addr *a, ਅੱਖਰ *buf, size_t len) {
        ਵਾਪਸ inet_ntop(AF_INET6, a, buf, len);
}

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u,
                ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen,
                ਅੱਖਰ *buf,
                size_t buf_len) {

        ਪ੍ਰਤਿਜ੍ਞਾ(u);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(family, AF_INET, AF_INET6))
                ਵਾਪਸ -EAFNOSUPPORT;

        errno = 0;
        ਜੇ (!typesafe_inet_ntop(family, u, buf, buf_len))
                ਵਾਪਸ errno_or_else(ENOSPC);

        size_t l = ਲੜੀ_ਲੰਬਾਈ(buf);
        ਜੇ (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ਵਾਪਸ -ENOSPC;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_ਪੋਰਟ_ifindex_name_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, uint16_t port, ਪੂਰਨ ifindex, ਅਚਲ ਅੱਖਰ *server_name, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ip_str = ਨਿਰਾ, *x = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(family, AF_INET, AF_INET6));
        ਪ੍ਰਤਿਜ੍ਞਾ(u);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = ਵਿੱਚ_addr_ਮੰਜ਼ਿਲ_string(family, u, &ip_str);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (family == AF_INET6) {
                r = ਵਿੱਚ_addr_is_link_local(family, u);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ifindex = 0;
        } ਨਹੀਂਤਾਂ
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        ਜੇ (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ਵਾਪਸ 0;
        }

        ਅਚਲ ਅੱਖਰ *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        ਜੇ (port > 0) {
                ਜੇ (family == AF_INET6) {
                        ਜੇ (ifindex > 0)
                                x = asprintf_ਸੁਰੱਖਿਅਤ("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        ਨਹੀਂਤਾਂ
                                x = asprintf_ਸੁਰੱਖਿਅਤ("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } ਨਹੀਂਤਾਂ
                        x = asprintf_ਸੁਰੱਖਿਅਤ("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } ਨਹੀਂਤਾਂ {
                ਜੇ (ifindex > 0)
                        x = asprintf_ਸੁਰੱਖਿਅਤ("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                ਨਹੀਂਤਾਂ
                        x = strjoin(ip_str, separator, server_name);
        }
        ਜੇ (!x)
                ਵਾਪਸ -ENOMEM;

        *ret = TAKE_PTR(x);
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_from_string(ਪੂਰਨ family, ਅਚਲ ਅੱਖਰ *s, ਸੰਘ ਵਿੱਚ_addr_union *ret) {
        ਸੰਘ ਵਿੱਚ_addr_union ਬਫਰ;
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(family, AF_INET, AF_INET6))
                ਵਾਪਸ -EAFNOSUPPORT;

        errno = 0;
        ਜੇ (inet_pton(family, s, ret ?: &ਬਫਰ) <= 0)
                ਵਾਪਸ errno_or_else(EINVAL);

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_from_string_auto(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret_family, ਸੰਘ ਵਿੱਚ_addr_union *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = ਵਿੱਚ_addr_from_string(AF_INET, s, ret);
        ਜੇ (r >= 0) {
                ਜੇ (ret_family)
                        *ret_family = AF_INET;
                ਵਾਪਸ 0;
        }

        r = ਵਿੱਚ_addr_from_string(AF_INET6, s, ret);
        ਜੇ (r >= 0) {
                ਜੇ (ret_family)
                        *ret_family = AF_INET6;
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ -EINVAL;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ in4_addr_netmask_ਮੰਜ਼ਿਲ_prefixlen(ਅਚਲ ਬਣਤਰ in_addr *addr) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        ਵਾਪਸ 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
ਬਣਤਰ in_addr* in4_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(ਬਣਤਰ in_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        ਜੇ (prefixlen == 0)
                addr->s_addr = 0;
        ਨਹੀਂਤਾਂ
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ਵਾਪਸ addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
ਬਣਤਰ in6_addr* in6_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(ਬਣਤਰ in6_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(prefixlen <= 128);

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 16; i++) {
                uint8_t ਨਕਾਬ_3;

                ਜੇ (prefixlen >= 8) {
                        ਨਕਾਬ_3 = 0xFF;
                        prefixlen -= 8;
                } ਨਹੀਂਤਾਂ ਜੇ (prefixlen > 0) {
                        ਨਕਾਬ_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ਨਹੀਂਤਾਂ {
                        ਪ੍ਰਤਿਜ੍ਞਾ(prefixlen == 0);
                        ਨਕਾਬ_3 = 0;
                }

                addr->s6_addr[i] = ਨਕਾਬ_3;
        }

        ਵਾਪਸ addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
ਪੂਰਨ ਵਿੱਚ_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(ਪੂਰਨ family, ਸੰਘ ਵਿੱਚ_addr_union *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        ਚੋਣ (family) {
        ਹਾਲਤ AF_INET:
                in4_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(&addr->ਵਿੱਚ, prefixlen);
                ਵਾਪਸ 0;
        ਹਾਲਤ AF_INET6:
                in6_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(&addr->in6, prefixlen);
                ਵਾਪਸ 0;
        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਪੂਰਨ in4_addr_default_prefixlen(ਅਚਲ ਬਣਤਰ in_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(prefixlen);

        ਜੇ (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        ਨਹੀਂਤਾਂ ਜੇ (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        ਨਹੀਂਤਾਂ ਜੇ (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        ਨਹੀਂਤਾਂ
                /* class D or E, no default prefixlen */
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ 0;
}

ਪੂਰਨ in4_addr_default_subnet_ਨਕਾਬ(ਅਚਲ ਬਣਤਰ in_addr *addr, ਬਣਤਰ in_addr *ਨਕਾਬ_3) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਕਾਬ_3);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        in4_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(ਨਕਾਬ_3, prefixlen);
        ਵਾਪਸ 0;
}

ਪੂਰਨ in4_addr_ਨਕਾਬ(ਬਣਤਰ in_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen) {
        ਬਣਤਰ in_addr ਨਕਾਬ_3;

        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        ਜੇ (!in4_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(&ਨਕਾਬ_3, prefixlen))
                ਵਾਪਸ -EINVAL;

        addr->s_addr &= ਨਕਾਬ_3.s_addr;
        ਵਾਪਸ 0;
}

ਪੂਰਨ in6_addr_ਨਕਾਬ(ਬਣਤਰ in6_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ i;

        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        ਲਈ (i = 0; i < 16; i++) {
                uint8_t ਨਕਾਬ_3;

                ਜੇ (prefixlen >= 8) {
                        ਨਕਾਬ_3 = 0xFF;
                        prefixlen -= 8;
                } ਨਹੀਂਤਾਂ ਜੇ (prefixlen > 0) {
                        ਨਕਾਬ_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ਨਹੀਂਤਾਂ {
                        ਪ੍ਰਤਿਜ੍ਞਾ(prefixlen == 0);
                        ਨਕਾਬ_3 = 0;
                }

                addr->s6_addr[i] &= ਨਕਾਬ_3;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_ਨਕਾਬ(ਪੂਰਨ family, ਸੰਘ ਵਿੱਚ_addr_union *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        ਚੋਣ (family) {
        ਹਾਲਤ AF_INET:
                ਵਾਪਸ in4_addr_ਨਕਾਬ(&addr->ਵਿੱਚ, prefixlen);
        ਹਾਲਤ AF_INET6:
                ਵਾਪਸ in6_addr_ਨਕਾਬ(&addr->in6, prefixlen);
        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਪੂਰਨ in4_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(
                ਅਚਲ ਬਣਤਰ in_addr *ਅਗੇਤਰ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen,
                ਅਚਲ ਬਣਤਰ in_addr *ਪਤਾ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪਤਾ_prefixlen) {

        ਬਣਤਰ in_addr masked_ਅਗੇਤਰ, masked_ਪਤਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਪਤਾ);

        ਜੇ (prefixlen > ਪਤਾ_prefixlen)
                ਵਾਪਸ false;

        masked_ਅਗੇਤਰ = *ਅਗੇਤਰ;
        r = in4_addr_ਨਕਾਬ(&masked_ਅਗੇਤਰ, prefixlen);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        masked_ਪਤਾ = *ਪਤਾ;
        r = in4_addr_ਨਕਾਬ(&masked_ਪਤਾ, prefixlen);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ in4_addr_equal(&masked_ਅਗੇਤਰ, &masked_ਪਤਾ);
}

ਪੂਰਨ in6_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(
                ਅਚਲ ਬਣਤਰ in6_addr *ਅਗੇਤਰ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen,
                ਅਚਲ ਬਣਤਰ in6_addr *ਪਤਾ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪਤਾ_prefixlen) {

        ਬਣਤਰ in6_addr masked_ਅਗੇਤਰ, masked_ਪਤਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਪਤਾ);

        ਜੇ (prefixlen > ਪਤਾ_prefixlen)
                ਵਾਪਸ false;

        masked_ਅਗੇਤਰ = *ਅਗੇਤਰ;
        r = in6_addr_ਨਕਾਬ(&masked_ਅਗੇਤਰ, prefixlen);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        masked_ਪਤਾ = *ਪਤਾ;
        r = in6_addr_ਨਕਾਬ(&masked_ਪਤਾ, prefixlen);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ in6_addr_equal(&masked_ਅਗੇਤਰ, &masked_ਪਤਾ);
}

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਅਗੇਤਰ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਪਤਾ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪਤਾ_prefixlen) {

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਪਤਾ);

        ਚੋਣ (family) {
        ਹਾਲਤ AF_INET:
                ਵਾਪਸ in4_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(&ਅਗੇਤਰ->ਵਿੱਚ, prefixlen, &ਪਤਾ->ਵਿੱਚ, ਪਤਾ_prefixlen);
        ਹਾਲਤ AF_INET6:
                ਵਾਪਸ in6_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(&ਅਗੇਤਰ->in6, prefixlen, &ਪਤਾ->in6, ਪਤਾ_prefixlen);
        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਪੂਰਨ ਵਿੱਚ_addr_parse_prefixlen(ਪੂਰਨ family, ਅਚਲ ਅੱਖਰ *p, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ret) {
        uint8_t u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(family, AF_INET, AF_INET6))
                ਵਾਪਸ -EAFNOSUPPORT;

        r = ਸੁਰੱਖਿਅਤ_atou8(p, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (u > FAMILY_ਪਤਾ_ਮਾਪ(family) * 8)
                ਵਾਪਸ -ERANGE;

        *ret = u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_from_string(
                ਅਚਲ ਅੱਖਰ *p,
                ਪੂਰਨ family,
                ਸੰਘ ਵਿੱਚ_addr_union *ret_ਅਗੇਤਰ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ret_prefixlen) {

        _cleanup_free_ ਅੱਖਰ *str = ਨਿਰਾ;
        ਸੰਘ ਵਿੱਚ_addr_union ਬਫਰ;
        ਅਚਲ ਅੱਖਰ *e, *l;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ k;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(family, AF_INET, AF_INET6))
                ਵਾਪਸ -EAFNOSUPPORT;

        e = strchr(p, '/');
        ਜੇ (e) {
                str = strndup(p, e - p);
                ਜੇ (!str)
                        ਵਾਪਸ -ENOMEM;

                l = str;
        } ਨਹੀਂਤਾਂ
                l = p;

        r = ਵਿੱਚ_addr_from_string(family, l, &ਬਫਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (e) {
                r = ਵਿੱਚ_addr_parse_prefixlen(family, e+1, &k);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        } ਨਹੀਂਤਾਂ
                k = FAMILY_ਪਤਾ_ਮਾਪ(family) * 8;

        ਜੇ (ret_ਅਗੇਤਰ)
                *ret_ਅਗੇਤਰ = ਬਫਰ;
        ਜੇ (ret_prefixlen)
                *ret_prefixlen = k;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_from_string_auto_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *p,
                ਵਿੱਚ_Addr_ਅਗੇਤਰ_Len_ਬਹੁਲਕ ਬਹੁਲਕ,
                ਪੂਰਨ *ret_family,
                ਸੰਘ ਵਿੱਚ_addr_union *ret_ਅਗੇਤਰ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ret_prefixlen) {

        _cleanup_free_ ਅੱਖਰ *str = ਨਿਰਾ;
        ਸੰਘ ਵਿੱਚ_addr_union ਬਫਰ;
        ਅਚਲ ਅੱਖਰ *e, *l;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ k;
        ਪੂਰਨ family, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        e = strchr(p, '/');
        ਜੇ (e) {
                str = strndup(p, e - p);
                ਜੇ (!str)
                        ਵਾਪਸ -ENOMEM;

                l = str;
        } ਨਹੀਂਤਾਂ
                l = p;

        r = ਵਿੱਚ_addr_from_string_auto(l, &family, &ਬਫਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (e) {
                r = ਵਿੱਚ_addr_parse_prefixlen(family, e+1, &k);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        } ਨਹੀਂਤਾਂ
                ਚੋਣ (ਬਹੁਲਕ) {
                ਹਾਲਤ PREFIXLEN_ਭਰਿਆ:
                        k = FAMILY_ਪਤਾ_ਮਾਪ(family) * 8;
                        ਰੋਕੋ;
                ਹਾਲਤ PREFIXLEN_REFUSE:
                        ਵਾਪਸ -ENOANO; /* To distinguish this error from others. */
                ਮੂਲ:
                        assert_not_reached();
                }

        ਜੇ (ret_family)
                *ret_family = family;
        ਜੇ (ret_ਅਗੇਤਰ)
                *ret_ਅਗੇਤਰ = ਬਫਰ;
        ਜੇ (ret_prefixlen)
                *ret_prefixlen = k;

        ਵਾਪਸ 0;
}

ਖਾਲੀ ਵਿੱਚ_addr_hash_func(ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, ਪੂਰਨ family, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(u);
        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        siphash24_compress(u->ਬਾਈਟਾਂ, FAMILY_ਪਤਾ_ਮਾਪ(family), state);
}

ਖਾਲੀ ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_func(ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *a, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        siphash24_compress_typesafe(a->family, state);
        ਵਿੱਚ_addr_hash_func(&a->ਪਤਾ, a->family, state);
}

ਪੂਰਨ ਵਿੱਚ_addr_ਅੰਕੜੇ_compare_func(ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *x, ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *y) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(x);
        ਪ੍ਰਤਿਜ੍ਞਾ(y);

        r = CMP(x->family, y->family);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਵਾਪਸ memcmp(&x->ਪਤਾ, &y->ਪਤਾ, FAMILY_ਪਤਾ_ਮਾਪ(x->family));
}

DEFINE_HASH_OPS(
        ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_ops,
        ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ,
        ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_func,
        ਵਿੱਚ_addr_ਅੰਕੜੇ_compare_func);

DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(
        ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_ops_free,
        ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ,
        ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_func,
        ਵਿੱਚ_addr_ਅੰਕੜੇ_compare_func,
        ਮੁਕਤ);

ਖਾਲੀ in6_addr_hash_func(ਅਚਲ ਬਣਤਰ in6_addr *addr, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        siphash24_compress_typesafe(*addr, state);
}

ਪੂਰਨ in6_addr_compare_func(ਅਚਲ ਬਣਤਰ in6_addr *a, ਅਚਲ ਬਣਤਰ in6_addr *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ memcmp(a, b, ਆਕਾਰ(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        ਬਣਤਰ in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(
        in6_addr_hash_ops_free,
        ਬਣਤਰ in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        ਮੁਕਤ);
