/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <arpa/inet.h>
#अन्तर्भाव <endian.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "hash-funcs.h"
#अन्तर्भाव "in-addr-util.h"
#अन्तर्भाव "logarithm.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "random-util.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"

bool in4_addr_is_null(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त a->s_addr == 0;
}

bool in6_addr_is_null(ध्रुव संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त eqzero(a->s6_addr32);
}

पूर्णाङ्क in_addr_is_null(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) {
        प्रतिज्ञा(u);

        सचे (family == AF_INET)
                पटिनिवत्त in4_addr_is_null(&u->in);

        सचे (family == AF_INET6)
                पटिनिवत्त in6_addr_is_null(&u->in6);

        पटिनिवत्त -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);

        सचे (!in4_addr_is_link_local(a))
                पटिनिवत्त false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        पटिनिवत्त !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ध्रुव संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

पूर्णाङ्क in_addr_is_link_local(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) {
        प्रतिज्ञा(u);

        सचे (family == AF_INET)
                पटिनिवत्त in4_addr_is_link_local(&u->in);

        सचे (family == AF_INET6)
                पटिनिवत्त in6_addr_is_link_local(&u->in6);

        पटिनिवत्त -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ध्रुव संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        /* ff02::1 */
        पटिनिवत्त be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ध्रुव संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त a->s6_addr[0] == 0xff;
}

पूर्णाङ्क in_addr_is_multicast(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) {
        प्रतिज्ञा(u);

        सचे (family == AF_INET)
                पटिनिवत्त in4_addr_is_multicast(&u->in);

        सचे (family == AF_INET6)
                पटिनिवत्त in6_addr_is_multicast(&u->in6);

        पटिनिवत्त -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);

        /* All of 127.x.x.x is localhost. */
        पटिनिवत्त (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ध्रुव संरचना in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        पटिनिवत्त !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

स्थिर bool in6_addr_is_loopback(ध्रुव संरचना in6_addr *a) {
        पटिनिवत्त memcmp(a, &(संरचना in6_addr) IN6ADDR_LOOPBACK_INIT, परिमाण(संरचना in6_addr)) == 0;
}

पूर्णाङ्क in_addr_is_localhost(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) {
        प्रतिज्ञा(u);

        सचे (family == AF_INET)
                पटिनिवत्त in4_addr_is_localhost(&u->in);

        सचे (family == AF_INET6)
                पटिनिवत्त in6_addr_is_loopback(&u->in6);

        पटिनिवत्त -EAFNOSUPPORT;
}

पूर्णाङ्क in_addr_is_localhost_one(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u) {
        प्रतिज्ञा(u);

        सचे (family == AF_INET)
                /* 127.0.0.1 */
                पटिनिवत्त be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        सचे (family == AF_INET6)
                पटिनिवत्त in6_addr_is_loopback(&u->in6);

        पटिनिवत्त -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(ध्रुव संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        पटिनिवत्त a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ध्रुव संरचना in_addr *a, ध्रुव संरचना in_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        पटिनिवत्त a->s_addr == b->s_addr;
}

bool in6_addr_equal(ध्रुव संरचना in6_addr *a, ध्रुव संरचना in6_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        पटिनिवत्त memcmp(a, b, परिमाण(संरचना in6_addr)) == 0;
}

पूर्णाङ्क in_addr_equal(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *a, ध्रुव सङ्घ in_addr_union *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        सचे (family == AF_INET)
                पटिनिवत्त in4_addr_equal(&a->in, &b->in);

        सचे (family == AF_INET6)
                पटिनिवत्त in6_addr_equal(&a->in6, &b->in6);

        पटिनिवत्त -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                ध्रुव संरचना in_addr *a,
                unsigned aprefixlen,
                ध्रुव संरचना in_addr *b,
                unsigned bprefixlen) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (परिमाण(संरचना in_addr) * 8));
        सचे (m == 0)
                पटिनिवत्त true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        पटिनिवत्त (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                ध्रुव संरचना in6_addr *a,
                unsigned aprefixlen,
                ध्रुव संरचना in6_addr *b,
                unsigned bprefixlen) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (परिमाण(संरचना in6_addr) * 8));
        सचे (m == 0)
                पटिनिवत्त true;

        अत्थाय (size_t i = 0; i < परिमाण(संरचना in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                सचे ((x & n) != 0)
                        पटिनिवत्त false;

                सचे (m <= 8)
                        भङ्ग;

                m -= 8;
        }

        पटिनिवत्त true;
}

