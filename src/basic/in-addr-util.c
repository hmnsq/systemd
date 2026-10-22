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

bool in4_addr_is_null(ثابت ساختار in_addr *a) {
        أكد(a);

        بازگشت a->s_addr == 0;
}

bool in6_addr_is_null(ثابت ساختار in6_addr *a) {
        أكد(a);

        بازگشت eqzero(a->s6_addr32);
}

صحیح in_addr_is_null(صحیح family, ثابت اجتماع in_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                بازگشت in4_addr_is_null(&u->in);

        اگر (family == AF_INET6)
                بازگشت in6_addr_is_null(&u->in6);

        بازگشت -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(ثابت ساختار in_addr *a) {
        أكد(a);

        بازگشت (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(ثابت ساختار in_addr *a) {
        أكد(a);

        اگر (!in4_addr_is_link_local(a))
                بازگشت false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        بازگشت !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(ثابت ساختار in6_addr *a) {
        أكد(a);

        بازگشت (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

صحیح in_addr_is_link_local(صحیح family, ثابت اجتماع in_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                بازگشت in4_addr_is_link_local(&u->in);

        اگر (family == AF_INET6)
                بازگشت in6_addr_is_link_local(&u->in6);

        بازگشت -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(ثابت ساختار in6_addr *a) {
        أكد(a);

        /* ff02::1 */
        بازگشت be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(ثابت ساختار in_addr *a) {
        أكد(a);

        بازگشت IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(ثابت ساختار in6_addr *a) {
        أكد(a);

        بازگشت a->s6_addr[0] == 0xff;
}

صحیح in_addr_is_multicast(صحیح family, ثابت اجتماع in_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                بازگشت in4_addr_is_multicast(&u->in);

        اگر (family == AF_INET6)
                بازگشت in6_addr_is_multicast(&u->in6);

        بازگشت -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(ثابت ساختار in_addr *a) {
        أكد(a);

        بازگشت (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(ثابت ساختار in_addr *a) {
        أكد(a);

        /* All of 127.x.x.x is localhost. */
        بازگشت (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(ثابت ساختار in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        بازگشت !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

ایستا bool in6_addr_is_loopback(ثابت ساختار in6_addr *a) {
        بازگشت memcmp(a, &(ساختار in6_addr) IN6ADDR_LOOPBACK_INIT, اندازه(ساختار in6_addr)) == 0;
}

صحیح in_addr_is_localhost(صحیح family, ثابت اجتماع in_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                بازگشت in4_addr_is_localhost(&u->in);

        اگر (family == AF_INET6)
                بازگشت in6_addr_is_loopback(&u->in6);

        بازگشت -EAFNOSUPPORT;
}

صحیح in_addr_is_localhost_one(صحیح family, ثابت اجتماع in_addr_union *u) {
        أكد(u);

        اگر (family == AF_INET)
                /* 127.0.0.1 */
                بازگشت be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        اگر (family == AF_INET6)
                بازگشت in6_addr_is_loopback(&u->in6);

        بازگشت -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(ثابت ساختار in6_addr *a) {
        أكد(a);

        بازگشت a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(ثابت ساختار in_addr *a, ثابت ساختار in_addr *b) {
        أكد(a);
        أكد(b);

        بازگشت a->s_addr == b->s_addr;
}

bool in6_addr_equal(ثابت ساختار in6_addr *a, ثابت ساختار in6_addr *b) {
        أكد(a);
        أكد(b);

        بازگشت memcmp(a, b, اندازه(ساختار in6_addr)) == 0;
}

صحیح in_addr_equal(صحیح family, ثابت اجتماع in_addr_union *a, ثابت اجتماع in_addr_union *b) {
        أكد(a);
        أكد(b);

        اگر (family == AF_INET)
                بازگشت in4_addr_equal(&a->in, &b->in);

        اگر (family == AF_INET6)
                بازگشت in6_addr_equal(&a->in6, &b->in6);

        بازگشت -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                ثابت ساختار in_addr *a,
                unsigned aprefixlen,
                ثابت ساختار in_addr *b,
                unsigned bprefixlen) {

        أكد(a);
        أكد(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (اندازه(ساختار in_addr) * 8));
        اگر (m == 0)
                بازگشت true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        بازگشت (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                ثابت ساختار in6_addr *a,
                unsigned aprefixlen,
                ثابت ساختار in6_addr *b,
                unsigned bprefixlen) {

        أكد(a);
        أكد(b);

        unsigned m = MIN3(aprefixlen, bprefixlen, (unsigned) (اندازه(ساختار in6_addr) * 8));
        اگر (m == 0)
                بازگشت true;

        برای (size_t i = 0; i < اندازه(ساختار in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                اگر ((x & n) != 0)
                        بازگشت false;

                اگر (m <= 8)
                        بشکن;

                m -= 8;
        }

        بازگشت true;
}

صحیح in_addr_prefix_intersect(
                صحیح family,
                ثابت اجتماع in_addr_union *a,
                unsigned aprefixlen,
                ثابت اجتماع in_addr_union *b,
                unsigned bprefixlen) {

        أكد(a);
        أكد(b);

        /* Checks whether there are any addresses that are in both networks. */

        اگر (family == AF_INET)
                بازگشت in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        اگر (family == AF_INET6)
                بازگشت in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        بازگشت -EAFNOSUPPORT;
}

صحیح in_addr_random_prefix(
                صحیح family,
                اجتماع in_addr_union *u,
                unsigned prefixlen_fixed_part,
                unsigned prefixlen) {

        أكد(u);

        /* Random network part of an address by one. */

        اگر (prefixlen <= 0)
                بازگشت 0;

        اگر (family == AF_INET) {
                uint32_t c, n;

                اگر (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                اگر (prefixlen > 32)
                        prefixlen = 32;
                اگر (prefixlen_fixed_part >= prefixlen)
                        بازگشت -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, اندازه(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                بازگشت 1;
        }

        اگر (family == AF_INET6) {
                ساختار in6_addr n;
                unsigned i, j;

                اگر (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                اگر (prefixlen > 128)
                        prefixlen = 128;
                اگر (prefixlen_fixed_part >= prefixlen)
                        بازگشت -EINVAL;

                random_bytes(&n, اندازه(n));

                برای (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        اگر (i < (prefixlen_fixed_part + 7) / 8) {
                                اگر (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                وگرنه {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        اگر (i < (prefixlen + 7) / 8) {
                                اگر (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                وگرنه {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                بازگشت 1;
        }

        بازگشت -EAFNOSUPPORT;
}

صحیح in_addr_prefix_range(
                صحیح family,
                ثابت اجتماع in_addr_union *in,
                unsigned prefixlen,
                اجتماع in_addr_union *ret_start,
                اجتماع in_addr_union *ret_end) {

        صحیح r;

        أكد(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        انتخاب (family) {
        حالت AF_INET:
                اگر (prefixlen > 32)
                        بازگشت -EINVAL;
                بشکن;
        حالت AF_INET6:
                اگر (prefixlen > 128)
                        بازگشت -EINVAL;
                بشکن;
        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }

        اگر (prefixlen == 0) {
                /* shortcut for trivial case */
                اگر (ret_start)
                        *ret_start = IN_ADDR_NULL;
                اگر (ret_end)
                        *ret_end = IN_ADDR_NULL;
                بازگشت 0;
        }

        اجتماع in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        اگر (r < 0)
                بازگشت r;

        اجتماع in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        برای (unsigned i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                اگر (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } وگرنه {
                        end.bytes[i] += add;
                        بشکن;
                }

        اگر (ret_start)
                *ret_start = start;
        اگر (ret_end)
                *ret_end = end;

        بازگشت 0;
}

صحیح in_addr_to_string(صحیح family, ثابت اجتماع in_addr_union *u, نویسه **ret) {
        _cleanup_free_ نویسه *x = پوچ;
        size_t l;

        أكد(u);
        أكد(ret);

        اگر (family == AF_INET)
                l = INET_ADDRSTRLEN;
        وگرنه اگر (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        وگرنه
                بازگشت -EAFNOSUPPORT;

        x = new(نویسه, l);
        اگر (!x)
                بازگشت -ENOMEM;

        errno = 0;
        اگر (!typesafe_inet_ntop(family, u, x, l))
                بازگشت errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        بازگشت 0;
}

ثابت نویسه* typesafe_inet_ntop(صحیح family, ثابت اجتماع in_addr_union *a, نویسه *buf, size_t len) {
        بازگشت inet_ntop(family, a, buf, len);
}

ثابت نویسه* typesafe_inet_ntop4(ثابت ساختار in_addr *a, نویسه *buf, size_t len) {
        بازگشت inet_ntop(AF_INET, a, buf, len);
}

ثابت نویسه* typesafe_inet_ntop6(ثابت ساختار in6_addr *a, نویسه *buf, size_t len) {
        بازگشت inet_ntop(AF_INET6, a, buf, len);
}

صحیح in_addr_prefix_to_string(
                صحیح family,
                ثابت اجتماع in_addr_union *u,
                unsigned prefixlen,
                نویسه *buf,
                size_t buf_len) {

        أكد(u);
        أكد(buf);

        اگر (!IN_SET(family, AF_INET, AF_INET6))
                بازگشت -EAFNOSUPPORT;

        errno = 0;
        اگر (!typesafe_inet_ntop(family, u, buf, buf_len))
                بازگشت errno_or_else(ENOSPC);

        size_t l = strlen(buf);
        اگر (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                بازگشت -ENOSPC;
        بازگشت 0;
}

صحیح in_addr_port_ifindex_name_to_string(صحیح family, ثابت اجتماع in_addr_union *u, uint16_t port, صحیح ifindex, ثابت نویسه *server_name, نویسه **ret) {
        _cleanup_free_ نویسه *ip_str = پوچ, *x = پوچ;
        صحیح r;

        أكد(IN_SET(family, AF_INET, AF_INET6));
        أكد(u);
        أكد(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        اگر (r < 0)
                بازگشت r;

        اگر (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        ifindex = 0;
        } وگرنه
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        اگر (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                بازگشت 0;
        }

        ثابت نویسه *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        اگر (port > 0) {
                اگر (family == AF_INET6) {
                        اگر (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        وگرنه
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } وگرنه
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } وگرنه {
                اگر (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                وگرنه
                        x = strjoin(ip_str, separator, server_name);
        }
        اگر (!x)
                بازگشت -ENOMEM;

        *ret = TAKE_PTR(x);
        بازگشت 0;
}

صحیح in_addr_from_string(صحیح family, ثابت نویسه *s, اجتماع in_addr_union *ret) {
        اجتماع in_addr_union buffer;
        أكد(s);

        اگر (!IN_SET(family, AF_INET, AF_INET6))
                بازگشت -EAFNOSUPPORT;

        errno = 0;
        اگر (inet_pton(family, s, ret ?: &buffer) <= 0)
                بازگشت errno_or_else(EINVAL);

        بازگشت 0;
}

صحیح in_addr_from_string_auto(ثابت نویسه *s, صحیح *ret_family, اجتماع in_addr_union *ret) {
        صحیح r;

        أكد(s);

        r = in_addr_from_string(AF_INET, s, ret);
        اگر (r >= 0) {
                اگر (ret_family)
                        *ret_family = AF_INET;
                بازگشت 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        اگر (r >= 0) {
                اگر (ret_family)
                        *ret_family = AF_INET6;
                بازگشت 0;
        }

        بازگشت -EINVAL;
}

unsigned نویسه in4_addr_netmask_to_prefixlen(ثابت ساختار in_addr *addr) {
        أكد(addr);

        بازگشت 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
ساختار in_addr* in4_addr_prefixlen_to_netmask(ساختار in_addr *addr, unsigned نویسه prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        اگر (prefixlen == 0)
                addr->s_addr = 0;
        وگرنه
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        بازگشت addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
ساختار in6_addr* in6_addr_prefixlen_to_netmask(ساختار in6_addr *addr, unsigned نویسه prefixlen) {
        أكد(addr);
        أكد(prefixlen <= 128);

        برای (unsigned i = 0; i < 16; i++) {
                uint8_t mask;

                اگر (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } وگرنه اگر (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } وگرنه {
                        أكد(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        بازگشت addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
صحیح in_addr_prefixlen_to_netmask(صحیح family, اجتماع in_addr_union *addr, unsigned نویسه prefixlen) {
        أكد(addr);

        انتخاب (family) {
        حالت AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                بازگشت 0;
        حالت AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                بازگشت 0;
        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

صحیح in4_addr_default_prefixlen(ثابت ساختار in_addr *addr, unsigned نویسه *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        أكد(addr);
        أكد(prefixlen);

        اگر (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        وگرنه اگر (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        وگرنه اگر (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        وگرنه
                /* class D or E, no default prefixlen */
                بازگشت -ERANGE;

        بازگشت 0;
}

صحیح in4_addr_default_subnet_mask(ثابت ساختار in_addr *addr, ساختار in_addr *mask) {
        unsigned نویسه prefixlen;
        صحیح r;

        أكد(addr);
        أكد(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        اگر (r < 0)
                بازگشت r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        بازگشت 0;
}

صحیح in4_addr_mask(ساختار in_addr *addr, unsigned نویسه prefixlen) {
        ساختار in_addr mask;

        أكد(addr);

        اگر (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                بازگشت -EINVAL;

        addr->s_addr &= mask.s_addr;
        بازگشت 0;
}

صحیح in6_addr_mask(ساختار in6_addr *addr, unsigned نویسه prefixlen) {
        unsigned i;

        أكد(addr);

        برای (i = 0; i < 16; i++) {
                uint8_t mask;

                اگر (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } وگرنه اگر (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } وگرنه {
                        أكد(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        بازگشت 0;
}

صحیح in_addr_mask(صحیح family, اجتماع in_addr_union *addr, unsigned نویسه prefixlen) {
        أكد(addr);

        انتخاب (family) {
        حالت AF_INET:
                بازگشت in4_addr_mask(&addr->in, prefixlen);
        حالت AF_INET6:
                بازگشت in6_addr_mask(&addr->in6, prefixlen);
        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

صحیح in4_addr_prefix_covers_full(
                ثابت ساختار in_addr *prefix,
                unsigned نویسه prefixlen,
                ثابت ساختار in_addr *address,
                unsigned نویسه address_prefixlen) {

        ساختار in_addr masked_prefix, masked_address;
        صحیح r;

        أكد(prefix);
        أكد(address);

        اگر (prefixlen > address_prefixlen)
                بازگشت false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        اگر (r < 0)
                بازگشت r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        اگر (r < 0)
                بازگشت r;

        بازگشت in4_addr_equal(&masked_prefix, &masked_address);
}

صحیح in6_addr_prefix_covers_full(
                ثابت ساختار in6_addr *prefix,
                unsigned نویسه prefixlen,
                ثابت ساختار in6_addr *address,
                unsigned نویسه address_prefixlen) {

        ساختار in6_addr masked_prefix, masked_address;
        صحیح r;

        أكد(prefix);
        أكد(address);

        اگر (prefixlen > address_prefixlen)
                بازگشت false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        اگر (r < 0)
                بازگشت r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        اگر (r < 0)
                بازگشت r;

        بازگشت in6_addr_equal(&masked_prefix, &masked_address);
}

صحیح in_addr_prefix_covers_full(
                صحیح family,
                ثابت اجتماع in_addr_union *prefix,
                unsigned نویسه prefixlen,
                ثابت اجتماع in_addr_union *address,
                unsigned نویسه address_prefixlen) {

        أكد(prefix);
        أكد(address);

        انتخاب (family) {
        حالت AF_INET:
                بازگشت in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        حالت AF_INET6:
                بازگشت in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

صحیح in_addr_parse_prefixlen(صحیح family, ثابت نویسه *p, unsigned نویسه *ret) {
        uint8_t u;
        صحیح r;

        أكد(ret);

        اگر (!IN_SET(family, AF_INET, AF_INET6))
                بازگشت -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        اگر (r < 0)
                بازگشت r;

        اگر (u > FAMILY_ADDRESS_SIZE(family) * 8)
                بازگشت -ERANGE;

        *ret = u;
        بازگشت 0;
}

صحیح in_addr_prefix_from_string(
                ثابت نویسه *p,
                صحیح family,
                اجتماع in_addr_union *ret_prefix,
                unsigned نویسه *ret_prefixlen) {

        _cleanup_free_ نویسه *str = پوچ;
        اجتماع in_addr_union buffer;
        ثابت نویسه *e, *l;
        unsigned نویسه k;
        صحیح r;

        أكد(p);

        اگر (!IN_SET(family, AF_INET, AF_INET6))
                بازگشت -EAFNOSUPPORT;

        e = strchr(p, '/');
        اگر (e) {
                str = strndup(p, e - p);
                اگر (!str)
                        بازگشت -ENOMEM;

                l = str;
        } وگرنه
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        اگر (r < 0)
                بازگشت r;

        اگر (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                اگر (r < 0)
                        بازگشت r;
        } وگرنه
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        اگر (ret_prefix)
                *ret_prefix = buffer;
        اگر (ret_prefixlen)
                *ret_prefixlen = k;

        بازگشت 0;
}

صحیح in_addr_prefix_from_string_auto_full(
                ثابت نویسه *p,
                InAddrPrefixLenMode mode,
                صحیح *ret_family,
                اجتماع in_addr_union *ret_prefix,
                unsigned نویسه *ret_prefixlen) {

        _cleanup_free_ نویسه *str = پوچ;
        اجتماع in_addr_union buffer;
        ثابت نویسه *e, *l;
        unsigned نویسه k;
        صحیح family, r;

        أكد(p);

        e = strchr(p, '/');
        اگر (e) {
                str = strndup(p, e - p);
                اگر (!str)
                        بازگشت -ENOMEM;

                l = str;
        } وگرنه
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        اگر (r < 0)
                بازگشت r;

        اگر (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                اگر (r < 0)
                        بازگشت r;
        } وگرنه
                انتخاب (mode) {
                حالت PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        بشکن;
                حالت PREFIXLEN_REFUSE:
                        بازگشت -ENOANO; /* To distinguish this error from others. */
                پیشفرض:
                        assert_not_reached();
                }

        اگر (ret_family)
                *ret_family = family;
        اگر (ret_prefix)
                *ret_prefix = buffer;
        اگر (ret_prefixlen)
                *ret_prefixlen = k;

        بازگشت 0;
}

تهی in_addr_hash_func(ثابت اجتماع in_addr_union *u, صحیح family, ساختار siphash *state) {
        أكد(u);
        أكد(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

تهی in_addr_data_hash_func(ثابت ساختار in_addr_data *a, ساختار siphash *state) {
        أكد(a);
        أكد(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

صحیح in_addr_data_compare_func(ثابت ساختار in_addr_data *x, ثابت ساختار in_addr_data *y) {
        صحیح r;

        أكد(x);
        أكد(y);

        r = CMP(x->family, y->family);
        اگر (r != 0)
                بازگشت r;

        بازگشت memcmp(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        ساختار in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        ساختار in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        آزاد);

تهی in6_addr_hash_func(ثابت ساختار in6_addr *addr, ساختار siphash *state) {
        أكد(addr);
        أكد(state);

        siphash24_compress_typesafe(*addr, state);
}

صحیح in6_addr_compare_func(ثابت ساختار in6_addr *a, ثابت ساختار in6_addr *b) {
        أكد(a);
        أكد(b);

        بازگشت memcmp(a, b, اندازه(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        ساختار in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
        in6_addr_hash_ops_free,
        ساختار in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        آزاد);
