/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <arpa/inet.h>
#ضمن <endian.h>
#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "hash-funcs.h"
#ضمن "in-addr-util.h"
#ضمن "logarithm.h"
#ضمن "memory-util.h"
#ضمن "parse-util.h"
#ضمن "random-util.h"
#ضمن "siphash24.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

bool in4_addr_is_null(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع a->s_addr == 0;
}

bool in6_addr_is_null(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع eqzero(a->s6_addr32);
}

صحيح درون_addr_is_null(صحيح family, ثابت اتحاد درون_addr_union *u) {
        أكد(u);

        إذا (family == AF_INET)
                أرجع in4_addr_is_null(&u->درون);

        إذا (family == AF_INET6)
                أرجع in6_addr_is_null(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ثابت بنية in_addr *a) {
        أكد(a);

        إذا (!in4_addr_is_link_local(a))
                أرجع false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        أرجع !درون_مجموعه(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

صحيح درون_addr_is_link_local(صحيح family, ثابت اتحاد درون_addr_union *u) {
        أكد(u);

        إذا (family == AF_INET)
                أرجع in4_addr_is_link_local(&u->درون);

        إذا (family == AF_INET6)
                أرجع in6_addr_is_link_local(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ثابت بنية in6_addr *a) {
        أكد(a);

        /* ff02::1 */
        أرجع be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع a->s6_addr[0] == 0xff;
}

صحيح درون_addr_is_multicast(صحيح family, ثابت اتحاد درون_addr_union *u) {
        أكد(u);

        إذا (family == AF_INET)
                أرجع in4_addr_is_multicast(&u->درون);

        إذا (family == AF_INET6)
                أرجع in6_addr_is_multicast(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ثابت بنية in_addr *a) {
        أكد(a);

        أرجع (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ثابت بنية in_addr *a) {
        أكد(a);

        /* All of 127.x.x.x is localhost. */
        أرجع (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ثابت بنية in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        أرجع !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ساكن bool in6_addr_is_loopback(ثابت بنية in6_addr *a) {
        أرجع memcmp(a, &(بنية in6_addr) IN6ADDR_LOOPBACK_INIT, حجم(بنية in6_addr)) == 0;
}

صحيح درون_addr_is_localhost(صحيح family, ثابت اتحاد درون_addr_union *u) {
        أكد(u);

        إذا (family == AF_INET)
                أرجع in4_addr_is_localhost(&u->درون);

        إذا (family == AF_INET6)
                أرجع in6_addr_is_loopback(&u->in6);

        أرجع -EAFNOSUPPORT;
}

صحيح درون_addr_is_localhost_one(صحيح family, ثابت اتحاد درون_addr_union *u) {
        أكد(u);

        إذا (family == AF_INET)
                /* 127.0.0.1 */
                أرجع be32toh(u->درون.s_addr) == UINT32_C(0x7F000001);

        إذا (family == AF_INET6)
                أرجع in6_addr_is_loopback(&u->in6);

        أرجع -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_آدرس(ثابت بنية in6_addr *a) {
        أكد(a);

        أرجع a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ثابت بنية in_addr *a, ثابت بنية in_addr *b) {
        أكد(a);
        أكد(b);

        أرجع a->s_addr == b->s_addr;
}

bool in6_addr_equal(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) {
        أكد(a);
        أكد(b);

        أرجع memcmp(a, b, حجم(بنية in6_addr)) == 0;
}

صحيح درون_addr_equal(صحيح family, ثابت اتحاد درون_addr_union *a, ثابت اتحاد درون_addr_union *b) {
        أكد(a);
        أكد(b);

        إذا (family == AF_INET)
                أرجع in4_addr_equal(&a->درون, &b->درون);

        إذا (family == AF_INET6)
                أرجع in6_addr_equal(&a->in6, &b->in6);

        أرجع -EAFNOSUPPORT;
}

bool in4_addr_پیشوند_intersect(
                ثابت بنية in_addr *a,
                بیعلامت aprefixlen,
                ثابت بنية in_addr *b,
                بیعلامت bprefixlen) {

        أكد(a);
        أكد(b);

        بیعلامت m = MIN3(aprefixlen, bprefixlen, (بیعلامت) (حجم(بنية in_addr) * 8));
        إذا (m == 0)
                أرجع true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        أرجع (x & n) == 0;
}

bool in6_addr_پیشوند_intersect(
                ثابت بنية in6_addr *a,
                بیعلامت aprefixlen,
                ثابت بنية in6_addr *b,
                بیعلامت bprefixlen) {

        أكد(a);
        أكد(b);

        بیعلامت m = MIN3(aprefixlen, bprefixlen, (بیعلامت) (حجم(بنية in6_addr) * 8));
        إذا (m == 0)
                أرجع true;

        لكل (size_t i = 0; i < حجم(بنية in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                إذا ((x & n) != 0)
                        أرجع false;

                إذا (m <= 8)
                        اكسر;

                m -= 8;
        }

        أرجع true;
}

صحيح درون_addr_پیشوند_intersect(
                صحيح family,
                ثابت اتحاد درون_addr_union *a,
                بیعلامت aprefixlen,
                ثابت اتحاد درون_addr_union *b,
                بیعلامت bprefixlen) {

        أكد(a);
        أكد(b);

        /* Checks whether there are any addresses that are in both networks. */

        إذا (family == AF_INET)
                أرجع in4_addr_پیشوند_intersect(&a->درون, aprefixlen, &b->درون, bprefixlen);

        إذا (family == AF_INET6)
                أرجع in6_addr_پیشوند_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        أرجع -EAFNOSUPPORT;
}

صحيح درون_addr_random_پیشوند(
                صحيح family,
                اتحاد درون_addr_union *u,
                بیعلامت prefixlen_fixed_part,
                بیعلامت prefixlen) {

        أكد(u);

        /* Random network part of an address by one. */

        إذا (prefixlen <= 0)
                أرجع 0;

        إذا (family == AF_INET) {
                uint32_t c, n;

                إذا (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                إذا (prefixlen > 32)
                        prefixlen = 32;
                إذا (prefixlen_fixed_part >= prefixlen)
                        أرجع -EINVAL;

                c = be32toh(u->درون.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_بایتها(&n, حجم(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->درون.s_addr = htobe32(n | c);
                أرجع 1;
        }

        إذا (family == AF_INET6) {
                بنية in6_addr n;
                بیعلامت i, j;

                إذا (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                إذا (prefixlen > 128)
                        prefixlen = 128;
                إذا (prefixlen_fixed_part >= prefixlen)
                        أرجع -EINVAL;

                random_بایتها(&n, حجم(n));

                لكل (i = 0; i < 16; i++) {
                        uint8_t ماسک_fixed_part = 0, ماسک_3 = 0;

                        إذا (i < (prefixlen_fixed_part + 7) / 8) {
                                إذا (i < prefixlen_fixed_part / 8)
                                        ماسک_fixed_part = 0xffu;
                                وإلا {
                                        j = prefixlen_fixed_part % 8;
                                        ماسک_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        إذا (i < (prefixlen + 7) / 8) {
                                إذا (i < prefixlen / 8)
                                        ماسک_3 = 0xffu ^ ماسک_fixed_part;
                                وإلا {
                                        j = prefixlen % 8;
                                        ماسک_3 = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ ماسک_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= ماسک_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & ماسک_3;
                }

                أرجع 1;
        }

        أرجع -EAFNOSUPPORT;
}

صحيح درون_addr_پیشوند_range(
                صحيح family,
                ثابت اتحاد درون_addr_union *درون,
                بیعلامت prefixlen,
                اتحاد درون_addr_union *ret_شروع,
                اتحاد درون_addr_union *ret_انتها) {

        صحيح r;

        أكد(درون);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        بدل (family) {
        حالة AF_INET:
                إذا (prefixlen > 32)
                        أرجع -EINVAL;
                اكسر;
        حالة AF_INET6:
                إذا (prefixlen > 128)
                        أرجع -EINVAL;
                اكسر;
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }

        إذا (prefixlen == 0) {
                /* shortcut for trivial case */
                إذا (ret_شروع)
                        *ret_شروع = درون_ADDR_NULL;
                إذا (ret_انتها)
                        *ret_انتها = درون_ADDR_NULL;
                أرجع 0;
        }

        اتحاد درون_addr_union شروع = *درون;
        r = درون_addr_ماسک(family, &شروع, prefixlen);
        إذا (r < 0)
                أرجع r;

        اتحاد درون_addr_union انتها = شروع;
        uint8_t جمع_کن = UINT8_C(1) << ((FAMILY_آدرس_اندازه(family) * 8 - prefixlen) % 8);
        لكل (بیعلامت i = DIV_دور_به_بالا(prefixlen, 8); i-- > 0;)
                إذا (جمع_کن > UINT8_MAX - انتها.بایتها[i]) {
                        انتها.بایتها[i] = 0;
                        جمع_کن = 1;
                } وإلا {
                        انتها.بایتها[i] += جمع_کن;
                        اكسر;
                }

        إذا (ret_شروع)
                *ret_شروع = شروع;
        إذا (ret_انتها)
                *ret_انتها = انتها;

        أرجع 0;
}

صحيح درون_addr_مقصد_string(صحيح family, ثابت اتحاد درون_addr_union *u, حرف **ret) {
        _cleanup_free_ حرف *x = عدم;
        size_t l;

        أكد(u);
        أكد(ret);

        إذا (family == AF_INET)
                l = INET_ADDRSTRLEN;
        وإلا إذا (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        وإلا
                أرجع -EAFNOSUPPORT;

        x = new(حرف, l);
        إذا (!x)
                أرجع -ENOMEM;

        errno = 0;
        إذا (!typesafe_inet_ntop(family, u, x, l))
                أرجع errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        أرجع 0;
}

ثابت حرف* typesafe_inet_ntop(صحيح family, ثابت اتحاد درون_addr_union *a, حرف *buf, size_t len) {
        أرجع inet_ntop(family, a, buf, len);
}

ثابت حرف* typesafe_inet_ntop4(ثابت بنية in_addr *a, حرف *buf, size_t len) {
        أرجع inet_ntop(AF_INET, a, buf, len);
}

ثابت حرف* typesafe_inet_ntop6(ثابت بنية in6_addr *a, حرف *buf, size_t len) {
        أرجع inet_ntop(AF_INET6, a, buf, len);
}

صحيح درون_addr_پیشوند_مقصد_string(
                صحيح family,
                ثابت اتحاد درون_addr_union *u,
                بیعلامت prefixlen,
                حرف *buf,
                size_t buf_len) {

        أكد(u);
        أكد(buf);

        إذا (!درون_مجموعه(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        errno = 0;
        إذا (!typesafe_inet_ntop(family, u, buf, buf_len))
                أرجع errno_or_else(ENOSPC);

        size_t l = طول_رشته(buf);
        إذا (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                أرجع -ENOSPC;
        أرجع 0;
}

صحيح درون_addr_پورت_ifindex_name_مقصد_string(صحيح family, ثابت اتحاد درون_addr_union *u, uint16_t port, صحيح ifindex, ثابت حرف *server_name, حرف **ret) {
        _cleanup_free_ حرف *ip_str = عدم, *x = عدم;
        صحيح r;

        أكد(درون_مجموعه(family, AF_INET, AF_INET6));
        أكد(u);
        أكد(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = درون_addr_مقصد_string(family, u, &ip_str);
        إذا (r < 0)
                أرجع r;

        إذا (family == AF_INET6) {
                r = درون_addr_is_link_local(family, u);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        ifindex = 0;
        } وإلا
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        إذا (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                أرجع 0;
        }

        ثابت حرف *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        إذا (port > 0) {
                إذا (family == AF_INET6) {
                        إذا (ifindex > 0)
                                x = asprintf_امن("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        وإلا
                                x = asprintf_امن("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } وإلا
                        x = asprintf_امن("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } وإلا {
                إذا (ifindex > 0)
                        x = asprintf_امن("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                وإلا
                        x = strjoin(ip_str, separator, server_name);
        }
        إذا (!x)
                أرجع -ENOMEM;

        *ret = TAKE_PTR(x);
        أرجع 0;
}

صحيح درون_addr_from_string(صحيح family, ثابت حرف *s, اتحاد درون_addr_union *ret) {
        اتحاد درون_addr_union میانگیر;
        أكد(s);

        إذا (!درون_مجموعه(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        errno = 0;
        إذا (inet_pton(family, s, ret ?: &میانگیر) <= 0)
                أرجع errno_or_else(EINVAL);

        أرجع 0;
}

صحيح درون_addr_from_string_auto(ثابت حرف *s, صحيح *ret_family, اتحاد درون_addr_union *ret) {
        صحيح r;

        أكد(s);

        r = درون_addr_from_string(AF_INET, s, ret);
        إذا (r >= 0) {
                إذا (ret_family)
                        *ret_family = AF_INET;
                أرجع 0;
        }

        r = درون_addr_from_string(AF_INET6, s, ret);
        إذا (r >= 0) {
                إذا (ret_family)
                        *ret_family = AF_INET6;
                أرجع 0;
        }

        أرجع -EINVAL;
}

بیعلامت حرف in4_addr_netmask_مقصد_prefixlen(ثابت بنية in_addr *addr) {
        أكد(addr);

        أرجع 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
بنية in_addr* in4_addr_prefixlen_مقصد_netmask(بنية in_addr *addr, بیعلامت حرف prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        إذا (prefixlen == 0)
                addr->s_addr = 0;
        وإلا
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        أرجع addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
بنية in6_addr* in6_addr_prefixlen_مقصد_netmask(بنية in6_addr *addr, بیعلامت حرف prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 128);

        لكل (بیعلامت i = 0; i < 16; i++) {
                uint8_t ماسک_3;

                إذا (prefixlen >= 8) {
                        ماسک_3 = 0xFF;
                        prefixlen -= 8;
                } وإلا إذا (prefixlen > 0) {
                        ماسک_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } وإلا {
                        أكد(prefixlen == 0);
                        ماسک_3 = 0;
                }

                addr->s6_addr[i] = ماسک_3;
        }

        أرجع addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
صحيح درون_addr_prefixlen_مقصد_netmask(صحيح family, اتحاد درون_addr_union *addr, بیعلامت حرف prefixlen) {
        أكد(addr);

        بدل (family) {
        حالة AF_INET:
                in4_addr_prefixlen_مقصد_netmask(&addr->درون, prefixlen);
                أرجع 0;
        حالة AF_INET6:
                in6_addr_prefixlen_مقصد_netmask(&addr->in6, prefixlen);
                أرجع 0;
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح in4_addr_default_prefixlen(ثابت بنية in_addr *addr, بیعلامت حرف *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        أكد(addr);
        أكد(prefixlen);

        إذا (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        وإلا إذا (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        وإلا إذا (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        وإلا
                /* class D or E, no default prefixlen */
                أرجع -ERANGE;

        أرجع 0;
}

صحيح in4_addr_default_subnet_ماسک(ثابت بنية in_addr *addr, بنية in_addr *ماسک_3) {
        بیعلامت حرف prefixlen;
        صحيح r;

        أكد(addr);
        أكد(ماسک_3);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        إذا (r < 0)
                أرجع r;

        in4_addr_prefixlen_مقصد_netmask(ماسک_3, prefixlen);
        أرجع 0;
}

صحيح in4_addr_ماسک(بنية in_addr *addr, بیعلامت حرف prefixlen) {
        بنية in_addr ماسک_3;

        أكد(addr);

        إذا (!in4_addr_prefixlen_مقصد_netmask(&ماسک_3, prefixlen))
                أرجع -EINVAL;

        addr->s_addr &= ماسک_3.s_addr;
        أرجع 0;
}

صحيح in6_addr_ماسک(بنية in6_addr *addr, بیعلامت حرف prefixlen) {
        بیعلامت i;

        أكد(addr);

        لكل (i = 0; i < 16; i++) {
                uint8_t ماسک_3;

                إذا (prefixlen >= 8) {
                        ماسک_3 = 0xFF;
                        prefixlen -= 8;
                } وإلا إذا (prefixlen > 0) {
                        ماسک_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } وإلا {
                        أكد(prefixlen == 0);
                        ماسک_3 = 0;
                }

                addr->s6_addr[i] &= ماسک_3;
        }

        أرجع 0;
}

صحيح درون_addr_ماسک(صحيح family, اتحاد درون_addr_union *addr, بیعلامت حرف prefixlen) {
        أكد(addr);

        بدل (family) {
        حالة AF_INET:
                أرجع in4_addr_ماسک(&addr->درون, prefixlen);
        حالة AF_INET6:
                أرجع in6_addr_ماسک(&addr->in6, prefixlen);
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح in4_addr_پیشوند_covers_پر(
                ثابت بنية in_addr *پیشوند,
                بیعلامت حرف prefixlen,
                ثابت بنية in_addr *آدرس,
                بیعلامت حرف آدرس_prefixlen) {

        بنية in_addr masked_پیشوند, masked_آدرس;
        صحيح r;

        أكد(پیشوند);
        أكد(آدرس);

        إذا (prefixlen > آدرس_prefixlen)
                أرجع false;

        masked_پیشوند = *پیشوند;
        r = in4_addr_ماسک(&masked_پیشوند, prefixlen);
        إذا (r < 0)
                أرجع r;

        masked_آدرس = *آدرس;
        r = in4_addr_ماسک(&masked_آدرس, prefixlen);
        إذا (r < 0)
                أرجع r;

        أرجع in4_addr_equal(&masked_پیشوند, &masked_آدرس);
}

صحيح in6_addr_پیشوند_covers_پر(
                ثابت بنية in6_addr *پیشوند,
                بیعلامت حرف prefixlen,
                ثابت بنية in6_addr *آدرس,
                بیعلامت حرف آدرس_prefixlen) {

        بنية in6_addr masked_پیشوند, masked_آدرس;
        صحيح r;

        أكد(پیشوند);
        أكد(آدرس);

        إذا (prefixlen > آدرس_prefixlen)
                أرجع false;

        masked_پیشوند = *پیشوند;
        r = in6_addr_ماسک(&masked_پیشوند, prefixlen);
        إذا (r < 0)
                أرجع r;

        masked_آدرس = *آدرس;
        r = in6_addr_ماسک(&masked_آدرس, prefixlen);
        إذا (r < 0)
                أرجع r;

        أرجع in6_addr_equal(&masked_پیشوند, &masked_آدرس);
}

صحيح درون_addr_پیشوند_covers_پر(
                صحيح family,
                ثابت اتحاد درون_addr_union *پیشوند,
                بیعلامت حرف prefixlen,
                ثابت اتحاد درون_addr_union *آدرس,
                بیعلامت حرف آدرس_prefixlen) {

        أكد(پیشوند);
        أكد(آدرس);

        بدل (family) {
        حالة AF_INET:
                أرجع in4_addr_پیشوند_covers_پر(&پیشوند->درون, prefixlen, &آدرس->درون, آدرس_prefixlen);
        حالة AF_INET6:
                أرجع in6_addr_پیشوند_covers_پر(&پیشوند->in6, prefixlen, &آدرس->in6, آدرس_prefixlen);
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح درون_addr_parse_prefixlen(صحيح family, ثابت حرف *p, بیعلامت حرف *ret) {
        uint8_t u;
        صحيح r;

        أكد(ret);

        إذا (!درون_مجموعه(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        r = امن_atou8(p, &u);
        إذا (r < 0)
                أرجع r;

        إذا (u > FAMILY_آدرس_اندازه(family) * 8)
                أرجع -ERANGE;

        *ret = u;
        أرجع 0;
}

صحيح درون_addr_پیشوند_from_string(
                ثابت حرف *p,
                صحيح family,
                اتحاد درون_addr_union *ret_پیشوند,
                بیعلامت حرف *ret_prefixlen) {

        _cleanup_free_ حرف *str = عدم;
        اتحاد درون_addr_union میانگیر;
        ثابت حرف *e, *l;
        بیعلامت حرف k;
        صحيح r;

        أكد(p);

        إذا (!درون_مجموعه(family, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        e = strchr(p, '/');
        إذا (e) {
                str = strndup(p, e - p);
                إذا (!str)
                        أرجع -ENOMEM;

                l = str;
        } وإلا
                l = p;

        r = درون_addr_from_string(family, l, &میانگیر);
        إذا (r < 0)
                أرجع r;

        إذا (e) {
                r = درون_addr_parse_prefixlen(family, e+1, &k);
                إذا (r < 0)
                        أرجع r;
        } وإلا
                k = FAMILY_آدرس_اندازه(family) * 8;

        إذا (ret_پیشوند)
                *ret_پیشوند = میانگیر;
        إذا (ret_prefixlen)
                *ret_prefixlen = k;

        أرجع 0;
}

صحيح درون_addr_پیشوند_from_string_auto_پر(
                ثابت حرف *p,
                درون_Addr_پیشوند_Len_نما نما,
                صحيح *ret_family,
                اتحاد درون_addr_union *ret_پیشوند,
                بیعلامت حرف *ret_prefixlen) {

        _cleanup_free_ حرف *str = عدم;
        اتحاد درون_addr_union میانگیر;
        ثابت حرف *e, *l;
        بیعلامت حرف k;
        صحيح family, r;

        أكد(p);

        e = strchr(p, '/');
        إذا (e) {
                str = strndup(p, e - p);
                إذا (!str)
                        أرجع -ENOMEM;

                l = str;
        } وإلا
                l = p;

        r = درون_addr_from_string_auto(l, &family, &میانگیر);
        إذا (r < 0)
                أرجع r;

        إذا (e) {
                r = درون_addr_parse_prefixlen(family, e+1, &k);
                إذا (r < 0)
                        أرجع r;
        } وإلا
                بدل (نما) {
                حالة PREFIXLEN_پر:
                        k = FAMILY_آدرس_اندازه(family) * 8;
                        اكسر;
                حالة PREFIXLEN_REFUSE:
                        أرجع -ENOANO; /* To distinguish this error from others. */
                افتراضي:
                        assert_not_reached();
                }

        إذا (ret_family)
                *ret_family = family;
        إذا (ret_پیشوند)
                *ret_پیشوند = میانگیر;
        إذا (ret_prefixlen)
                *ret_prefixlen = k;

        أرجع 0;
}

فراغ درون_addr_hash_func(ثابت اتحاد درون_addr_union *u, صحيح family, بنية siphash *state) {
        أكد(u);
        أكد(state);

        siphash24_compress(u->بایتها, FAMILY_آدرس_اندازه(family), state);
}

فراغ درون_addr_داده_hash_func(ثابت بنية درون_addr_داده *a, بنية siphash *state) {
        أكد(a);
        أكد(state);

        siphash24_compress_typesafe(a->family, state);
        درون_addr_hash_func(&a->آدرس, a->family, state);
}

صحيح درون_addr_داده_compare_func(ثابت بنية درون_addr_داده *x, ثابت بنية درون_addr_داده *y) {
        صحيح r;

        أكد(x);
        أكد(y);

        r = CMP(x->family, y->family);
        إذا (r != 0)
                أرجع r;

        أرجع memcmp(&x->آدرس, &y->آدرس, FAMILY_آدرس_اندازه(x->family));
}

DEFINE_HASH_OPS(
        درون_addr_داده_hash_ops,
        بنية درون_addr_داده,
        درون_addr_داده_hash_func,
        درون_addr_داده_compare_func);

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
        درون_addr_داده_hash_ops_free,
        بنية درون_addr_داده,
        درون_addr_داده_hash_func,
        درون_addr_داده_compare_func,
        حرر);

فراغ in6_addr_hash_func(ثابت بنية in6_addr *addr, بنية siphash *state) {
        أكد(addr);
        أكد(state);

        siphash24_compress_typesafe(*addr, state);
}

صحيح in6_addr_compare_func(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) {
        أكد(a);
        أكد(b);

        أرجع memcmp(a, b, حجم(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        بنية in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
        in6_addr_hash_ops_free,
        بنية in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        حرر);