पूर्णाङ्क in_addr_prefix_intersect(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *a,
                unsigned aprefixlen,
                ध्रुव सङ्घ in_addr_union *b,
                unsigned bprefixlen) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        /* Checks whether there are any addresses that are in both networks. */

        सचे (family == AF_INET)
                पटिनिवत्त in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        सचे (family == AF_INET6)
                पटिनिवत्त in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        पटिनिवत्त -EAFNOSUPPORT;
}

पूर्णाङ्क in_addr_random_prefix(
                पूर्णाङ्क family,
                सङ्घ in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        प्रतिज्ञा(u);

        /* Random network part of an address by one. */

        सचे (prefixlen <= 0)
                पटिनिवत्त 0;

        सचे (family == AF_INET) {
                uint32_t c, n;

                सचे (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                सचे (prefixlen > 32)
                        prefixlen = 32;
                सचे (prefixlen_fixed_part >= prefixlen)
                        पटिनिवत्त -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, परिमाण(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                पटिनिवत्त 1;
        }

        सचे (family == AF_INET6) {
                संरचना in6_addr n;
                unsigned i, j;

                सचे (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                सचे (prefixlen > 128)
                        prefixlen = 128;
                सचे (prefixlen_fixed_part >= prefixlen)
                        पटिनिवत्त -EINVAL;

                random_bytes(&n, परिमाण(n));

                अत्थाय (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        सचे (i < (prefixlen_fixed_part + 7) / 8) {
                                सचे (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                अञ्ञथा {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        सचे (i < (prefixlen + 7) / 8) {
                                सचे (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                अञ्ञथा {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                पटिनिवत्त 1;
        }

        पटिनिवत्त -EAFNOSUPPORT;
}

पूर्णाङ्क in_addr_prefix_range(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *in,
                unsigned prefixlen,
                सङ्घ in_addr_union *ret_start,
                सङ्घ in_addr_union *ret_end) {

        पूर्णाङ्क r;

        प्रतिज्ञा(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        विकल्प (family) {
        पक्ष AF_INET:
                सचे (prefixlen > 32)
                        पटिनिवत्त -EINVAL;
                भङ्ग;
        पक्ष AF_INET6:
                सचे (prefixlen > 128)
                        पटिनिवत्त -EINVAL;
                भङ्ग;
        स्वभाव:
                पटिनिवत्त -EAFNOSUPPORT;
        }

        सचे (prefixlen == 0) {
                /* shortcut for trivial case */
                सचे (ret_start)
                        *ret_start = IN_ADDR_NULL;
                सचे (ret_end)
                        *ret_end = IN_ADDR_NULL;
                पटिनिवत्त 0;
        }

        सङ्घ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        सचे (r < 0)
                पटिनिवत्त r;

        सङ्घ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        अत्थाय (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                सचे (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } अञ्ञथा {
                        end.bytes[i] += add;
                        भङ्ग;
                }

        सचे (ret_start)
                *ret_start = start;
        सचे (ret_end)
                *ret_end = end;

        पटिनिवत्त 0;
}

पूर्णाङ्क in_addr_to_string(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u, वर्ण **ret) {
        _cleanup_free_ वर्ण *x = रिक्त;
        size_t l;

        प्रतिज्ञा(u);
        प्रतिज्ञा(ret);

        सचे (family == AF_INET)
                l = INET_ADDRSTRLEN;
        अञ्ञथा सचे (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        अञ्ञथा
                पटिनिवत्त -EAFNOSUPPORT;

        x = new(वर्ण, l);
        सचे (!x)
                पटिनिवत्त -ENOMEM;

        errno = 0;
        सचे (!typesafe_inet_ntop(family, u, x, l))
                पटिनिवत्त errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        पटिनिवत्त 0;
}

ध्रुव वर्ण* typesafe_inet_ntop(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *a, वर्ण *buf, size_t len) {
        पटिनिवत्त inet_ntop(family, a, buf, len);
}

ध्रुव वर्ण* typesafe_inet_ntop4(ध्रुव संरचना in_addr *a, वर्ण *buf, size_t len) {
        पटिनिवत्त inet_ntop(AF_INET, a, buf, len);
}

ध्रुव वर्ण* typesafe_inet_ntop6(ध्रुव संरचना in6_addr *a, वर्ण *buf, size_t len) {
        पटिनिवत्त inet_ntop(AF_INET6, a, buf, len);
}

पूर्णाङ्क in_addr_prefix_to_string(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *u,
                unsigned prefixlen,
                वर्ण *buf,
                size_t buf_len) {

        प्रतिज्ञा(u);
        प्रतिज्ञा(buf);

        सचे (!IN_SET(family, AF_INET, AF_INET6))
                पटिनिवत्त -EAFNOSUPPORT;

        errno = 0;
        सचे (!typesafe_inet_ntop(family, u, buf, buf_len))
                पटिनिवत्त errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        सचे (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                पटिनिवत्त -ENOSPC;
        पटिनिवत्त 0;
}

पूर्णाङ्क in_addr_port_ifindex_name_to_string(पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *u, uint16_t port, पूर्णाङ्क ifindex, ध्रुव वर्ण *server_name, वर्ण **ret) {
        _cleanup_free_ वर्ण *ip_str = रिक्त, *x = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(IN_SET(family, AF_INET, AF_INET6));
        प्रतिज्ञा(u);
        प्रतिज्ञा(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        ifindex = 0;
        } अञ्ञथा
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        सचे (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                पटिनिवत्त 0;
        }

        ध्रुव वर्ण *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        सचे (port > 0) {
                सचे (family == AF_INET6) {
                        सचे (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        अञ्ञथा
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } अञ्ञथा
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } अञ्ञथा {
                सचे (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                अञ्ञथा
                        x = strjoin(ip_str, separator, server_name);
        }
        सचे (!x)
                पटिनिवत्त -ENOMEM;

        *ret = TAKE_PTR(x);
        पटिनिवत्त 0;
}

पूर्णाङ्क in_addr_from_string(पूर्णाङ्क family, ध्रुव वर्ण *s, सङ्घ in_addr_union *ret) {
        सङ्घ in_addr_union buffer;
        प्रतिज्ञा(s);

        सचे (!IN_SET(family, AF_INET, AF_INET6))
                पटिनिवत्त -EAFNOSUPPORT;

        errno = 0;
        सचे (inet_pton(family, s, ret ?: &buffer) <= 0)
                पटिनिवत्त errno_or_else(EINVAL);

        पटिनिवत्त 0;
}

पूर्णाङ्क in_addr_from_string_auto(ध्रुव वर्ण *s, पूर्णाङ्क *ret_family, सङ्घ in_addr_union *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = in_addr_from_string(AF_INET, s, ret);
        सचे (r >= 0) {
                सचे (ret_family)
                        *ret_family = AF_INET;
                पटिनिवत्त 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        सचे (r >= 0) {
                सचे (ret_family)
                        *ret_family = AF_INET6;
                पटिनिवत्त 0;
        }

        पटिनिवत्त -EINVAL;
}

unsigned वर्ण in4_addr_netmask_to_prefixlen(ध्रुव संरचना in_addr *addr) {
        प्रतिज्ञा(addr);

        पटिनिवत्त 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
संरचना in_addr* in4_addr_prefixlen_to_netmask(संरचना in_addr *addr, unsigned वर्ण prefixlen) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        सचे (prefixlen == 0)
                addr->s_addr = 0;
        अञ्ञथा
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        पटिनिवत्त addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
संरचना in6_addr* in6_addr_prefixlen_to_netmask(संरचना in6_addr *addr, unsigned वर्ण prefixlen) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(prefixlen <= 128);

        अत्थाय (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                सचे (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } अञ्ञथा सचे (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } अञ्ञथा {
                        प्रतिज्ञा(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        पटिनिवत्त addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
पूर्णाङ्क in_addr_prefixlen_to_netmask(पूर्णाङ्क family, सङ्घ in_addr_union *addr, unsigned वर्ण prefixlen) {
        प्रतिज्ञा(addr);

        विकल्प (family) {
        पक्ष AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                पटिनिवत्त 0;
        पक्ष AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                पटिनिवत्त 0;
        स्वभाव:
                पटिनिवत्त -EAFNOSUPPORT;
        }
}

पूर्णाङ्क in4_addr_default_prefixlen(ध्रुव संरचना in_addr *addr, unsigned वर्ण *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        प्रतिज्ञा(addr);
        प्रतिज्ञा(prefixlen);

        सचे (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        अञ्ञथा सचे (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        अञ्ञथा सचे (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        अञ्ञथा
                /* class D or E, no default prefixlen */
                पटिनिवत्त -ERANGE;

        पटिनिवत्त 0;
}

पूर्णाङ्क in4_addr_default_subnet_mask(ध्रुव संरचना in_addr *addr, संरचना in_addr *mask) {
        unsigned वर्ण prefixlen;
        पूर्णाङ्क r;

        प्रतिज्ञा(addr);
        प्रतिज्ञा(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        सचे (r < 0)
                पटिनिवत्त r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        पटिनिवत्त 0;
}

पूर्णाङ्क in4_addr_mask(संरचना in_addr *addr, unsigned वर्ण prefixlen) {
        संरचना in_addr mask;

        प्रतिज्ञा(addr);

        सचे (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                पटिनिवत्त -EINVAL;

        addr->s_addr &= mask.s_addr;
        पटिनिवत्त 0;
}

पूर्णाङ्क in6_addr_mask(संरचना in6_addr *addr, unsigned वर्ण prefixlen) {
        unsigned i;

        प्रतिज्ञा(addr);

        अत्थाय (i = 0; i < 16; i++) {
                uint8_t mask;

                सचे (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } अञ्ञथा सचे (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } अञ्ञथा {
                        प्रतिज्ञा(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क in_addr_mask(पूर्णाङ्क family, सङ्घ in_addr_union *addr, unsigned वर्ण prefixlen) {
        प्रतिज्ञा(addr);

        विकल्प (family) {
        पक्ष AF_INET:
                पटिनिवत्त in4_addr_mask(&addr->in, prefixlen);
        पक्ष AF_INET6:
                पटिनिवत्त in6_addr_mask(&addr->in6, prefixlen);
        स्वभाव:
                पटिनिवत्त -EAFNOSUPPORT;
        }
}

पूर्णाङ्क in4_addr_prefix_covers_full(
                ध्रुव संरचना in_addr *prefix,
                unsigned वर्ण prefixlen,
                ध्रुव संरचना in_addr *address,
                unsigned वर्ण address_prefixlen) {

        संरचना in_addr masked_prefix, masked_address;
        पूर्णाङ्क r;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(address);

        सचे (prefixlen > address_prefixlen)
                पटिनिवत्त false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        सचे (r < 0)
                पटिनिवत्त r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त in4_addr_equal(&masked_prefix, &masked_address);
}

पूर्णाङ्क in6_addr_prefix_covers_full(
                ध्रुव संरचना in6_addr *prefix,
                unsigned वर्ण prefixlen,
                ध्रुव संरचना in6_addr *address,
                unsigned वर्ण address_prefixlen) {

        संरचना in6_addr masked_prefix, masked_address;
        पूर्णाङ्क r;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(address);

        सचे (prefixlen > address_prefixlen)
                पटिनिवत्त false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        सचे (r < 0)
                पटिनिवत्त r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त in6_addr_equal(&masked_prefix, &masked_address);
}

पूर्णाङ्क in_addr_prefix_covers_full(
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *prefix,
                unsigned वर्ण prefixlen,
                ध्रुव सङ्घ in_addr_union *address,
                unsigned वर्ण address_prefixlen) {

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(address);

        विकल्प (family) {
        पक्ष AF_INET:
                पटिनिवत्त in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        पक्ष AF_INET6:
                पटिनिवत्त in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        स्वभाव:
                पटिनिवत्त -EAFNOSUPPORT;
        }
}

पूर्णाङ्क in_addr_parse_prefixlen(पूर्णाङ्क family, ध्रुव वर्ण *p, unsigned वर्ण *ret) {
        uint8_t u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        सचे (!IN_SET(family, AF_INET, AF_INET6))
                पटिनिवत्त -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (u > FAMILY_ADDRESS_SIZE(family) * 8)
                पटिनिवत्त -ERANGE;

        *ret = u;
        पटिनिवत्त 0;
}

पूर्णाङ्क in_addr_prefix_from_string(
                ध्रुव वर्ण *p,
                पूर्णाङ्क family,
                सङ्घ in_addr_union *ret_prefix,
                unsigned वर्ण *ret_prefixlen) {

        _cleanup_free_ वर्ण *str = रिक्त;
        सङ्घ in_addr_union buffer;
        ध्रुव वर्ण *e, *l;
        unsigned वर्ण k;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);

        सचे (!IN_SET(family, AF_INET, AF_INET6))
                पटिनिवत्त -EAFNOSUPPORT;

        e = strchr(p, '/');
        सचे (e) {
                str = strndup(p, e - p);
                सचे (!str)
                        पटिनिवत्त -ENOMEM;

                l = str;
        } अञ्ञथा
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                सचे (r < 0)
                        पटिनिवत्त r;
        } अञ्ञथा
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        सचे (ret_prefix)
                *ret_prefix = buffer;
        सचे (ret_prefixlen)
                *ret_prefixlen = k;

        पटिनिवत्त 0;
}

पूर्णाङ्क in_addr_prefix_from_string_auto_full(
                ध्रुव वर्ण *p,
                InAddrPrefixLenMode mode,
                पूर्णाङ्क *ret_family,
                सङ्घ in_addr_union *ret_prefix,
                unsigned वर्ण *ret_prefixlen) {

        _cleanup_free_ वर्ण *str = रिक्त;
        सङ्घ in_addr_union buffer;
        ध्रुव वर्ण *e, *l;
        unsigned वर्ण k;
        पूर्णाङ्क family, r;

        प्रतिज्ञा(p);

        e = strchr(p, '/');
        सचे (e) {
                str = strndup(p, e - p);
                सचे (!str)
                        पटिनिवत्त -ENOMEM;

                l = str;
        } अञ्ञथा
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                सचे (r < 0)
                        पटिनिवत्त r;
        } अञ्ञथा
                विकल्प (mode) {
                पक्ष PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        भङ्ग;
                पक्ष PREFIXLEN_REFUSE:
                        पटिनिवत्त -ENOANO; /* To distinguish this error from others. */
                स्वभाव:
                        assert_not_reached();
                }

        सचे (ret_family)
                *ret_family = family;
        सचे (ret_prefix)
                *ret_prefix = buffer;
        सचे (ret_prefixlen)
                *ret_prefixlen = k;

        पटिनिवत्त 0;
}

शून्य in_addr_hash_func(ध्रुव सङ्घ in_addr_union *u, पूर्णाङ्क family, संरचना siphash *state) {
        प्रतिज्ञा(u);
        प्रतिज्ञा(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

शून्य in_addr_data_hash_func(ध्रुव संरचना in_addr_data *a, संरचना siphash *state) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

पूर्णाङ्क in_addr_data_compare_func(ध्रुव संरचना in_addr_data *x, ध्रुव संरचना in_addr_data *y) {
        पूर्णाङ्क r;

        प्रतिज्ञा(x);
        प्रतिज्ञा(y);

        r = CMP(x->family, y->family);
        सचे (r != 0)
                पटिनिवत्त r;

        पटिनिवत्त memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        संरचना in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_कुञ्चिका_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        संरचना in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        मुक्त);

शून्य in6_addr_hash_func(ध्रुव संरचना in6_addr *addr, संरचना siphash *state) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(state);

        siphash24_compress_typesafe(*addr, state);
}

पूर्णाङ्क in6_addr_compare_func(ध्रुव संरचना in6_addr *a, ध्रुव संरचना in6_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        पटिनिवत्त memcmp(a, b, परिमाण(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        संरचना in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_कुञ्चिका_DESTRUCTOR(
        in6_addr_hash_ops_free,
        संरचना in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        मुक्त);
