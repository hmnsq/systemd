/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <arpa/inet.h>
#شامل <endian.h>
#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "hash-funcs.h"
#شامل "in-addr-util.h"
#شامل "logarithm.h"
#شامل "memory-util.h"
#شامل "parse-util.h"
#شامل "random-util.h"
#شامل "siphash24.h"
#شامل "stdio-util.h"
#شامل "string-util.h"

bool in4_addr_is_null(ثابت ساخت in_addr *a) {
        أكد(a);

        واپس a->s_addr == 0;
}

bool in6_addr_is_null(ثابت ساخت in6_addr *a) {
        أكد(a);

        واپس eqzero(a->s6_addr32);
}

صحیح اندر_addr_is_null(صحیح family, ثابت اتحاد اندر_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                واپس in4_addr_is_null(&u->اندر);

        اگر (family == AF_INET6)
                واپس in6_addr_is_null(&u->in6);

        واپس -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ثابت ساخت in_addr *a) {
        أكد(a);

        واپس (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ثابت ساخت in_addr *a) {
        أكد(a);

        اگر (!in4_addr_is_link_local(a))
                واپس false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        واپس !اندر_سیٹ(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ثابت ساخت in6_addr *a) {
        أكد(a);

        واپس (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

صحیح اندر_addr_is_link_local(صحیح family, ثابت اتحاد اندر_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                واپس in4_addr_is_link_local(&u->اندر);

        اگر (family == AF_INET6)
                واپس in6_addr_is_link_local(&u->in6);

        واپس -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ثابت ساخت in6_addr *a) {
        أكد(a);

        /* ff02::1 */
        واپس be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ثابت ساخت in_addr *a) {
        أكد(a);

        واپس IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ثابت ساخت in6_addr *a) {
        أكد(a);

        واپس a->s6_addr[0] == 0xff;
}

صحیح اندر_addr_is_multicast(صحیح family, ثابت اتحاد اندر_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                واپس in4_addr_is_multicast(&u->اندر);

        اگر (family == AF_INET6)
                واپس in6_addr_is_multicast(&u->in6);

        واپس -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ثابت ساخت in_addr *a) {
        أكد(a);

        واپس (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ثابت ساخت in_addr *a) {
        أكد(a);

        /* All of 127.x.x.x is localhost. */
        واپس (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ثابت ساخت in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        واپس !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ساکن bool in6_addr_is_loopback(ثابت ساخت in6_addr *a) {
        واپس memcmp(a, &(ساخت in6_addr) IN6ADDR_LOOPBACK_INIT, حجم(ساخت in6_addr)) == 0;
}

صحیح اندر_addr_is_localhost(صحیح family, ثابت اتحاد اندر_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                واپس in4_addr_is_localhost(&u->اندر);

        اگر (family == AF_INET6)
                واپس in6_addr_is_loopback(&u->in6);

        واپس -EAFNOSUPPORT;
}

صحیح اندر_addr_is_localhost_one(صحیح family, ثابت اتحاد اندر_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                /* 127.0.0.1 */
                واپس be32toh(u->اندر.s_addr) == UINT32_C(0x7F000001);

        اگر (family == AF_INET6)
                واپس in6_addr_is_loopback(&u->in6);

        واپس -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_پتہ(ثابت ساخت in6_addr *a) {
        أكد(a);

        واپس a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ثابت ساخت in_addr *a, ثابت ساخت in_addr *b) {
        أكد(a);
        أكد(b);

        واپس a->s_addr == b->s_addr;
}

bool in6_addr_equal(ثابت ساخت in6_addr *a, ثابت ساخت in6_addr *b) {
        أكد(a);
        أكد(b);

        واپس memcmp(a, b, حجم(ساخت in6_addr)) == 0;
}

صحیح اندر_addr_equal(صحیح family, ثابت اتحاد اندر_addr_union *a, ثابت اتحاد اندر_addr_union *b) {
        أكد(a);
        أكد(b);

        اگر (family == AF_INET)
                واپس in4_addr_equal(&a->اندر, &b->اندر);

        اگر (family == AF_INET6)
                واپس in6_addr_equal(&a->in6, &b->in6);

        واپس -EAFNOSUPPORT;
}

bool in4_addr_سابقہ_intersect(
                ثابت ساخت in_addr *a,
                غیرعلامتی aprefixlen,
                ثابت ساخت in_addr *b,
                غیرعلامتی bprefixlen) {

        أكد(a);
        أكد(b);

        غیرعلامتی m = MIN3(aprefixlen, bprefixlen, (غیرعلامتی) (حجم(ساخت in_addr) * 8));
        اگر (m == 0)
                واپس true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        واپس (x & n) == 0;
}

bool in6_addr_سابقہ_intersect(
                ثابت ساخت in6_addr *a,
                غیرعلامتی aprefixlen,
                ثابت ساخت in6_addr *b,
                غیرعلامتی bprefixlen) {

        أكد(a);
        أكد(b);

        غیرعلامتی m = MIN3(aprefixlen, bprefixlen, (غیرعلامتی) (حجم(ساخت in6_addr) * 8));
        اگر (m == 0)
                واپس true;

        برائے (size_t i = 0; i < حجم(ساخت in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                اگر ((x & n) != 0)
                        واپس false;

                اگر (m <= 8)
                        توڑو;

                m -= 8;
        }

        واپس true;
}

صحیح اندر_addr_سابقہ_intersect(
                صحیح family,
                ثابت اتحاد اندر_addr_union *a,
                غیرعلامتی aprefixlen,
                ثابت اتحاد اندر_addr_union *b,
                غیرعلامتی bprefixlen) {

        أكد(a);
        أكد(b);

        /* Checks whether there are any addresses that are in both networks. */

        اگر (family == AF_INET)
                واپس in4_addr_سابقہ_intersect(&a->اندر, aprefixlen, &b->اندر, bprefixlen);

        اگر (family == AF_INET6)
                واپس in6_addr_سابقہ_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        واپس -EAFNOSUPPORT;
}

صحیح اندر_addr_random_سابقہ(
                صحیح family,
                اتحاد اندر_addr_union *u,
                غیرعلامتی prefixlen_fixed_part,
                غیرعلامتی prefixlen) {

        أكد(u);

        /* Random network part of an address by one. */

        اگر (prefixlen <= 0)
                واپس 0;

        اگر (family == AF_INET) {
                uint32_t c, n;

                اگر (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                اگر (prefixlen > 32)
                        prefixlen = 32;
                اگر (prefixlen_fixed_part >= prefixlen)
                        واپس -EINVAL;

                c = be32toh(u->اندر.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_بائٹیں(&n, حجم(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->اندر.s_addr = htobe32(n | c);
                واپس 1;
        }

        اگر (family == AF_INET6) {
                ساخت in6_addr n;
                غیرعلامتی i, j;

                اگر (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                اگر (prefixlen > 128)
                        prefixlen = 128;
                اگر (prefixlen_fixed_part >= prefixlen)
                        واپس -EINVAL;

                random_بائٹیں(&n, حجم(n));

                برائے (i = 0; i < 16; i++) {
                        uint8_t نقاب_fixed_part = 0, نقاب_3 = 0;

                        اگر (i < (prefixlen_fixed_part + 7) / 8) {
                                اگر (i < prefixlen_fixed_part / 8)
                                        نقاب_fixed_part = 0xffu;
                                ورنہ {
                                        j = prefixlen_fixed_part % 8;
                                        نقاب_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        اگر (i < (prefixlen + 7) / 8) {
                                اگر (i < prefixlen / 8)
                                        نقاب_3 = 0xffu ^ نقاب_fixed_part;
                                ورنہ {
                                        j = prefixlen % 8;
                                        نقاب_3 = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ نقاب_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= نقاب_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & نقاب_3;
                }

                واپس 1;
        }

        واپس -EAFNOSUPPORT;
}

صحیح اندر_addr_سابقہ_range(
                صحیح family,
                ثابت اتحاد اندر_addr_union *اندر,
                غیرعلامتی prefixlen,
                اتحاد اندر_addr_union *ret_آغاز,
                اتحاد اندر_addr_union *ret_انتہا) {

        صحیح r;

        أكد(اندر);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        انتخاب (family) {
        حالت AF_INET:
                اگر (prefixlen > 32)
                        واپس -EINVAL;
                توڑو;
        حالت AF_INET6:
                اگر (prefixlen > 128)
                        واپس -EINVAL;
                توڑو;
        طےشدہ:
                واپس -EAFNOSUPPORT;
        }

        اگر (prefixlen == 0) {
                /* shortcut for trivial case */
                اگر (ret_آغاز)
                        *ret_آغاز = اندر_ADDR_NULL;
                اگر (ret_انتہا)
                        *ret_انتہا = اندر_ADDR_NULL;
                واپس 0;
        }

        اتحاد اندر_addr_union آغاز = *اندر;
        r = اندر_addr_نقاب(family, &آغاز, prefixlen);
        اگر (r < 0)
                واپس r;

        اتحاد اندر_addr_union انتہا = آغاز;
        uint8_t جوڑو = UINT8_C(1) << ((FAMILY_پتہ_سائز(family) * 8 - prefixlen) % 8);
        برائے (غیرعلامتی i = DIV_باری_اوپر(prefixlen, 8); i-- > 0;)
                اگر (جوڑو > UINT8_MAX - انتہا.بائٹیں[i]) {
                        انتہا.بائٹیں[i] = 0;
                        جوڑو = 1;
                } ورنہ {
                        انتہا.بائٹیں[i] += جوڑو;
                        توڑو;
                }

        اگر (ret_آغاز)
                *ret_آغاز = آغاز;
        اگر (ret_انتہا)
                *ret_انتہا = انتہا;

        واپس 0;
}

صحیح اندر_addr_مقصد_string(صحیح family, ثابت اتحاد اندر_addr_union *u, حرف **ret) {
        _cleanup_free_ حرف *x = تہی;
        size_t l;

        أكد(u);
        أكد(ret);

        اگر (family == AF_INET)
                l = INET_ADDRSTRLEN;
        ورنہ اگر (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        ورنہ
                واپس -EAFNOSUPPORT;

        x = new(حرف, l);
        اگر (!x)
                واپس -ENOMEM;

        errno = 0;
        اگر (!typesafe_inet_ntop(family, u, x, l))
                واپس errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        واپس 0;
}

ثابت حرف* typesafe_inet_ntop(صحیح family, ثابت اتحاد اندر_addr_union *a, حرف *buf, size_t len) {
        واپس inet_ntop(family, a, buf, len);
}

ثابت حرف* typesafe_inet_ntop4(ثابت ساخت in_addr *a, حرف *buf, size_t len) {
        واپس inet_ntop(AF_INET, a, buf, len);
}

ثابت حرف* typesafe_inet_ntop6(ثابت ساخت in6_addr *a, حرف *buf, size_t len) {
        واپس inet_ntop(AF_INET6, a, buf, len);
}

صحیح اندر_addr_سابقہ_مقصد_string(
                صحیح family,
                ثابت اتحاد اندر_addr_union *u,
                غیرعلامتی prefixlen,
                حرف *buf,
                size_t buf_len) {

        أكد(u);
        أكد(buf);

        اگر (!اندر_سیٹ(family, AF_INET, AF_INET6))
                واپس -EAFNOSUPPORT;

        errno = 0;
        اگر (!typesafe_inet_ntop(family, u, buf, buf_len))
                واپس errno_or_else(ENOSPC);

        size_t l = لڑی_لمبائی(buf);
        اگر (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                واپس -ENOSPC;
        واپس 0;
}

صحیح اندر_addr_پورٹ_ifindex_name_مقصد_string(صحیح family, ثابت اتحاد اندر_addr_union *u, uint16_t port, صحیح ifindex, ثابت حرف *server_name, حرف **ret) {
        _cleanup_free_ حرف *ip_str = تہی, *x = تہی;
        صحیح r;

        أكد(اندر_سیٹ(family, AF_INET, AF_INET6));
        أكد(u);
        أكد(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = اندر_addr_مقصد_string(family, u, &ip_str);
        اگر (r < 0)
                واپس r;

        اگر (family == AF_INET6) {
                r = اندر_addr_is_link_local(family, u);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        ifindex = 0;
        } ورنہ
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        اگر (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                واپس 0;
        }

        ثابت حرف *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        اگر (port > 0) {
                اگر (family == AF_INET6) {
                        اگر (ifindex > 0)
                                x = asprintf_محفوظ("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        ورنہ
                                x = asprintf_محفوظ("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } ورنہ
                        x = asprintf_محفوظ("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } ورنہ {
                اگر (ifindex > 0)
                        x = asprintf_محفوظ("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                ورنہ
                        x = strjoin(ip_str, separator, server_name);
        }
        اگر (!x)
                واپس -ENOMEM;

        *ret = TAKE_PTR(x);
        واپس 0;
}

صحیح اندر_addr_from_string(صحیح family, ثابت حرف *s, اتحاد اندر_addr_union *ret) {
        اتحاد اندر_addr_union بفر;
        أكد(s);

        اگر (!اندر_سیٹ(family, AF_INET, AF_INET6))
                واپس -EAFNOSUPPORT;

        errno = 0;
        اگر (inet_pton(family, s, ret ?: &بفر) <= 0)
                واپس errno_or_else(EINVAL);

        واپس 0;
}

صحیح اندر_addr_from_string_auto(ثابت حرف *s, صحیح *ret_family, اتحاد اندر_addr_union *ret) {
        صحیح r;

        أكد(s);

        r = اندر_addr_from_string(AF_INET, s, ret);
        اگر (r >= 0) {
                اگر (ret_family)
                        *ret_family = AF_INET;
                واپس 0;
        }

        r = اندر_addr_from_string(AF_INET6, s, ret);
        اگر (r >= 0) {
                اگر (ret_family)
                        *ret_family = AF_INET6;
                واپس 0;
        }

        واپس -EINVAL;
}

غیرعلامتی حرف in4_addr_netmask_مقصد_prefixlen(ثابت ساخت in_addr *addr) {
        أكد(addr);

        واپس 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
ساخت in_addr* in4_addr_prefixlen_مقصد_netmask(ساخت in_addr *addr, غیرعلامتی حرف prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        اگر (prefixlen == 0)
                addr->s_addr = 0;
        ورنہ
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        واپس addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
ساخت in6_addr* in6_addr_prefixlen_مقصد_netmask(ساخت in6_addr *addr, غیرعلامتی حرف prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 128);

        برائے (غیرعلامتی i = 0; i < 16; i++) {
                uint8_t نقاب_3;

                اگر (prefixlen >= 8) {
                        نقاب_3 = 0xFF;
                        prefixlen -= 8;
                } ورنہ اگر (prefixlen > 0) {
                        نقاب_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ورنہ {
                        أكد(prefixlen == 0);
                        نقاب_3 = 0;
                }

                addr->s6_addr[i] = نقاب_3;
        }

        واپس addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
صحیح اندر_addr_prefixlen_مقصد_netmask(صحیح family, اتحاد اندر_addr_union *addr, غیرعلامتی حرف prefixlen) {
        أكد(addr);

        انتخاب (family) {
        حالت AF_INET:
                in4_addr_prefixlen_مقصد_netmask(&addr->اندر, prefixlen);
                واپس 0;
        حالت AF_INET6:
                in6_addr_prefixlen_مقصد_netmask(&addr->in6, prefixlen);
                واپس 0;
        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

صحیح in4_addr_default_prefixlen(ثابت ساخت in_addr *addr, غیرعلامتی حرف *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        أكد(addr);
        أكد(prefixlen);

        اگر (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        ورنہ اگر (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        ورنہ اگر (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        ورنہ
                /* class D or E, no default prefixlen */
                واپس -ERANGE;

        واپس 0;
}

صحیح in4_addr_default_subnet_نقاب(ثابت ساخت in_addr *addr, ساخت in_addr *نقاب_3) {
        غیرعلامتی حرف prefixlen;
        صحیح r;

        أكد(addr);
        أكد(نقاب_3);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        اگر (r < 0)
                واپس r;

        in4_addr_prefixlen_مقصد_netmask(نقاب_3, prefixlen);
        واپس 0;
}

صحیح in4_addr_نقاب(ساخت in_addr *addr, غیرعلامتی حرف prefixlen) {
        ساخت in_addr نقاب_3;

        أكد(addr);

        اگر (!in4_addr_prefixlen_مقصد_netmask(&نقاب_3, prefixlen))
                واپس -EINVAL;

        addr->s_addr &= نقاب_3.s_addr;
        واپس 0;
}

صحیح in6_addr_نقاب(ساخت in6_addr *addr, غیرعلامتی حرف prefixlen) {
        غیرعلامتی i;

        أكد(addr);

        برائے (i = 0; i < 16; i++) {
                uint8_t نقاب_3;

                اگر (prefixlen >= 8) {
                        نقاب_3 = 0xFF;
                        prefixlen -= 8;
                } ورنہ اگر (prefixlen > 0) {
                        نقاب_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } ورنہ {
                        أكد(prefixlen == 0);
                        نقاب_3 = 0;
                }

                addr->s6_addr[i] &= نقاب_3;
        }

        واپس 0;
}

صحیح اندر_addr_نقاب(صحیح family, اتحاد اندر_addr_union *addr, غیرعلامتی حرف prefixlen) {
        أكد(addr);

        انتخاب (family) {
        حالت AF_INET:
                واپس in4_addr_نقاب(&addr->اندر, prefixlen);
        حالت AF_INET6:
                واپس in6_addr_نقاب(&addr->in6, prefixlen);
        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

صحیح in4_addr_سابقہ_covers_بھرا(
                ثابت ساخت in_addr *سابقہ,
                غیرعلامتی حرف prefixlen,
                ثابت ساخت in_addr *پتہ,
                غیرعلامتی حرف پتہ_prefixlen) {

        ساخت in_addr masked_سابقہ, masked_پتہ;
        صحیح r;

        أكد(سابقہ);
        أكد(پتہ);

        اگر (prefixlen > پتہ_prefixlen)
                واپس false;

        masked_سابقہ = *سابقہ;
        r = in4_addr_نقاب(&masked_سابقہ, prefixlen);
        اگر (r < 0)
                واپس r;

        masked_پتہ = *پتہ;
        r = in4_addr_نقاب(&masked_پتہ, prefixlen);
        اگر (r < 0)
                واپس r;

        واپس in4_addr_equal(&masked_سابقہ, &masked_پتہ);
}

صحیح in6_addr_سابقہ_covers_بھرا(
                ثابت ساخت in6_addr *سابقہ,
                غیرعلامتی حرف prefixlen,
                ثابت ساخت in6_addr *پتہ,
                غیرعلامتی حرف پتہ_prefixlen) {

        ساخت in6_addr masked_سابقہ, masked_پتہ;
        صحیح r;

        أكد(سابقہ);
        أكد(پتہ);

        اگر (prefixlen > پتہ_prefixlen)
                واپس false;

        masked_سابقہ = *سابقہ;
        r = in6_addr_نقاب(&masked_سابقہ, prefixlen);
        اگر (r < 0)
                واپس r;

        masked_پتہ = *پتہ;
        r = in6_addr_نقاب(&masked_پتہ, prefixlen);
        اگر (r < 0)
                واپس r;

        واپس in6_addr_equal(&masked_سابقہ, &masked_پتہ);
}

صحیح اندر_addr_سابقہ_covers_بھرا(
                صحیح family,
                ثابت اتحاد اندر_addr_union *سابقہ,
                غیرعلامتی حرف prefixlen,
                ثابت اتحاد اندر_addr_union *پتہ,
                غیرعلامتی حرف پتہ_prefixlen) {

        أكد(سابقہ);
        أكد(پتہ);

        انتخاب (family) {
        حالت AF_INET:
                واپس in4_addr_سابقہ_covers_بھرا(&سابقہ->اندر, prefixlen, &پتہ->اندر, پتہ_prefixlen);
        حالت AF_INET6:
                واپس in6_addr_سابقہ_covers_بھرا(&سابقہ->in6, prefixlen, &پتہ->in6, پتہ_prefixlen);
        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

صحیح اندر_addr_parse_prefixlen(صحیح family, ثابت حرف *p, غیرعلامتی حرف *ret) {
        uint8_t u;
        صحیح r;

        أكد(ret);

        اگر (!اندر_سیٹ(family, AF_INET, AF_INET6))
                واپس -EAFNOSUPPORT;

        r = محفوظ_atou8(p, &u);
        اگر (r < 0)
                واپس r;

        اگر (u > FAMILY_پتہ_سائز(family) * 8)
                واپس -ERANGE;

        *ret = u;
        واپس 0;
}

صحیح اندر_addr_سابقہ_from_string(
                ثابت حرف *p,
                صحیح family,
                اتحاد اندر_addr_union *ret_سابقہ,
                غیرعلامتی حرف *ret_prefixlen) {

        _cleanup_free_ حرف *str = تہی;
        اتحاد اندر_addr_union بفر;
        ثابت حرف *e, *l;
        غیرعلامتی حرف k;
        صحیح r;

        أكد(p);

        اگر (!اندر_سیٹ(family, AF_INET, AF_INET6))
                واپس -EAFNOSUPPORT;

        e = strchr(p, '/');
        اگر (e) {
                str = strndup(p, e - p);
                اگر (!str)
                        واپس -ENOMEM;

                l = str;
        } ورنہ
                l = p;

        r = اندر_addr_from_string(family, l, &بفر);
        اگر (r < 0)
                واپس r;

        اگر (e) {
                r = اندر_addr_parse_prefixlen(family, e+1, &k);
                اگر (r < 0)
                        واپس r;
        } ورنہ
                k = FAMILY_پتہ_سائز(family) * 8;

        اگر (ret_سابقہ)
                *ret_سابقہ = بفر;
        اگر (ret_prefixlen)
                *ret_prefixlen = k;

        واپس 0;
}

صحیح اندر_addr_سابقہ_from_string_auto_بھرا(
                ثابت حرف *p,
                اندر_Addr_سابقہ_Len_منوال منوال,
                صحیح *ret_family,
                اتحاد اندر_addr_union *ret_سابقہ,
                غیرعلامتی حرف *ret_prefixlen) {

        _cleanup_free_ حرف *str = تہی;
        اتحاد اندر_addr_union بفر;
        ثابت حرف *e, *l;
        غیرعلامتی حرف k;
        صحیح family, r;

        أكد(p);

        e = strchr(p, '/');
        اگر (e) {
                str = strndup(p, e - p);
                اگر (!str)
                        واپس -ENOMEM;

                l = str;
        } ورنہ
                l = p;

        r = اندر_addr_from_string_auto(l, &family, &بفر);
        اگر (r < 0)
                واپس r;

        اگر (e) {
                r = اندر_addr_parse_prefixlen(family, e+1, &k);
                اگر (r < 0)
                        واپس r;
        } ورنہ
                انتخاب (منوال) {
                حالت PREFIXLEN_بھرا:
                        k = FAMILY_پتہ_سائز(family) * 8;
                        توڑو;
                حالت PREFIXLEN_REFUSE:
                        واپس -ENOANO; /* To distinguish this error from others. */
                طےشدہ:
                        assert_not_reached();
                }

        اگر (ret_family)
                *ret_family = family;
        اگر (ret_سابقہ)
                *ret_سابقہ = بفر;
        اگر (ret_prefixlen)
                *ret_prefixlen = k;

        واپس 0;
}

خالی اندر_addr_hash_func(ثابت اتحاد اندر_addr_union *u, صحیح family, ساخت siphash *state) {
        أكد(u);
        أكد(state);

        siphash24_compress(u->بائٹیں, FAMILY_پتہ_سائز(family), state);
}

خالی اندر_addr_ڈیٹا_hash_func(ثابت ساخت اندر_addr_ڈیٹا *a, ساخت siphash *state) {
        أكد(a);
        أكد(state);

        siphash24_compress_typesafe(a->family, state);
        اندر_addr_hash_func(&a->پتہ, a->family, state);
}

صحیح اندر_addr_ڈیٹا_compare_func(ثابت ساخت اندر_addr_ڈیٹا *x, ثابت ساخت اندر_addr_ڈیٹا *y) {
        صحیح r;

        أكد(x);
        أكد(y);

        r = CMP(x->family, y->family);
        اگر (r != 0)
                واپس r;

        واپس memcmp(&x->پتہ, &y->پتہ, FAMILY_پتہ_سائز(x->family));
}

DEFINE_HASH_OPS(
        اندر_addr_ڈیٹا_hash_ops,
        ساخت اندر_addr_ڈیٹا,
        اندر_addr_ڈیٹا_hash_func,
        اندر_addr_ڈیٹا_compare_func);

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
        اندر_addr_ڈیٹا_hash_ops_free,
        ساخت اندر_addr_ڈیٹا,
        اندر_addr_ڈیٹا_hash_func,
        اندر_addr_ڈیٹا_compare_func,
        آزاد);

خالی in6_addr_hash_func(ثابت ساخت in6_addr *addr, ساخت siphash *state) {
        أكد(addr);
        أكد(state);

        siphash24_compress_typesafe(*addr, state);
}

صحیح in6_addr_compare_func(ثابت ساخت in6_addr *a, ثابت ساخت in6_addr *b) {
        أكد(a);
        أكد(b);

        واپس memcmp(a, b, حجم(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        ساخت in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
        in6_addr_hash_ops_free,
        ساخت in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        آزاد);
