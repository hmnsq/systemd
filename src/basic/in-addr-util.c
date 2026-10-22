/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#ଅନ୍ତର୍ଭାଵ <endian.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "hash-funcs.h"
#ଅନ୍ତର୍ଭାଵ "in-addr-util.h"
#ଅନ୍ତର୍ଭାଵ "logarithm.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "random-util.h"
#ଅନ୍ତର୍ଭାଵ "siphash24.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

bool in4_addr_is_null(ଅଚଳ ସଂରଚନା in_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ a->s_addr == 0;
}

bool in6_addr_is_null(ଅଚଳ ସଂରଚନା in6_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ eqzero(a->s6_addr32);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_null(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) {
        ପ୍ରତିଜ୍ଞା(u);

        ଯଦି (family == AF_INET)
                ଫେରନ୍ତୁ in4_addr_is_null(&u->in);

        ଯଦି (family == AF_INET6)
                ଫେରନ୍ତୁ in6_addr_is_null(&u->in6);

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ଅଚଳ ସଂରଚନା in_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ଅଚଳ ସଂରଚନା in_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଯଦି (!in4_addr_is_link_local(a))
                ଫେରନ୍ତୁ false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        ଫେରନ୍ତୁ !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ଅଚଳ ସଂରଚନା in6_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_link_local(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) {
        ପ୍ରତିଜ୍ଞା(u);

        ଯଦି (family == AF_INET)
                ଫେରନ୍ତୁ in4_addr_is_link_local(&u->in);

        ଯଦି (family == AF_INET6)
                ଫେରନ୍ତୁ in6_addr_is_link_local(&u->in6);

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ଅଚଳ ସଂରଚନା in6_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        /* ff02::1 */
        ଫେରନ୍ତୁ be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ଅଚଳ ସଂରଚନା in_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ଅଚଳ ସଂରଚନା in6_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ a->s6_addr[0] == 0xff;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_multicast(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) {
        ପ୍ରତିଜ୍ଞା(u);

        ଯଦି (family == AF_INET)
                ଫେରନ୍ତୁ in4_addr_is_multicast(&u->in);

        ଯଦି (family == AF_INET6)
                ଫେରନ୍ତୁ in6_addr_is_multicast(&u->in6);

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ଅଚଳ ସଂରଚନା in_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ଅଚଳ ସଂରଚନା in_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        /* All of 127.x.x.x is localhost. */
        ଫେରନ୍ତୁ (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ଅଚଳ ସଂରଚନା in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        ଫେରନ୍ତୁ !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ସ୍ଥିର bool in6_addr_is_loopback(ଅଚଳ ସଂରଚନା in6_addr *a) {
        ଫେରନ୍ତୁ memcmp(a, &(ସଂରଚନା in6_addr) IN6ADDR_LOOPBACK_INIT, ଆକାର(ସଂରଚନା in6_addr)) == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_localhost(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) {
        ପ୍ରତିଜ୍ଞା(u);

        ଯଦି (family == AF_INET)
                ଫେରନ୍ତୁ in4_addr_is_localhost(&u->in);

        ଯଦି (family == AF_INET6)
                ଫେରନ୍ତୁ in6_addr_is_loopback(&u->in6);

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_is_localhost_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u) {
        ପ୍ରତିଜ୍ଞା(u);

        ଯଦି (family == AF_INET)
                /* 127.0.0.1 */
                ଫେରନ୍ତୁ be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        ଯଦି (family == AF_INET6)
                ଫେରନ୍ତୁ in6_addr_is_loopback(&u->in6);

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(ଅଚଳ ସଂରଚନା in6_addr *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ଅଚଳ ସଂରଚନା in_addr *a, ଅଚଳ ସଂରଚନା in_addr *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ a->s_addr == b->s_addr;
}

bool in6_addr_equal(ଅଚଳ ସଂରଚନା in6_addr *a, ଅଚଳ ସଂରଚନା in6_addr *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ memcmp(a, b, ଆକାର(ସଂରଚନା in6_addr)) == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_equal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *a, ଅଚଳ ସଂଘ in_addr_union *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଯଦି (family == AF_INET)
                ଫେରନ୍ତୁ in4_addr_equal(&a->in, &b->in);

        ଯଦି (family == AF_INET6)
                ଫେରନ୍ତୁ in6_addr_equal(&a->in6, &b->in6);

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                ଅଚଳ ସଂରଚନା in_addr *a,
                unsigned aprefixlen,
                ଅଚଳ ସଂରଚନା in_addr *b,
                unsigned bprefixlen) {

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (ଆକାର(ସଂରଚନା in_addr) * 8));
        ଯଦି (m == 0)
                ଫେରନ୍ତୁ true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        ଫେରନ୍ତୁ (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                ଅଚଳ ସଂରଚନା in6_addr *a,
                unsigned aprefixlen,
                ଅଚଳ ସଂରଚନା in6_addr *b,
                unsigned bprefixlen) {

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (ଆକାର(ସଂରଚନା in6_addr) * 8));
        ଯଦି (m == 0)
                ଫେରନ୍ତୁ true;

        ପାଇଁ (size_t i = 0; i < ଆକାର(ସଂରଚନା in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                ଯଦି ((x & n) != 0)
                        ଫେରନ୍ତୁ false;

                ଯଦି (m <= 8)
                        ଭାଙ୍ଗ;

                m -= 8;
        }

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_intersect(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *a,
                unsigned aprefixlen,
                ଅଚଳ ସଂଘ in_addr_union *b,
                unsigned bprefixlen) {

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        /* Checks whether there are any addresses that are in both networks. */

        ଯଦି (family == AF_INET)
                ଫେରନ୍ତୁ in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        ଯଦି (family == AF_INET6)
                ଫେରନ୍ତୁ in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_random_prefix(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ସଂଘ in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        ପ୍ରତିଜ୍ଞା(u);

        /* Random network part of an address by one. */

        ଯଦି (prefixlen <= 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (family == AF_INET) {
                uint32_t c, n;

                ଯଦି (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                ଯଦି (prefixlen > 32)
                        prefixlen = 32;
                ଯଦି (prefixlen_fixed_part >= prefixlen)
                        ଫେରନ୍ତୁ -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, ଆକାର(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                ଫେରନ୍ତୁ 1;
        }

        ଯଦି (family == AF_INET6) {
                ସଂରଚନା in6_addr n;
                unsigned i, j;

                ଯଦି (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                ଯଦି (prefixlen > 128)
                        prefixlen = 128;
                ଯଦି (prefixlen_fixed_part >= prefixlen)
                        ଫେରନ୍ତୁ -EINVAL;

                random_bytes(&n, ଆକାର(n));

                ପାଇଁ (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        ଯଦି (i < (prefixlen_fixed_part + 7) / 8) {
                                ଯଦି (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                ଅନ୍ୟଥା {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        ଯଦି (i < (prefixlen + 7) / 8) {
                                ଯଦି (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                ଅନ୍ୟଥା {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                ଫେରନ୍ତୁ 1;
        }

        ଫେରନ୍ତୁ -EAFNOSUPPORT;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_range(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *in,
                unsigned prefixlen,
                ସଂଘ in_addr_union *ret_start,
                ସଂଘ in_addr_union *ret_end) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        ବାଛ (family) {
        ସ୍ଥିତି AF_INET:
                ଯଦି (prefixlen > 32)
                        ଫେରନ୍ତୁ -EINVAL;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି AF_INET6:
                ଯଦି (prefixlen > 128)
                        ଫେରନ୍ତୁ -EINVAL;
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }

        ଯଦି (prefixlen == 0) {
                /* shortcut for trivial case */
                ଯଦି (ret_start)
                        *ret_start = IN_ADDR_NULL;
                ଯଦି (ret_end)
                        *ret_end = IN_ADDR_NULL;
                ଫେରନ୍ତୁ 0;
        }

        ସଂଘ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ସଂଘ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        ପାଇଁ (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                ଯଦି (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } ଅନ୍ୟଥା {
                        end.bytes[i] += add;
                        ଭାଙ୍ଗ;
                }

        ଯଦି (ret_start)
                *ret_start = start;
        ଯଦି (ret_end)
                *ret_end = end;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *x = ଖାଲି;
        size_t l;

        ପ୍ରତିଜ୍ଞା(u);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (family == AF_INET)
                l = INET_ADDRSTRLEN;
        ଅନ୍ୟଥା ଯଦି (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        x = new(ଅକ୍ଷର, l);
        ଯଦି (!x)
                ଫେରନ୍ତୁ -ENOMEM;

        errno = 0;
        ଯଦି (!typesafe_inet_ntop(family, u, x, l))
                ଫେରନ୍ତୁ errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        ଫେରନ୍ତୁ 0;
}

ଅଚଳ ଅକ୍ଷର* typesafe_inet_ntop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *a, ଅକ୍ଷର *buf, size_t len) {
        ଫେରନ୍ତୁ inet_ntop(family, a, buf, len);
}

ଅଚଳ ଅକ୍ଷର* typesafe_inet_ntop4(ଅଚଳ ସଂରଚନା in_addr *a, ଅକ୍ଷର *buf, size_t len) {
        ଫେରନ୍ତୁ inet_ntop(AF_INET, a, buf, len);
}

ଅଚଳ ଅକ୍ଷର* typesafe_inet_ntop6(ଅଚଳ ସଂରଚନା in6_addr *a, ଅକ୍ଷର *buf, size_t len) {
        ଫେରନ୍ତୁ inet_ntop(AF_INET6, a, buf, len);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_to_string(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *u,
                unsigned prefixlen,
                ଅକ୍ଷର *buf,
                size_t buf_len) {

        ପ୍ରତିଜ୍ଞା(u);
        ପ୍ରତିଜ୍ଞା(buf);

        ଯଦି (!IN_SET(family, AF_INET, AF_INET6))
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        errno = 0;
        ଯଦି (!typesafe_inet_ntop(family, u, buf, buf_len))
                ଫେରନ୍ତୁ errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        ଯଦି (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                ଫେରନ୍ତୁ -ENOSPC;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_port_ifindex_name_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *u, uint16_t port, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, ଅଚଳ ଅକ୍ଷର *server_name, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *ip_str = ଖାଲି, *x = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(IN_SET(family, AF_INET, AF_INET6));
        ପ୍ରତିଜ୍ଞା(u);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ifindex = 0;
        } ଅନ୍ୟଥା
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        ଯଦି (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                ଫେରନ୍ତୁ 0;
        }

        ଅଚଳ ଅକ୍ଷର *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        ଯଦି (port > 0) {
                ଯଦି (family == AF_INET6) {
                        ଯଦି (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        ଅନ୍ୟଥା
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } ଅନ୍ୟଥା
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } ଅନ୍ୟଥା {
                ଯଦି (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                ଅନ୍ୟଥା
                        x = strjoin(ip_str, separator, server_name);
        }
        ଯଦି (!x)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = TAKE_PTR(x);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_from_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ଅକ୍ଷର *s, ସଂଘ in_addr_union *ret) {
        ସଂଘ in_addr_union buffer;
        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (!IN_SET(family, AF_INET, AF_INET6))
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        errno = 0;
        ଯଦି (inet_pton(family, s, ret ?: &buffer) <= 0)
                ଫେରନ୍ତୁ errno_or_else(EINVAL);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_from_string_auto(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_family, ସଂଘ in_addr_union *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        r = in_addr_from_string(AF_INET, s, ret);
        ଯଦି (r >= 0) {
                ଯଦି (ret_family)
                        *ret_family = AF_INET;
                ଫେରନ୍ତୁ 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        ଯଦି (r >= 0) {
                ଯଦି (ret_family)
                        *ret_family = AF_INET6;
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ -EINVAL;
}

unsigned ଅକ୍ଷର in4_addr_netmask_to_prefixlen(ଅଚଳ ସଂରଚନା in_addr *addr) {
        ପ୍ରତିଜ୍ଞା(addr);

        ଫେରନ୍ତୁ 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
ସଂରଚନା in_addr* in4_addr_prefixlen_to_netmask(ସଂରଚନା in_addr *addr, unsigned ଅକ୍ଷର prefixlen) {
        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        ଯଦି (prefixlen == 0)
                addr->s_addr = 0;
        ଅନ୍ୟଥା
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        ଫେରନ୍ତୁ addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
ସଂରଚନା in6_addr* in6_addr_prefixlen_to_netmask(ସଂରଚନା in6_addr *addr, unsigned ଅକ୍ଷର prefixlen) {
        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(prefixlen <= 128);

        ପାଇଁ (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                ଯଦି (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } ଅନ୍ୟଥା ଯଦି (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ଅନ୍ୟଥା {
                        ପ୍ରତିଜ୍ଞା(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        ଫେରନ୍ତୁ addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefixlen_to_netmask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂଘ in_addr_union *addr, unsigned ଅକ୍ଷର prefixlen) {
        ପ୍ରତିଜ୍ଞା(addr);

        ବାଛ (family) {
        ସ୍ଥିତି AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                ଫେରନ୍ତୁ 0;
        ସ୍ଥିତି AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                ଫେରନ୍ତୁ 0;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_default_prefixlen(ଅଚଳ ସଂରଚନା in_addr *addr, unsigned ଅକ୍ଷର *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(prefixlen);

        ଯଦି (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        ଅନ୍ୟଥା ଯଦି (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        ଅନ୍ୟଥା ଯଦି (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        ଅନ୍ୟଥା
                /* class D or E, no default prefixlen */
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_default_subnet_mask(ଅଚଳ ସଂରଚନା in_addr *addr, ସଂରଚନା in_addr *mask) {
        unsigned ଅକ୍ଷର prefixlen;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_mask(ସଂରଚନା in_addr *addr, unsigned ଅକ୍ଷର prefixlen) {
        ସଂରଚନା in_addr mask;

        ପ୍ରତିଜ୍ଞା(addr);

        ଯଦି (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                ଫେରନ୍ତୁ -EINVAL;

        addr->s_addr &= mask.s_addr;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_mask(ସଂରଚନା in6_addr *addr, unsigned ଅକ୍ଷର prefixlen) {
        unsigned i;

        ପ୍ରତିଜ୍ଞା(addr);

        ପାଇଁ (i = 0; i < 16; i++) {
                uint8_t mask;

                ଯଦି (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } ଅନ୍ୟଥା ଯଦି (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ଅନ୍ୟଥା {
                        ପ୍ରତିଜ୍ଞା(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_mask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂଘ in_addr_union *addr, unsigned ଅକ୍ଷର prefixlen) {
        ପ୍ରତିଜ୍ଞା(addr);

        ବାଛ (family) {
        ସ୍ଥିତି AF_INET:
                ଫେରନ୍ତୁ in4_addr_mask(&addr->in, prefixlen);
        ସ୍ଥିତି AF_INET6:
                ଫେରନ୍ତୁ in6_addr_mask(&addr->in6, prefixlen);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in4_addr_prefix_covers_full(
                ଅଚଳ ସଂରଚନା in_addr *prefix,
                unsigned ଅକ୍ଷର prefixlen,
                ଅଚଳ ସଂରଚନା in_addr *address,
                unsigned ଅକ୍ଷର address_prefixlen) {

        ସଂରଚନା in_addr masked_prefix, masked_address;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(prefix);
        ପ୍ରତିଜ୍ଞା(address);

        ଯଦି (prefixlen > address_prefixlen)
                ଫେରନ୍ତୁ false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ in4_addr_equal(&masked_prefix, &masked_address);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_prefix_covers_full(
                ଅଚଳ ସଂରଚନା in6_addr *prefix,
                unsigned ଅକ୍ଷର prefixlen,
                ଅଚଳ ସଂରଚନା in6_addr *address,
                unsigned ଅକ୍ଷର address_prefixlen) {

        ସଂରଚନା in6_addr masked_prefix, masked_address;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(prefix);
        ପ୍ରତିଜ୍ଞା(address);

        ଯଦି (prefixlen > address_prefixlen)
                ଫେରନ୍ତୁ false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ in6_addr_equal(&masked_prefix, &masked_address);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_covers_full(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *prefix,
                unsigned ଅକ୍ଷର prefixlen,
                ଅଚଳ ସଂଘ in_addr_union *address,
                unsigned ଅକ୍ଷର address_prefixlen) {

        ପ୍ରତିଜ୍ଞା(prefix);
        ପ୍ରତିଜ୍ଞା(address);

        ବାଛ (family) {
        ସ୍ଥିତି AF_INET:
                ଫେରନ୍ତୁ in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        ସ୍ଥିତି AF_INET6:
                ଫେରନ୍ତୁ in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_parse_prefixlen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ଅକ୍ଷର *p, unsigned ଅକ୍ଷର *ret) {
        uint8_t u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!IN_SET(family, AF_INET, AF_INET6))
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (u > FAMILY_ADDRESS_SIZE(family) * 8)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_from_string(
                ଅଚଳ ଅକ୍ଷର *p,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ସଂଘ in_addr_union *ret_prefix,
                unsigned ଅକ୍ଷର *ret_prefixlen) {

        _cleanup_free_ ଅକ୍ଷର *str = ଖାଲି;
        ସଂଘ in_addr_union buffer;
        ଅଚଳ ଅକ୍ଷର *e, *l;
        unsigned ଅକ୍ଷର k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);

        ଯଦି (!IN_SET(family, AF_INET, AF_INET6))
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        e = strchr(p, '/');
        ଯଦି (e) {
                str = strndup(p, e - p);
                ଯଦି (!str)
                        ଫେରନ୍ତୁ -ENOMEM;

                l = str;
        } ଅନ୍ୟଥା
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        } ଅନ୍ୟଥା
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        ଯଦି (ret_prefix)
                *ret_prefix = buffer;
        ଯଦି (ret_prefixlen)
                *ret_prefixlen = k;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_prefix_from_string_auto_full(
                ଅଚଳ ଅକ୍ଷର *p,
                InAddrPrefixLenMode mode,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_family,
                ସଂଘ in_addr_union *ret_prefix,
                unsigned ଅକ୍ଷର *ret_prefixlen) {

        _cleanup_free_ ଅକ୍ଷର *str = ଖାଲି;
        ସଂଘ in_addr_union buffer;
        ଅଚଳ ଅକ୍ଷର *e, *l;
        unsigned ଅକ୍ଷର k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, r;

        ପ୍ରତିଜ୍ଞା(p);

        e = strchr(p, '/');
        ଯଦି (e) {
                str = strndup(p, e - p);
                ଯଦି (!str)
                        ଫେରନ୍ତୁ -ENOMEM;

                l = str;
        } ଅନ୍ୟଥା
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        } ଅନ୍ୟଥା
                ବାଛ (mode) {
                ସ୍ଥିତି PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି PREFIXLEN_REFUSE:
                        ଫେରନ୍ତୁ -ENOANO; /* To distinguish this error from others. */
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        assert_not_reached();
                }

        ଯଦି (ret_family)
                *ret_family = family;
        ଯଦି (ret_prefix)
                *ret_prefix = buffer;
        ଯଦି (ret_prefixlen)
                *ret_prefixlen = k;

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ in_addr_hash_func(ଅଚଳ ସଂଘ in_addr_union *u, ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(u);
        ପ୍ରତିଜ୍ଞା(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

ଶୂନ୍ୟ in_addr_data_hash_func(ଅଚଳ ସଂରଚନା in_addr_data *a, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_addr_data_compare_func(ଅଚଳ ସଂରଚନା in_addr_data *x, ଅଚଳ ସଂରଚନା in_addr_data *y) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(x);
        ପ୍ରତିଜ୍ଞା(y);

        r = CMP(x->family, y->family);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        ସଂରଚନା in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        ସଂରଚନା in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        ମୁକ୍ତ);

ଶୂନ୍ୟ in6_addr_hash_func(ଅଚଳ ସଂରଚନା in6_addr *addr, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(state);

        siphash24_compress_typesafe(*addr, state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in6_addr_compare_func(ଅଚଳ ସଂରଚନା in6_addr *a, ଅଚଳ ସଂରଚନା in6_addr *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ memcmp(a, b, ଆକାର(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        ସଂରଚନା in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(
        in6_addr_hash_ops_free,
        ସଂରଚନା in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        ମୁକ୍ତ);
