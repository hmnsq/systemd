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

bool in4_addr_is_null(स्थिरांक संरचना in_addr *a) {
        प्रतिज्ञा(a);

        परत a->s_addr == 0;
}

bool in6_addr_is_null(स्थिरांक संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        परत eqzero(a->s6_addr32);
}

पूर्णांक in_addr_is_null(पूर्णांक family, स्थिरांक संघ in_addr_union *u) {
        प्रतिज्ञा(u);

        जर (family == AF_INET)
                परत in4_addr_is_null(&u->in);

        जर (family == AF_INET6)
                परत in6_addr_is_null(&u->in6);

        परत -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(स्थिरांक संरचना in_addr *a) {
        प्रतिज्ञा(a);

        परत (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(स्थिरांक संरचना in_addr *a) {
        प्रतिज्ञा(a);

        जर (!in4_addr_is_link_local(a))
                परत false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        परत !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(स्थिरांक संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        परत (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

पूर्णांक in_addr_is_link_local(पूर्णांक family, स्थिरांक संघ in_addr_union *u) {
        प्रतिज्ञा(u);

        जर (family == AF_INET)
                परत in4_addr_is_link_local(&u->in);

        जर (family == AF_INET6)
                परत in6_addr_is_link_local(&u->in6);

        परत -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(स्थिरांक संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        /* ff02::1 */
        परत be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(स्थिरांक संरचना in_addr *a) {
        प्रतिज्ञा(a);

        परत IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(स्थिरांक संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        परत a->s6_addr[0] == 0xff;
}

पूर्णांक in_addr_is_multicast(पूर्णांक family, स्थिरांक संघ in_addr_union *u) {
        प्रतिज्ञा(u);

        जर (family == AF_INET)
                परत in4_addr_is_multicast(&u->in);

        जर (family == AF_INET6)
                परत in6_addr_is_multicast(&u->in6);

        परत -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(स्थिरांक संरचना in_addr *a) {
        प्रतिज्ञा(a);

        परत (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(स्थिरांक संरचना in_addr *a) {
        प्रतिज्ञा(a);

        /* All of 127.x.x.x is localhost. */
        परत (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(स्थिरांक संरचना in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        परत !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

स्थिर bool in6_addr_is_loopback(स्थिरांक संरचना in6_addr *a) {
        परत स_भेद(a, &(संरचना in6_addr) IN6ADDR_LOOPBACK_INIT, आकार(संरचना in6_addr)) == 0;
}

पूर्णांक in_addr_is_localhost(पूर्णांक family, स्थिरांक संघ in_addr_union *u) {
        प्रतिज्ञा(u);

        जर (family == AF_INET)
                परत in4_addr_is_localhost(&u->in);

        जर (family == AF_INET6)
                परत in6_addr_is_loopback(&u->in6);

        परत -EAFNOSUPPORT;
}

पूर्णांक in_addr_is_localhost_one(पूर्णांक family, स्थिरांक संघ in_addr_union *u) {
        प्रतिज्ञा(u);

        जर (family == AF_INET)
                /* 127.0.0.1 */
                परत be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        जर (family == AF_INET6)
                परत in6_addr_is_loopback(&u->in6);

        परत -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(स्थिरांक संरचना in6_addr *a) {
        प्रतिज्ञा(a);

        परत a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(स्थिरांक संरचना in_addr *a, स्थिरांक संरचना in_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        परत a->s_addr == b->s_addr;
}

bool in6_addr_equal(स्थिरांक संरचना in6_addr *a, स्थिरांक संरचना in6_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        परत स_भेद(a, b, आकार(संरचना in6_addr)) == 0;
}

पूर्णांक in_addr_equal(पूर्णांक family, स्थिरांक संघ in_addr_union *a, स्थिरांक संघ in_addr_union *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        जर (family == AF_INET)
                परत in4_addr_equal(&a->in, &b->in);

        जर (family == AF_INET6)
                परत in6_addr_equal(&a->in6, &b->in6);

        परत -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                स्थिरांक संरचना in_addr *a,
                अचिन्हित aprefixlen,
                स्थिरांक संरचना in_addr *b,
                अचिन्हित bprefixlen) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        अचिन्हित m = MIN3(aprefixlen, bprefixlen, (अचिन्हित) (आकार(संरचना in_addr) * 8));
        जर (m == 0)
                परत true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        परत (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                स्थिरांक संरचना in6_addr *a,
                अचिन्हित aprefixlen,
                स्थिरांक संरचना in6_addr *b,
                अचिन्हित bprefixlen) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        अचिन्हित m = MIN3(aprefixlen, bprefixlen, (अचिन्हित) (आकार(संरचना in6_addr) * 8));
        जर (m == 0)
                परत true;

        साठी (माप_प्रकार i = 0; i < आकार(संरचना in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                जर ((x & n) != 0)
                        परत false;

                जर (m <= 8)
                        थांबा;

                m -= 8;
        }

        परत true;
}

पूर्णांक in_addr_prefix_intersect(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *a,
                अचिन्हित aprefixlen,
                स्थिरांक संघ in_addr_union *b,
                अचिन्हित bprefixlen) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        /* Checks whether there are any addresses that are in both networks. */

        जर (family == AF_INET)
                परत in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        जर (family == AF_INET6)
                परत in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        परत -EAFNOSUPPORT;
}

पूर्णांक in_addr_random_prefix(
                पूर्णांक family,
                संघ in_addr_union *u,
                अचिन्हित prefixlen_fixed_part,
                अचिन्हित prefixlen) {

        प्रतिज्ञा(u);

        /* Random network part of an address by one. */

        जर (prefixlen <= 0)
                परत 0;

        जर (family == AF_INET) {
                uint32_t c, n;

                जर (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                जर (prefixlen > 32)
                        prefixlen = 32;
                जर (prefixlen_fixed_part >= prefixlen)
                        परत -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, आकार(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                परत 1;
        }

        जर (family == AF_INET6) {
                संरचना in6_addr n;
                अचिन्हित i, j;

                जर (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                जर (prefixlen > 128)
                        prefixlen = 128;
                जर (prefixlen_fixed_part >= prefixlen)
                        परत -EINVAL;

                random_bytes(&n, आकार(n));

                साठी (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        जर (i < (prefixlen_fixed_part + 7) / 8) {
                                जर (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                अन्यथा {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        जर (i < (prefixlen + 7) / 8) {
                                जर (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                अन्यथा {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                परत 1;
        }

        परत -EAFNOSUPPORT;
}

पूर्णांक in_addr_prefix_range(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *in,
                अचिन्हित prefixlen,
                संघ in_addr_union *ret_start,
                संघ in_addr_union *ret_end) {

        पूर्णांक r;

        प्रतिज्ञा(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        निवड (family) {
        स्थिती AF_INET:
                जर (prefixlen > 32)
                        परत -EINVAL;
                थांबा;
        स्थिती AF_INET6:
                जर (prefixlen > 128)
                        परत -EINVAL;
                थांबा;
        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }

        जर (prefixlen == 0) {
                /* shortcut for trivial case */
                जर (ret_start)
                        *ret_start = IN_ADDR_NULL;
                जर (ret_end)
                        *ret_end = IN_ADDR_NULL;
                परत 0;
        }

        संघ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        जर (r < 0)
                परत r;

        संघ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_SIZE(family) * 8 - prefixlen) % 8);
        साठी (अचिन्हित i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                जर (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } अन्यथा {
                        end.bytes[i] += add;
                        थांबा;
                }

        जर (ret_start)
                *ret_start = start;
        जर (ret_end)
                *ret_end = end;

        परत 0;
}

पूर्णांक in_addr_to_string(पूर्णांक family, स्थिरांक संघ in_addr_union *u, अक्षर **ret) {
        _cleanup_free_ अक्षर *x = रिक्त;
        माप_प्रकार l;

        प्रतिज्ञा(u);
        प्रतिज्ञा(ret);

        जर (family == AF_INET)
                l = INET_ADDRSTRLEN;
        अन्यथा जर (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        अन्यथा
                परत -EAFNOSUPPORT;

        x = new(अक्षर, l);
        जर (!x)
                परत -ENOMEM;

        त्रुटि_सं = 0;
        जर (!typesafe_inet_ntop(family, u, x, l))
                परत errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        परत 0;
}

स्थिरांक अक्षर* typesafe_inet_ntop(पूर्णांक family, स्थिरांक संघ in_addr_union *a, अक्षर *buf, माप_प्रकार len) {
        परत inet_ntop(family, a, buf, len);
}

स्थिरांक अक्षर* typesafe_inet_ntop4(स्थिरांक संरचना in_addr *a, अक्षर *buf, माप_प्रकार len) {
        परत inet_ntop(AF_INET, a, buf, len);
}

स्थिरांक अक्षर* typesafe_inet_ntop6(स्थिरांक संरचना in6_addr *a, अक्षर *buf, माप_प्रकार len) {
        परत inet_ntop(AF_INET6, a, buf, len);
}

पूर्णांक in_addr_prefix_to_string(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *u,
                अचिन्हित prefixlen,
                अक्षर *buf,
                माप_प्रकार buf_len) {

        प्रतिज्ञा(u);
        प्रतिज्ञा(buf);

        जर (!IN_SET(family, AF_INET, AF_INET6))
                परत -EAFNOSUPPORT;

        त्रुटि_सं = 0;
        जर (!typesafe_inet_ntop(family, u, buf, buf_len))
                परत errno_or_else(ENOSPC);

        माप_प्रकार l = म_माप(buf);
        जर (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                परत -ENOSPC;
        परत 0;
}

पूर्णांक in_addr_port_ifindex_name_to_string(पूर्णांक family, स्थिरांक संघ in_addr_union *u, uint16_t port, पूर्णांक ifindex, स्थिरांक अक्षर *server_name, अक्षर **ret) {
        _cleanup_free_ अक्षर *ip_str = रिक्त, *x = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(IN_SET(family, AF_INET, AF_INET6));
        प्रतिज्ञा(u);
        प्रतिज्ञा(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        जर (r < 0)
                परत r;

        जर (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        ifindex = 0;
        } अन्यथा
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        जर (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                परत 0;
        }

        स्थिरांक अक्षर *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        जर (port > 0) {
                जर (family == AF_INET6) {
                        जर (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        अन्यथा
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } अन्यथा
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } अन्यथा {
                जर (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                अन्यथा
                        x = strjoin(ip_str, separator, server_name);
        }
        जर (!x)
                परत -ENOMEM;

        *ret = TAKE_PTR(x);
        परत 0;
}

पूर्णांक in_addr_from_string(पूर्णांक family, स्थिरांक अक्षर *s, संघ in_addr_union *ret) {
        संघ in_addr_union buffer;
        प्रतिज्ञा(s);

        जर (!IN_SET(family, AF_INET, AF_INET6))
                परत -EAFNOSUPPORT;

        त्रुटि_सं = 0;
        जर (inet_pton(family, s, ret ?: &buffer) <= 0)
                परत errno_or_else(EINVAL);

        परत 0;
}

पूर्णांक in_addr_from_string_auto(स्थिरांक अक्षर *s, पूर्णांक *ret_family, संघ in_addr_union *ret) {
        पूर्णांक r;

        प्रतिज्ञा(s);

        r = in_addr_from_string(AF_INET, s, ret);
        जर (r >= 0) {
                जर (ret_family)
                        *ret_family = AF_INET;
                परत 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        जर (r >= 0) {
                जर (ret_family)
                        *ret_family = AF_INET6;
                परत 0;
        }

        परत -EINVAL;
}

अचिन्हित अक्षर in4_addr_netmask_to_prefixlen(स्थिरांक संरचना in_addr *addr) {
        प्रतिज्ञा(addr);

        परत 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
संरचना in_addr* in4_addr_prefixlen_to_netmask(संरचना in_addr *addr, अचिन्हित अक्षर prefixlen) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        जर (prefixlen == 0)
                addr->s_addr = 0;
        अन्यथा
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        परत addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
संरचना in6_addr* in6_addr_prefixlen_to_netmask(संरचना in6_addr *addr, अचिन्हित अक्षर prefixlen) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(prefixlen <= 128);

        साठी (अचिन्हित i = 0; i < 16; i++) {
                uint8_t mask;

                जर (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } अन्यथा जर (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } अन्यथा {
                        प्रतिज्ञा(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        परत addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
पूर्णांक in_addr_prefixlen_to_netmask(पूर्णांक family, संघ in_addr_union *addr, अचिन्हित अक्षर prefixlen) {
        प्रतिज्ञा(addr);

        निवड (family) {
        स्थिती AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                परत 0;
        स्थिती AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                परत 0;
        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

पूर्णांक in4_addr_default_prefixlen(स्थिरांक संरचना in_addr *addr, अचिन्हित अक्षर *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        प्रतिज्ञा(addr);
        प्रतिज्ञा(prefixlen);

        जर (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        अन्यथा जर (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        अन्यथा जर (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        अन्यथा
                /* class D or E, no default prefixlen */
                परत -दुस्फल;

        परत 0;
}

पूर्णांक in4_addr_default_subnet_mask(स्थिरांक संरचना in_addr *addr, संरचना in_addr *mask) {
        अचिन्हित अक्षर prefixlen;
        पूर्णांक r;

        प्रतिज्ञा(addr);
        प्रतिज्ञा(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        जर (r < 0)
                परत r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        परत 0;
}

पूर्णांक in4_addr_mask(संरचना in_addr *addr, अचिन्हित अक्षर prefixlen) {
        संरचना in_addr mask;

        प्रतिज्ञा(addr);

        जर (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                परत -EINVAL;

        addr->s_addr &= mask.s_addr;
        परत 0;
}

पूर्णांक in6_addr_mask(संरचना in6_addr *addr, अचिन्हित अक्षर prefixlen) {
        अचिन्हित i;

        प्रतिज्ञा(addr);

        साठी (i = 0; i < 16; i++) {
                uint8_t mask;

                जर (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } अन्यथा जर (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } अन्यथा {
                        प्रतिज्ञा(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        परत 0;
}

पूर्णांक in_addr_mask(पूर्णांक family, संघ in_addr_union *addr, अचिन्हित अक्षर prefixlen) {
        प्रतिज्ञा(addr);

        निवड (family) {
        स्थिती AF_INET:
                परत in4_addr_mask(&addr->in, prefixlen);
        स्थिती AF_INET6:
                परत in6_addr_mask(&addr->in6, prefixlen);
        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

पूर्णांक in4_addr_prefix_covers_full(
                स्थिरांक संरचना in_addr *prefix,
                अचिन्हित अक्षर prefixlen,
                स्थिरांक संरचना in_addr *address,
                अचिन्हित अक्षर address_prefixlen) {

        संरचना in_addr masked_prefix, masked_address;
        पूर्णांक r;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(address);

        जर (prefixlen > address_prefixlen)
                परत false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        जर (r < 0)
                परत r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        जर (r < 0)
                परत r;

        परत in4_addr_equal(&masked_prefix, &masked_address);
}

पूर्णांक in6_addr_prefix_covers_full(
                स्थिरांक संरचना in6_addr *prefix,
                अचिन्हित अक्षर prefixlen,
                स्थिरांक संरचना in6_addr *address,
                अचिन्हित अक्षर address_prefixlen) {

        संरचना in6_addr masked_prefix, masked_address;
        पूर्णांक r;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(address);

        जर (prefixlen > address_prefixlen)
                परत false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        जर (r < 0)
                परत r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        जर (r < 0)
                परत r;

        परत in6_addr_equal(&masked_prefix, &masked_address);
}

पूर्णांक in_addr_prefix_covers_full(
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *prefix,
                अचिन्हित अक्षर prefixlen,
                स्थिरांक संघ in_addr_union *address,
                अचिन्हित अक्षर address_prefixlen) {

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(address);

        निवड (family) {
        स्थिती AF_INET:
                परत in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        स्थिती AF_INET6:
                परत in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

पूर्णांक in_addr_parse_prefixlen(पूर्णांक family, स्थिरांक अक्षर *p, अचिन्हित अक्षर *ret) {
        uint8_t u;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        जर (!IN_SET(family, AF_INET, AF_INET6))
                परत -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        जर (r < 0)
                परत r;

        जर (u > FAMILY_ADDRESS_SIZE(family) * 8)
                परत -दुस्फल;

        *ret = u;
        परत 0;
}

पूर्णांक in_addr_prefix_from_string(
                स्थिरांक अक्षर *p,
                पूर्णांक family,
                संघ in_addr_union *ret_prefix,
                अचिन्हित अक्षर *ret_prefixlen) {

        _cleanup_free_ अक्षर *str = रिक्त;
        संघ in_addr_union buffer;
        स्थिरांक अक्षर *e, *l;
        अचिन्हित अक्षर k;
        पूर्णांक r;

        प्रतिज्ञा(p);

        जर (!IN_SET(family, AF_INET, AF_INET6))
                परत -EAFNOSUPPORT;

        e = म_अक्षर(p, '/');
        जर (e) {
                str = strndup(p, e - p);
                जर (!str)
                        परत -ENOMEM;

                l = str;
        } अन्यथा
                l = p;

        r = in_addr_from_string(family, l, &buffer);
        जर (r < 0)
                परत r;

        जर (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                जर (r < 0)
                        परत r;
        } अन्यथा
                k = FAMILY_ADDRESS_SIZE(family) * 8;

        जर (ret_prefix)
                *ret_prefix = buffer;
        जर (ret_prefixlen)
                *ret_prefixlen = k;

        परत 0;
}

पूर्णांक in_addr_prefix_from_string_auto_full(
                स्थिरांक अक्षर *p,
                InAddrPrefixLenMode mode,
                पूर्णांक *ret_family,
                संघ in_addr_union *ret_prefix,
                अचिन्हित अक्षर *ret_prefixlen) {

        _cleanup_free_ अक्षर *str = रिक्त;
        संघ in_addr_union buffer;
        स्थिरांक अक्षर *e, *l;
        अचिन्हित अक्षर k;
        पूर्णांक family, r;

        प्रतिज्ञा(p);

        e = म_अक्षर(p, '/');
        जर (e) {
                str = strndup(p, e - p);
                जर (!str)
                        परत -ENOMEM;

                l = str;
        } अन्यथा
                l = p;

        r = in_addr_from_string_auto(l, &family, &buffer);
        जर (r < 0)
                परत r;

        जर (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                जर (r < 0)
                        परत r;
        } अन्यथा
                निवड (mode) {
                स्थिती PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_SIZE(family) * 8;
                        थांबा;
                स्थिती PREFIXLEN_REFUSE:
                        परत -ENOANO; /* To distinguish this error from others. */
                पूर्वनिर्धारित:
                        assert_not_reached();
                }

        जर (ret_family)
                *ret_family = family;
        जर (ret_prefix)
                *ret_prefix = buffer;
        जर (ret_prefixlen)
                *ret_prefixlen = k;

        परत 0;
}

शून्य in_addr_hash_func(स्थिरांक संघ in_addr_union *u, पूर्णांक family, संरचना siphash *state) {
        प्रतिज्ञा(u);
        प्रतिज्ञा(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_SIZE(family), state);
}

शून्य in_addr_data_hash_func(स्थिरांक संरचना in_addr_data *a, संरचना siphash *state) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

पूर्णांक in_addr_data_compare_func(स्थिरांक संरचना in_addr_data *x, स्थिरांक संरचना in_addr_data *y) {
        पूर्णांक r;

        प्रतिज्ञा(x);
        प्रतिज्ञा(y);

        r = CMP(x->family, y->family);
        जर (r != 0)
                परत r;

        परत स_भेद(&x->address, &y->address, FAMILY_ADDRESS_SIZE(x->family));
}

DEFINE_HASH_OPS(
        in_addr_data_hash_ops,
        संरचना in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func);

DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(
        in_addr_data_hash_ops_free,
        संरचना in_addr_data,
        in_addr_data_hash_func,
        in_addr_data_compare_func,
        मुक्त);

शून्य in6_addr_hash_func(स्थिरांक संरचना in6_addr *addr, संरचना siphash *state) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(state);

        siphash24_compress_typesafe(*addr, state);
}

पूर्णांक in6_addr_compare_func(स्थिरांक संरचना in6_addr *a, स्थिरांक संरचना in6_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        परत स_भेद(a, b, आकार(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        संरचना in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(
        in6_addr_hash_ops_free,
        संरचना in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        मुक्त);
