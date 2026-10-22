/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <arpa/inet.h>
#അന്തര്ഭാവ <endian.h>
#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "hash-funcs.h"
#അന്തര്ഭാവ "in-addr-util.h"
#അന്തര്ഭാവ "logarithm.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "random-util.h"
#അന്തര്ഭാവ "siphash24.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"

bool in4_addr_is_null(സ്ഥിരം ഘടന in_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ a->s_addr == 0;
}

bool in6_addr_is_null(സ്ഥിരം ഘടന in6_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ eqzero(a->s6_addr32);
}

പൂർണ്ണസംഖ്യ in_addr_is_null(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) {
        പ്രതിജ്ഞാ(u);

        എങ്കിൽ (family == AF_INET)
                തിരികെ in4_addr_is_null(&u->in);

        എങ്കിൽ (family == AF_INET6)
                തിരികെ in6_addr_is_null(&u->in6);

        തിരികെ -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(സ്ഥിരം ഘടന in_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(സ്ഥിരം ഘടന in_addr *a) {
        പ്രതിജ്ഞാ(a);

        എങ്കിൽ (!in4_addr_is_link_local(a))
                തിരികെ false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        തിരികെ !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(സ്ഥിരം ഘടന in6_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

പൂർണ്ണസംഖ്യ in_addr_is_link_local(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) {
        പ്രതിജ്ഞാ(u);

        എങ്കിൽ (family == AF_INET)
                തിരികെ in4_addr_is_link_local(&u->in);

        എങ്കിൽ (family == AF_INET6)
                തിരികെ in6_addr_is_link_local(&u->in6);

        തിരികെ -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(സ്ഥിരം ഘടന in6_addr *a) {
        പ്രതിജ്ഞാ(a);

        /* ff02::1 */
        തിരികെ be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(സ്ഥിരം ഘടന in_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(സ്ഥിരം ഘടന in6_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ a->s6_addr[0] == 0xff;
}

പൂർണ്ണസംഖ്യ in_addr_is_multicast(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) {
        പ്രതിജ്ഞാ(u);

        എങ്കിൽ (family == AF_INET)
                തിരികെ in4_addr_is_multicast(&u->in);

        എങ്കിൽ (family == AF_INET6)
                തിരികെ in6_addr_is_multicast(&u->in6);

        തിരികെ -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(സ്ഥിരം ഘടന in_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(സ്ഥിരം ഘടന in_addr *a) {
        പ്രതിജ്ഞാ(a);

        /* All of 127.x.x.x is localhost. */
        തിരികെ (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(സ്ഥിരം ഘടന in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        തിരികെ !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

സ്ഥിതം bool in6_addr_is_loopback(സ്ഥിരം ഘടന in6_addr *a) {
        തിരികെ memcmp(a, &(ഘടന in6_addr) IN6ADDR_LOOPBACK_INIT, വലുപ്പം(ഘടന in6_addr)) == 0;
}

പൂർണ്ണസംഖ്യ in_addr_is_localhost(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) {
        പ്രതിജ്ഞാ(u);

        എങ്കിൽ (family == AF_INET)
                തിരികെ in4_addr_is_localhost(&u->in);

        എങ്കിൽ (family == AF_INET6)
                തിരികെ in6_addr_is_loopback(&u->in6);

        തിരികെ -EAFNOSUPPORT;
}

പൂർണ്ണസംഖ്യ in_addr_is_localhost_one(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u) {
        പ്രതിജ്ഞാ(u);

        എങ്കിൽ (family == AF_INET)
                /* 127.0.0.1 */
                തിരികെ be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        എങ്കിൽ (family == AF_INET6)
                തിരികെ in6_addr_is_loopback(&u->in6);

        തിരികെ -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(സ്ഥിരം ഘടന in6_addr *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(സ്ഥിരം ഘടന in_addr *a, സ്ഥിരം ഘടന in_addr *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ a->s_addr == b->s_addr;
}

bool in6_addr_equal(സ്ഥിരം ഘടന in6_addr *a, സ്ഥിരം ഘടന in6_addr *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ memcmp(a, b, വലുപ്പം(ഘടന in6_addr)) == 0;
}

പൂർണ്ണസംഖ്യ in_addr_equal(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *a, സ്ഥിരം സംഘം in_addr_union *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        എങ്കിൽ (family == AF_INET)
                തിരികെ in4_addr_equal(&a->in, &b->in);

        എങ്കിൽ (family == AF_INET6)
                തിരികെ in6_addr_equal(&a->in6, &b->in6);

        തിരികെ -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                സ്ഥിരം ഘടന in_addr *a,
                unsigned aprefixlen,
                സ്ഥിരം ഘടന in_addr *b,
                unsigned bprefixlen) {

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (വലുപ്പം(ഘടന in_addr) * 8));
        എങ്കിൽ (m == 0)
                തിരികെ true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        തിരികെ (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                സ്ഥിരം ഘടന in6_addr *a,
                unsigned aprefixlen,
                സ്ഥിരം ഘടന in6_addr *b,
                unsigned bprefixlen) {

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (വലുപ്പം(ഘടന in6_addr) * 8));
        എങ്കിൽ (m == 0)
                തിരികെ true;

        വേണ്ടി (size_t i = 0; i < വലുപ്പം(ഘടന in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                എങ്കിൽ ((x & n) != 0)
                        തിരികെ false;

                എങ്കിൽ (m <= 8)
                        നിർത്തുക;

                m -= 8;
        }

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ in_addr_prefix_intersect(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *a,
                unsigned aprefixlen,
                സ്ഥിരം സംഘം in_addr_union *b,
                unsigned bprefixlen) {

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        /* Checks whether there are any addresses that are in both networks. */

        എങ്കിൽ (family == AF_INET)
                തിരികെ in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        എങ്കിൽ (family == AF_INET6)
                തിരികെ in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        തിരികെ -EAFNOSUPPORT;
}

പൂർണ്ണസംഖ്യ in_addr_random_prefix(
                പൂർണ്ണസംഖ്യ family,
                സംഘം in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        പ്രതിജ്ഞാ(u);

        /* Random network part of an address by one. */

        എങ്കിൽ (prefixlen <= 0)
                തിരികെ 0;

        എങ്കിൽ (family == AF_INET) {
                uint32_t c, n;

                എങ്കിൽ (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                എങ്കിൽ (prefixlen > 32)
                        prefixlen = 32;
                എങ്കിൽ (prefixlen_fixed_part >= prefixlen)
                        തിരികെ -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, വലുപ്പം(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                തിരികെ 1;
        }

        എങ്കിൽ (family == AF_INET6) {
                ഘടന in6_addr n;
                unsigned i, j;

                എങ്കിൽ (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                എങ്കിൽ (prefixlen > 128)
                        prefixlen = 128;
                എങ്കിൽ (prefixlen_fixed_part >= prefixlen)
                        തിരികെ -EINVAL;

                random_bytes(&n, വലുപ്പം(n));

                വേണ്ടി (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        എങ്കിൽ (i < (prefixlen_fixed_part + 7) / 8) {
                                എങ്കിൽ (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                അല്ലെങ്കിൽ {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        എങ്കിൽ (i < (prefixlen + 7) / 8) {
                                എങ്കിൽ (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                അല്ലെങ്കിൽ {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                തിരികെ 1;
        }

        തിരികെ -EAFNOSUPPORT;
}

പൂർണ്ണസംഖ്യ in_addr_prefix_range(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *in,
                unsigned prefixlen,
                സംഘം in_addr_union *ret_start,
                സംഘം in_addr_union *ret_end) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        തിരഞ്ഞെടുപ്പ് (family) {
        അവസ്ഥ AF_INET:
                എങ്കിൽ (prefixlen > 32)
                        തിരികെ -EINVAL;
                നിർത്തുക;
        അവസ്ഥ AF_INET6:
                എങ്കിൽ (prefixlen > 128)
                        തിരികെ -EINVAL;
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }

        എങ്കിൽ (prefixlen == 0) {
                /* shortcut for trivial case */
                എങ്കിൽ (ret_start)
                        *ret_start = IN_ADDR_NULL;
                എങ്കിൽ (ret_end)
                        *ret_end = IN_ADDR_NULL;
                തിരികെ 0;
        }

        സംഘം in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        സംഘം in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        വേണ്ടി (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                എങ്കിൽ (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } അല്ലെങ്കിൽ {
                        end.bytes[i] += add;
                        നിർത്തുക;
                }

        എങ്കിൽ (ret_start)
                *ret_start = start;
        എങ്കിൽ (ret_end)
                *ret_end = end;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in_addr_to_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *x = ശൂന്യ;
        size_t l;

        പ്രതിജ്ഞാ(u);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (family == AF_INET)
                l = INET_ADDRSTRLEN;
        അല്ലെങ്കിൽ എങ്കിൽ (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        അല്ലെങ്കിൽ
                തിരികെ -EAFNOSUPPORT;

        x = new(അക്ഷരം, l);
        എങ്കിൽ (!x)
                തിരികെ -ENOMEM;

        errno = 0;
        എങ്കിൽ (!typesafe_inet_ntop(family, u, x, l))
                തിരികെ errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        തിരികെ 0;
}

സ്ഥിരം അക്ഷരം* typesafe_inet_ntop(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *a, അക്ഷരം *buf, size_t len) {
        തിരികെ inet_ntop(family, a, buf, len);
}

സ്ഥിരം അക്ഷരം* typesafe_inet_ntop4(സ്ഥിരം ഘടന in_addr *a, അക്ഷരം *buf, size_t len) {
        തിരികെ inet_ntop(AF_INET, a, buf, len);
}

സ്ഥിരം അക്ഷരം* typesafe_inet_ntop6(സ്ഥിരം ഘടന in6_addr *a, അക്ഷരം *buf, size_t len) {
        തിരികെ inet_ntop(AF_INET6, a, buf, len);
}

പൂർണ്ണസംഖ്യ in_addr_prefix_to_string(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *u,
                unsigned prefixlen,
                അക്ഷരം *buf,
                size_t buf_len) {

        പ്രതിജ്ഞാ(u);
        പ്രതിജ്ഞാ(buf);

        എങ്കിൽ (!IN_SET(family, AF_INET, AF_INET6))
                തിരികെ -EAFNOSUPPORT;

        errno = 0;
        എങ്കിൽ (!typesafe_inet_ntop(family, u, buf, buf_len))
                തിരികെ errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        എങ്കിൽ (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                തിരികെ -ENOSPC;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in_addr_port_ifindex_name_to_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *u, uint16_t port, പൂർണ്ണസംഖ്യ ifindex, സ്ഥിരം അക്ഷരം *server_name, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *ip_str = ശൂന്യ, *x = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(IN_SET(family, AF_INET, AF_INET6));
        പ്രതിജ്ഞാ(u);
        പ്രതിജ്ഞാ(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        ifindex = 0;
        } അല്ലെങ്കിൽ
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        എങ്കിൽ (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                തിരികെ 0;
        }

        സ്ഥിരം അക്ഷരം *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        എങ്കിൽ (port > 0) {
                എങ്കിൽ (family == AF_INET6) {
                        എങ്കിൽ (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        അല്ലെങ്കിൽ
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } അല്ലെങ്കിൽ
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                അല്ലെങ്കിൽ
                        x = strjoin(ip_str, separator, server_name);
        }
        എങ്കിൽ (!x)
                തിരികെ -ENOMEM;

        *ret = TAKE_PTR(x);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in_addr_from_string(പൂർണ്ണസംഖ്യ family, സ്ഥിരം അക്ഷരം *s, സംഘം in_addr_union *ret) {
        സംഘം in_addr_union buffer;
        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (!IN_SET(family, AF_INET, AF_INET6))
                തിരികെ -EAFNOSUPPORT;

        errno = 0;
        എങ്കിൽ (inet_pton(family, s, ret ?: &buffer) <= 0)
                തിരികെ errno_or_else(EINVAL);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in_addr_from_string_auto(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret_family, സംഘം in_addr_union *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        r = in_addr_from_string(AF_INET, s, ret);
        എങ്കിൽ (r >= 0) {
                എങ്കിൽ (ret_family)
                        *ret_family = AF_INET;
                തിരികെ 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        എങ്കിൽ (r >= 0) {
                എങ്കിൽ (ret_family)
                        *ret_family = AF_INET6;
                തിരികെ 0;
        }

        തിരികെ -EINVAL;
}

unsigned അക്ഷരം in4_addr_netmask_to_prefixlen(സ്ഥിരം ഘടന in_addr *addr) {
        പ്രതിജ്ഞാ(addr);

        തിരികെ 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
ഘടന in_addr* in4_addr_prefixlen_to_netmask(ഘടന in_addr *addr, unsigned അക്ഷരം prefixlen) {
        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        എങ്കിൽ (prefixlen == 0)
                addr->s_addr = 0;
        അല്ലെങ്കിൽ
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        തിരികെ addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
ഘടന in6_addr* in6_addr_prefixlen_to_netmask(ഘടന in6_addr *addr, unsigned അക്ഷരം prefixlen) {
        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(prefixlen <= 128);

        വേണ്ടി (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                എങ്കിൽ (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } അല്ലെങ്കിൽ എങ്കിൽ (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } അല്ലെങ്കിൽ {
                        പ്രതിജ്ഞാ(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        തിരികെ addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
പൂർണ്ണസംഖ്യ in_addr_prefixlen_to_netmask(പൂർണ്ണസംഖ്യ family, സംഘം in_addr_union *addr, unsigned അക്ഷരം prefixlen) {
        പ്രതിജ്ഞാ(addr);

        തിരഞ്ഞെടുപ്പ് (family) {
        അവസ്ഥ AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                തിരികെ 0;
        അവസ്ഥ AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                തിരികെ 0;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ in4_addr_default_prefixlen(സ്ഥിരം ഘടന in_addr *addr, unsigned അക്ഷരം *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(prefixlen);

        എങ്കിൽ (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        അല്ലെങ്കിൽ എങ്കിൽ (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        അല്ലെങ്കിൽ എങ്കിൽ (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        അല്ലെങ്കിൽ
                /* class D or E, no default prefixlen */
                തിരികെ -ERANGE;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in4_addr_default_subnet_mask(സ്ഥിരം ഘടന in_addr *addr, ഘടന in_addr *mask) {
        unsigned അക്ഷരം prefixlen;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in4_addr_mask(ഘടന in_addr *addr, unsigned അക്ഷരം prefixlen) {
        ഘടന in_addr mask;

        പ്രതിജ്ഞാ(addr);

        എങ്കിൽ (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                തിരികെ -EINVAL;

        addr->s_addr &= mask.s_addr;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in6_addr_mask(ഘടന in6_addr *addr, unsigned അക്ഷരം prefixlen) {
        unsigned i;

        പ്രതിജ്ഞാ(addr);

        വേണ്ടി (i = 0; i < 16; i++) {
                uint8_t mask;

                എങ്കിൽ (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } അല്ലെങ്കിൽ എങ്കിൽ (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } അല്ലെങ്കിൽ {
                        പ്രതിജ്ഞാ(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in_addr_mask(പൂർണ്ണസംഖ്യ family, സംഘം in_addr_union *addr, unsigned അക്ഷരം prefixlen) {
        പ്രതിജ്ഞാ(addr);

        തിരഞ്ഞെടുപ്പ് (family) {
        അവസ്ഥ AF_INET:
                തിരികെ in4_addr_mask(&addr->in, prefixlen);
        അവസ്ഥ AF_INET6:
                തിരികെ in6_addr_mask(&addr->in6, prefixlen);
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ in4_addr_prefix_covers_full(
                സ്ഥിരം ഘടന in_addr *prefix,
                unsigned അക്ഷരം prefixlen,
                സ്ഥിരം ഘടന in_addr *address,
                unsigned അക്ഷരം address_prefixlen) {

        ഘടന in_addr masked_prefix, masked_address;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(prefix);
        പ്രതിജ്ഞാ(address);

        എങ്കിൽ (prefixlen > address_prefixlen)
                തിരികെ false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ in4_addr_equal(&masked_prefix, &masked_address);
}

പൂർണ്ണസംഖ്യ in6_addr_prefix_covers_full(
                സ്ഥിരം ഘടന in6_addr *prefix,
                unsigned അക്ഷരം prefixlen,
                സ്ഥിരം ഘടന in6_addr *address,
                unsigned അക്ഷരം address_prefixlen) {

        ഘടന in6_addr masked_prefix, masked_address;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(prefix);
        പ്രതിജ്ഞാ(address);

        എങ്കിൽ (prefixlen > address_prefixlen)
                തിരികെ false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ in6_addr_equal(&masked_prefix, &masked_address);
}

പൂർണ്ണസംഖ്യ in_addr_prefix_covers_full(
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *prefix,
                unsigned അക്ഷരം prefixlen,
                സ്ഥിരം സംഘം in_addr_union *address,
                unsigned അക്ഷരം address_prefixlen) {

        പ്രതിജ്ഞാ(prefix);
        പ്രതിജ്ഞാ(address);

        തിരഞ്ഞെടുപ്പ് (family) {
        അവസ്ഥ AF_INET:
                തിരികെ in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        അവസ്ഥ AF_INET6:
                തിരികെ in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ in_addr_parse_prefixlen(പൂർണ്ണസംഖ്യ family, സ്ഥിരം അക്ഷരം *p, unsigned അക്ഷരം *ret) {
        uint8_t u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!IN_SET(family, AF_INET, AF_INET6))
                തിരികെ -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (u > FAMILY_ADDRESS_SIZE(family) * 8)
                തിരികെ -ERANGE;

        *ret = u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in_addr_prefix_from_string(
                സ്ഥിരം അക്ഷരം *p,
                പൂർണ്ണസംഖ്യ family,
                സംഘം in_addr_union *ret_prefix,
                unsigned അക്ഷരം *ret_prefixlen) {

        _cleanup_free_ അക്ഷരം *str = ശൂന്യ;
        സംഘം in_addr_union buffer;
        സ്ഥിരം അക്ഷരം *e, *l;
        unsigned അക്ഷരം k;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);

        എങ്കിൽ (!IN_SET(family, AF_INET, AF_INET6))
                തിരികെ -EAFNOSUPPORT;

        e = strchr(p, '/');
        എങ്കിൽ (e) {
                str = strndup(p, e - p);
                എങ്കിൽ (!str)
                        തിരികെ -ENOMEM;

                l = str;
        } അല്ലെങ്കിൽ
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        } അല്ലെങ്കിൽ
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        എങ്കിൽ (ret_prefix)
                *ret_prefix = buffer;
        എങ്കിൽ (ret_prefixlen)
                *ret_prefixlen = k;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ in_addr_prefix_from_string_auto_full(
                സ്ഥിരം അക്ഷരം *p,
                InAddrPrefixLenMode mode,
                പൂർണ്ണസംഖ്യ *ret_family,
                സംഘം in_addr_union *ret_prefix,
                unsigned അക്ഷരം *ret_prefixlen) {

        _cleanup_free_ അക്ഷരം *str = ശൂന്യ;
        സംഘം in_addr_union buffer;
        സ്ഥിരം അക്ഷരം *e, *l;
        unsigned അക്ഷരം k;
        പൂർണ്ണസംഖ്യ family, r;

        പ്രതിജ്ഞാ(p);

        e = strchr(p, '/');
        എങ്കിൽ (e) {
                str = strndup(p, e - p);
                എങ്കിൽ (!str)
                        തിരികെ -ENOMEM;

                l = str;
        } അല്ലെങ്കിൽ
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        } അല്ലെങ്കിൽ
                തിരഞ്ഞെടുപ്പ് (mode) {
                അവസ്ഥ PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        നിർത്തുക;
                അവസ്ഥ PREFIXLEN_REFUSE:
                        തിരികെ -ENOANO; /* To distinguish this error from others. */
                സ്ഥിരസ്ഥിതി:
                        assert_not_reached();
                }

        എങ്കിൽ (ret_family)
                *ret_family = family;
        എങ്കിൽ (ret_prefix)
                *ret_prefix = buffer;
        എങ്കിൽ (ret_prefixlen)
                *ret_prefixlen = k;

        തിരികെ 0;
}

ശൂന്യം in_addr_hash_func(സ്ഥിരം സംഘം in_addr_union *u, പൂർണ്ണസംഖ്യ family, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(u);
        പ്രതിജ്ഞാ(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

ശൂന്യം in_addr_data_hash_func(സ്ഥിരം ഘടന in_addr_data *a, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

പൂർണ്ണസംഖ്യ in_addr_data_compare_func(സ്ഥിരം ഘടന in_addr_data *x, സ്ഥിരം ഘടന in_addr_data *y) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(x);
        പ്രതിജ്ഞാ(y);

        r = CMP(x->family, y->family);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        തിരികെ memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        ഘടന in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        ഘടന in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        സ്വതന്ത്രം);

ശൂന്യം in6_addr_hash_func(സ്ഥിരം ഘടന in6_addr *addr, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(state);

        siphash24_compress_typesafe(*addr, state);
}

പൂർണ്ണസംഖ്യ in6_addr_compare_func(സ്ഥിരം ഘടന in6_addr *a, സ്ഥിരം ഘടന in6_addr *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ memcmp(a, b, വലുപ്പം(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        ഘടന in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(
        in6_addr_hash_ops_free,
        ഘടന in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        സ്വതന്ത്രം);
