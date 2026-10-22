/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <arpa/inet.h>
#அந்தர்பாவ <endian.h>
#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "hash-funcs.h"
#அந்தர்பாவ "in-addr-util.h"
#அந்தர்பாவ "logarithm.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "random-util.h"
#அந்தர்பாவ "siphash24.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"

bool in4_addr_is_null(மாறிலி கட்டமைப்பு in_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு a->s_addr == 0;
}

bool in6_addr_is_null(மாறிலி கட்டமைப்பு in6_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு eqzero(a->s6_addr32);
}

முழு in_addr_is_null(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) {
        ப்ரதிஜ்ஞா(u);

        எனில் (family == AF_INET)
                திரும்பு in4_addr_is_null(&u->in);

        எனில் (family == AF_INET6)
                திரும்பு in6_addr_is_null(&u->in6);

        திரும்பு -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(மாறிலி கட்டமைப்பு in_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(மாறிலி கட்டமைப்பு in_addr *a) {
        ப்ரதிஜ்ஞா(a);

        எனில் (!in4_addr_is_link_local(a))
                திரும்பு false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        திரும்பு !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(மாறிலி கட்டமைப்பு in6_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

முழு in_addr_is_link_local(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) {
        ப்ரதிஜ்ஞா(u);

        எனில் (family == AF_INET)
                திரும்பு in4_addr_is_link_local(&u->in);

        எனில் (family == AF_INET6)
                திரும்பு in6_addr_is_link_local(&u->in6);

        திரும்பு -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(மாறிலி கட்டமைப்பு in6_addr *a) {
        ப்ரதிஜ்ஞா(a);

        /* ff02::1 */
        திரும்பு be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(மாறிலி கட்டமைப்பு in_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(மாறிலி கட்டமைப்பு in6_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு a->s6_addr[0] == 0xff;
}

முழு in_addr_is_multicast(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) {
        ப்ரதிஜ்ஞா(u);

        எனில் (family == AF_INET)
                திரும்பு in4_addr_is_multicast(&u->in);

        எனில் (family == AF_INET6)
                திரும்பு in6_addr_is_multicast(&u->in6);

        திரும்பு -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(மாறிலி கட்டமைப்பு in_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(மாறிலி கட்டமைப்பு in_addr *a) {
        ப்ரதிஜ்ஞா(a);

        /* All of 127.x.x.x is localhost. */
        திரும்பு (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(மாறிலி கட்டமைப்பு in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        திரும்பு !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

நிலையான bool in6_addr_is_loopback(மாறிலி கட்டமைப்பு in6_addr *a) {
        திரும்பு memcmp(a, &(கட்டமைப்பு in6_addr) IN6ADDR_LOOPBACK_INIT, அளவு(கட்டமைப்பு in6_addr)) == 0;
}

முழு in_addr_is_localhost(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) {
        ப்ரதிஜ்ஞா(u);

        எனில் (family == AF_INET)
                திரும்பு in4_addr_is_localhost(&u->in);

        எனில் (family == AF_INET6)
                திரும்பு in6_addr_is_loopback(&u->in6);

        திரும்பு -EAFNOSUPPORT;
}

முழு in_addr_is_localhost_one(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u) {
        ப்ரதிஜ்ஞா(u);

        எனில் (family == AF_INET)
                /* 127.0.0.1 */
                திரும்பு be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        எனில் (family == AF_INET6)
                திரும்பு in6_addr_is_loopback(&u->in6);

        திரும்பு -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(மாறிலி கட்டமைப்பு in6_addr *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(மாறிலி கட்டமைப்பு in_addr *a, மாறிலி கட்டமைப்பு in_addr *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு a->s_addr == b->s_addr;
}

bool in6_addr_equal(மாறிலி கட்டமைப்பு in6_addr *a, மாறிலி கட்டமைப்பு in6_addr *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு memcmp(a, b, அளவு(கட்டமைப்பு in6_addr)) == 0;
}

முழு in_addr_equal(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *a, மாறிலி ஒன்றிப்பு in_addr_union *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        எனில் (family == AF_INET)
                திரும்பு in4_addr_equal(&a->in, &b->in);

        எனில் (family == AF_INET6)
                திரும்பு in6_addr_equal(&a->in6, &b->in6);

        திரும்பு -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                மாறிலி கட்டமைப்பு in_addr *a,
                unsigned aprefixlen,
                மாறிலி கட்டமைப்பு in_addr *b,
                unsigned bprefixlen) {

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (அளவு(கட்டமைப்பு in_addr) * 8));
        எனில் (m == 0)
                திரும்பு true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        திரும்பு (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                மாறிலி கட்டமைப்பு in6_addr *a,
                unsigned aprefixlen,
                மாறிலி கட்டமைப்பு in6_addr *b,
                unsigned bprefixlen) {

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (அளவு(கட்டமைப்பு in6_addr) * 8));
        எனில் (m == 0)
                திரும்பு true;

        க்கு (size_t i = 0; i < அளவு(கட்டமைப்பு in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                எனில் ((x & n) != 0)
                        திரும்பு false;

                எனில் (m <= 8)
                        நிறுத்து;

                m -= 8;
        }

        திரும்பு true;
}

முழு in_addr_prefix_intersect(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *a,
                unsigned aprefixlen,
                மாறிலி ஒன்றிப்பு in_addr_union *b,
                unsigned bprefixlen) {

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        /* Checks whether there are any addresses that are in both networks. */

        எனில் (family == AF_INET)
                திரும்பு in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        எனில் (family == AF_INET6)
                திரும்பு in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        திரும்பு -EAFNOSUPPORT;
}

முழு in_addr_random_prefix(
                முழு family,
                ஒன்றிப்பு in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        ப்ரதிஜ்ஞா(u);

        /* Random network part of an address by one. */

        எனில் (prefixlen <= 0)
                திரும்பு 0;

        எனில் (family == AF_INET) {
                uint32_t c, n;

                எனில் (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                எனில் (prefixlen > 32)
                        prefixlen = 32;
                எனில் (prefixlen_fixed_part >= prefixlen)
                        திரும்பு -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, அளவு(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                திரும்பு 1;
        }

        எனில் (family == AF_INET6) {
                கட்டமைப்பு in6_addr n;
                unsigned i, j;

                எனில் (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                எனில் (prefixlen > 128)
                        prefixlen = 128;
                எனில் (prefixlen_fixed_part >= prefixlen)
                        திரும்பு -EINVAL;

                random_bytes(&n, அளவு(n));

                க்கு (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        எனில் (i < (prefixlen_fixed_part + 7) / 8) {
                                எனில் (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                இல்லையெனில் {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        எனில் (i < (prefixlen + 7) / 8) {
                                எனில் (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                இல்லையெனில் {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                திரும்பு 1;
        }

        திரும்பு -EAFNOSUPPORT;
}

முழு in_addr_prefix_range(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *in,
                unsigned prefixlen,
                ஒன்றிப்பு in_addr_union *ret_start,
                ஒன்றிப்பு in_addr_union *ret_end) {

        முழு r;

        ப்ரதிஜ்ஞா(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        தேர்வு (family) {
        நிலை AF_INET:
                எனில் (prefixlen > 32)
                        திரும்பு -EINVAL;
                நிறுத்து;
        நிலை AF_INET6:
                எனில் (prefixlen > 128)
                        திரும்பு -EINVAL;
                நிறுத்து;
        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }

        எனில் (prefixlen == 0) {
                /* shortcut for trivial case */
                எனில் (ret_start)
                        *ret_start = IN_ADDR_NULL;
                எனில் (ret_end)
                        *ret_end = IN_ADDR_NULL;
                திரும்பு 0;
        }

        ஒன்றிப்பு in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        எனில் (r < 0)
                திரும்பு r;

        ஒன்றிப்பு in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        க்கு (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                எனில் (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } இல்லையெனில் {
                        end.bytes[i] += add;
                        நிறுத்து;
                }

        எனில் (ret_start)
                *ret_start = start;
        எனில் (ret_end)
                *ret_end = end;

        திரும்பு 0;
}

முழு in_addr_to_string(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *x = வெற்று;
        size_t l;

        ப்ரதிஜ்ஞா(u);
        ப்ரதிஜ்ஞா(ret);

        எனில் (family == AF_INET)
                l = INET_ADDRSTRLEN;
        இல்லையெனில் எனில் (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        இல்லையெனில்
                திரும்பு -EAFNOSUPPORT;

        x = new(எழுத்து, l);
        எனில் (!x)
                திரும்பு -ENOMEM;

        errno = 0;
        எனில் (!typesafe_inet_ntop(family, u, x, l))
                திரும்பு errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        திரும்பு 0;
}

மாறிலி எழுத்து* typesafe_inet_ntop(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *a, எழுத்து *buf, size_t len) {
        திரும்பு inet_ntop(family, a, buf, len);
}

மாறிலி எழுத்து* typesafe_inet_ntop4(மாறிலி கட்டமைப்பு in_addr *a, எழுத்து *buf, size_t len) {
        திரும்பு inet_ntop(AF_INET, a, buf, len);
}

மாறிலி எழுத்து* typesafe_inet_ntop6(மாறிலி கட்டமைப்பு in6_addr *a, எழுத்து *buf, size_t len) {
        திரும்பு inet_ntop(AF_INET6, a, buf, len);
}

முழு in_addr_prefix_to_string(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *u,
                unsigned prefixlen,
                எழுத்து *buf,
                size_t buf_len) {

        ப்ரதிஜ்ஞா(u);
        ப்ரதிஜ்ஞா(buf);

        எனில் (!IN_SET(family, AF_INET, AF_INET6))
                திரும்பு -EAFNOSUPPORT;

        errno = 0;
        எனில் (!typesafe_inet_ntop(family, u, buf, buf_len))
                திரும்பு errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        எனில் (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                திரும்பு -ENOSPC;
        திரும்பு 0;
}

முழு in_addr_port_ifindex_name_to_string(முழு family, மாறிலி ஒன்றிப்பு in_addr_union *u, uint16_t port, முழு ifindex, மாறிலி எழுத்து *server_name, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *ip_str = வெற்று, *x = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(IN_SET(family, AF_INET, AF_INET6));
        ப்ரதிஜ்ஞா(u);
        ப்ரதிஜ்ஞா(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        ifindex = 0;
        } இல்லையெனில்
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        எனில் (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                திரும்பு 0;
        }

        மாறிலி எழுத்து *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        எனில் (port > 0) {
                எனில் (family == AF_INET6) {
                        எனில் (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        இல்லையெனில்
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } இல்லையெனில்
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } இல்லையெனில் {
                எனில் (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                இல்லையெனில்
                        x = strjoin(ip_str, separator, server_name);
        }
        எனில் (!x)
                திரும்பு -ENOMEM;

        *ret = TAKE_PTR(x);
        திரும்பு 0;
}

முழு in_addr_from_string(முழு family, மாறிலி எழுத்து *s, ஒன்றிப்பு in_addr_union *ret) {
        ஒன்றிப்பு in_addr_union buffer;
        ப்ரதிஜ்ஞா(s);

        எனில் (!IN_SET(family, AF_INET, AF_INET6))
                திரும்பு -EAFNOSUPPORT;

        errno = 0;
        எனில் (inet_pton(family, s, ret ?: &buffer) <= 0)
                திரும்பு errno_or_else(EINVAL);

        திரும்பு 0;
}

முழு in_addr_from_string_auto(மாறிலி எழுத்து *s, முழு *ret_family, ஒன்றிப்பு in_addr_union *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(s);

        r = in_addr_from_string(AF_INET, s, ret);
        எனில் (r >= 0) {
                எனில் (ret_family)
                        *ret_family = AF_INET;
                திரும்பு 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        எனில் (r >= 0) {
                எனில் (ret_family)
                        *ret_family = AF_INET6;
                திரும்பு 0;
        }

        திரும்பு -EINVAL;
}

unsigned எழுத்து in4_addr_netmask_to_prefixlen(மாறிலி கட்டமைப்பு in_addr *addr) {
        ப்ரதிஜ்ஞா(addr);

        திரும்பு 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
கட்டமைப்பு in_addr* in4_addr_prefixlen_to_netmask(கட்டமைப்பு in_addr *addr, unsigned எழுத்து prefixlen) {
        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        எனில் (prefixlen == 0)
                addr->s_addr = 0;
        இல்லையெனில்
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        திரும்பு addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
கட்டமைப்பு in6_addr* in6_addr_prefixlen_to_netmask(கட்டமைப்பு in6_addr *addr, unsigned எழுத்து prefixlen) {
        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(prefixlen <= 128);

        க்கு (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                எனில் (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } இல்லையெனில் எனில் (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } இல்லையெனில் {
                        ப்ரதிஜ்ஞா(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        திரும்பு addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
முழு in_addr_prefixlen_to_netmask(முழு family, ஒன்றிப்பு in_addr_union *addr, unsigned எழுத்து prefixlen) {
        ப்ரதிஜ்ஞா(addr);

        தேர்வு (family) {
        நிலை AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                திரும்பு 0;
        நிலை AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                திரும்பு 0;
        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

முழு in4_addr_default_prefixlen(மாறிலி கட்டமைப்பு in_addr *addr, unsigned எழுத்து *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(prefixlen);

        எனில் (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        இல்லையெனில் எனில் (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        இல்லையெனில் எனில் (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        இல்லையெனில்
                /* class D or E, no default prefixlen */
                திரும்பு -ERANGE;

        திரும்பு 0;
}

முழு in4_addr_default_subnet_mask(மாறிலி கட்டமைப்பு in_addr *addr, கட்டமைப்பு in_addr *mask) {
        unsigned எழுத்து prefixlen;
        முழு r;

        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        எனில் (r < 0)
                திரும்பு r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        திரும்பு 0;
}

முழு in4_addr_mask(கட்டமைப்பு in_addr *addr, unsigned எழுத்து prefixlen) {
        கட்டமைப்பு in_addr mask;

        ப்ரதிஜ்ஞா(addr);

        எனில் (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                திரும்பு -EINVAL;

        addr->s_addr &= mask.s_addr;
        திரும்பு 0;
}

முழு in6_addr_mask(கட்டமைப்பு in6_addr *addr, unsigned எழுத்து prefixlen) {
        unsigned i;

        ப்ரதிஜ்ஞா(addr);

        க்கு (i = 0; i < 16; i++) {
                uint8_t mask;

                எனில் (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } இல்லையெனில் எனில் (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } இல்லையெனில் {
                        ப்ரதிஜ்ஞா(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        திரும்பு 0;
}

முழு in_addr_mask(முழு family, ஒன்றிப்பு in_addr_union *addr, unsigned எழுத்து prefixlen) {
        ப்ரதிஜ்ஞா(addr);

        தேர்வு (family) {
        நிலை AF_INET:
                திரும்பு in4_addr_mask(&addr->in, prefixlen);
        நிலை AF_INET6:
                திரும்பு in6_addr_mask(&addr->in6, prefixlen);
        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

முழு in4_addr_prefix_covers_full(
                மாறிலி கட்டமைப்பு in_addr *prefix,
                unsigned எழுத்து prefixlen,
                மாறிலி கட்டமைப்பு in_addr *address,
                unsigned எழுத்து address_prefixlen) {

        கட்டமைப்பு in_addr masked_prefix, masked_address;
        முழு r;

        ப்ரதிஜ்ஞா(prefix);
        ப்ரதிஜ்ஞா(address);

        எனில் (prefixlen > address_prefixlen)
                திரும்பு false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        எனில் (r < 0)
                திரும்பு r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு in4_addr_equal(&masked_prefix, &masked_address);
}

முழு in6_addr_prefix_covers_full(
                மாறிலி கட்டமைப்பு in6_addr *prefix,
                unsigned எழுத்து prefixlen,
                மாறிலி கட்டமைப்பு in6_addr *address,
                unsigned எழுத்து address_prefixlen) {

        கட்டமைப்பு in6_addr masked_prefix, masked_address;
        முழு r;

        ப்ரதிஜ்ஞா(prefix);
        ப்ரதிஜ்ஞா(address);

        எனில் (prefixlen > address_prefixlen)
                திரும்பு false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        எனில் (r < 0)
                திரும்பு r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு in6_addr_equal(&masked_prefix, &masked_address);
}

முழு in_addr_prefix_covers_full(
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *prefix,
                unsigned எழுத்து prefixlen,
                மாறிலி ஒன்றிப்பு in_addr_union *address,
                unsigned எழுத்து address_prefixlen) {

        ப்ரதிஜ்ஞா(prefix);
        ப்ரதிஜ்ஞா(address);

        தேர்வு (family) {
        நிலை AF_INET:
                திரும்பு in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        நிலை AF_INET6:
                திரும்பு in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

முழு in_addr_parse_prefixlen(முழு family, மாறிலி எழுத்து *p, unsigned எழுத்து *ret) {
        uint8_t u;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        எனில் (!IN_SET(family, AF_INET, AF_INET6))
                திரும்பு -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (u > FAMILY_ADDRESS_SIZE(family) * 8)
                திரும்பு -ERANGE;

        *ret = u;
        திரும்பு 0;
}

முழு in_addr_prefix_from_string(
                மாறிலி எழுத்து *p,
                முழு family,
                ஒன்றிப்பு in_addr_union *ret_prefix,
                unsigned எழுத்து *ret_prefixlen) {

        _cleanup_free_ எழுத்து *str = வெற்று;
        ஒன்றிப்பு in_addr_union buffer;
        மாறிலி எழுத்து *e, *l;
        unsigned எழுத்து k;
        முழு r;

        ப்ரதிஜ்ஞா(p);

        எனில் (!IN_SET(family, AF_INET, AF_INET6))
                திரும்பு -EAFNOSUPPORT;

        e = strchr(p, '/');
        எனில் (e) {
                str = strndup(p, e - p);
                எனில் (!str)
                        திரும்பு -ENOMEM;

                l = str;
        } இல்லையெனில்
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                எனில் (r < 0)
                        திரும்பு r;
        } இல்லையெனில்
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        எனில் (ret_prefix)
                *ret_prefix = buffer;
        எனில் (ret_prefixlen)
                *ret_prefixlen = k;

        திரும்பு 0;
}

முழு in_addr_prefix_from_string_auto_full(
                மாறிலி எழுத்து *p,
                InAddrPrefixLenMode mode,
                முழு *ret_family,
                ஒன்றிப்பு in_addr_union *ret_prefix,
                unsigned எழுத்து *ret_prefixlen) {

        _cleanup_free_ எழுத்து *str = வெற்று;
        ஒன்றிப்பு in_addr_union buffer;
        மாறிலி எழுத்து *e, *l;
        unsigned எழுத்து k;
        முழு family, r;

        ப்ரதிஜ்ஞா(p);

        e = strchr(p, '/');
        எனில் (e) {
                str = strndup(p, e - p);
                எனில் (!str)
                        திரும்பு -ENOMEM;

                l = str;
        } இல்லையெனில்
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                எனில் (r < 0)
                        திரும்பு r;
        } இல்லையெனில்
                தேர்வு (mode) {
                நிலை PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        நிறுத்து;
                நிலை PREFIXLEN_REFUSE:
                        திரும்பு -ENOANO; /* To distinguish this error from others. */
                இயல்பு:
                        assert_not_reached();
                }

        எனில் (ret_family)
                *ret_family = family;
        எனில் (ret_prefix)
                *ret_prefix = buffer;
        எனில் (ret_prefixlen)
                *ret_prefixlen = k;

        திரும்பு 0;
}

வெறுமை in_addr_hash_func(மாறிலி ஒன்றிப்பு in_addr_union *u, முழு family, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(u);
        ப்ரதிஜ்ஞா(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

வெறுமை in_addr_data_hash_func(மாறிலி கட்டமைப்பு in_addr_data *a, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

முழு in_addr_data_compare_func(மாறிலி கட்டமைப்பு in_addr_data *x, மாறிலி கட்டமைப்பு in_addr_data *y) {
        முழு r;

        ப்ரதிஜ்ஞா(x);
        ப்ரதிஜ்ஞா(y);

        r = CMP(x->family, y->family);
        எனில் (r != 0)
                திரும்பு r;

        திரும்பு memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        கட்டமைப்பு in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        கட்டமைப்பு in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        விடுவி);

வெறுமை in6_addr_hash_func(மாறிலி கட்டமைப்பு in6_addr *addr, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(state);

        siphash24_compress_typesafe(*addr, state);
}

முழு in6_addr_compare_func(மாறிலி கட்டமைப்பு in6_addr *a, மாறிலி கட்டமைப்பு in6_addr *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு memcmp(a, b, அளவு(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        கட்டமைப்பு in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(
        in6_addr_hash_ops_free,
        கட்டமைப்பு in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        விடுவி);
