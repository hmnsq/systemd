/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <arpa/inet.h>
#समावेश <endian.h>
#समावेश <stdio.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "hash-funcs.h"
#समावेश "in-addr-util.h"
#समावेश "logarithm.h"
#समावेश "memory-util.h"
#समावेश "parse-util.h"
#समावेश "random-util.h"
#समावेश "siphash24.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"

bool in4_addr_is_null(स्थिर काष्ठा in_addr *a) {
        निश्चित(a);

        वापस a->s_addr == 0;
}

bool in6_addr_is_null(स्थिर काष्ठा in6_addr *a) {
        निश्चित(a);

        वापस eqzero(a->s6_addr32);
}

पूर्णांक in_addr_is_null(पूर्णांक family, स्थिर जोड़ in_addr_union *u) {
        निश्चित(u);

        अगर (family == AF_INET)
                वापस in4_addr_is_null(&u->in);

        अगर (family == AF_INET6)
                वापस in6_addr_is_null(&u->in6);

        वापस -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(स्थिर काष्ठा in_addr *a) {
        निश्चित(a);

        वापस (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(स्थिर काष्ठा in_addr *a) {
        निश्चित(a);

        अगर (!in4_addr_is_link_local(a))
                वापस false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        वापस !IN_SET(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(स्थिर काष्ठा in6_addr *a) {
        निश्चित(a);

        वापस (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

पूर्णांक in_addr_is_link_local(पूर्णांक family, स्थिर जोड़ in_addr_union *u) {
        निश्चित(u);

        अगर (family == AF_INET)
                वापस in4_addr_is_link_local(&u->in);

        अगर (family == AF_INET6)
                वापस in6_addr_is_link_local(&u->in6);

        वापस -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(स्थिर काष्ठा in6_addr *a) {
        निश्चित(a);

        /* ff02::1 */
        वापस be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(स्थिर काष्ठा in_addr *a) {
        निश्चित(a);

        वापस IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(स्थिर काष्ठा in6_addr *a) {
        निश्चित(a);

        वापस a->s6_addr[0] == 0xff;
}

पूर्णांक in_addr_is_multicast(पूर्णांक family, स्थिर जोड़ in_addr_union *u) {
        निश्चित(u);

        अगर (family == AF_INET)
                वापस in4_addr_is_multicast(&u->in);

        अगर (family == AF_INET6)
                वापस in6_addr_is_multicast(&u->in6);

        वापस -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(स्थिर काष्ठा in_addr *a) {
        निश्चित(a);

        वापस (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(स्थिर काष्ठा in_addr *a) {
        निश्चित(a);

        /* All of 127.x.x.x is localhost. */
        वापस (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(स्थिर काष्ठा in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        वापस !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

अटल bool in6_addr_is_loopback(स्थिर काष्ठा in6_addr *a) {
        वापस स_भेद(a, &(काष्ठा in6_addr) IN6ADDR_LOOPBACK_INIT, माप(काष्ठा in6_addr)) == 0;
}

पूर्णांक in_addr_is_localhost(पूर्णांक family, स्थिर जोड़ in_addr_union *u) {
        निश्चित(u);

        अगर (family == AF_INET)
                वापस in4_addr_is_localhost(&u->in);

        अगर (family == AF_INET6)
                वापस in6_addr_is_loopback(&u->in6);

        वापस -EAFNOSUPPORT;
}

पूर्णांक in_addr_is_localhost_one(पूर्णांक family, स्थिर जोड़ in_addr_union *u) {
        निश्चित(u);

        अगर (family == AF_INET)
                /* 127.0.0.1 */
                वापस be32toh(u->in.s_addr) == UINT32_C(0x7F000001);

        अगर (family == AF_INET6)
                वापस in6_addr_is_loopback(&u->in6);

        वापस -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_address(स्थिर काष्ठा in6_addr *a) {
        निश्चित(a);

        वापस a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(स्थिर काष्ठा in_addr *a, स्थिर काष्ठा in_addr *b) {
        निश्चित(a);
        निश्चित(b);

        वापस a->s_addr == b->s_addr;
}

bool in6_addr_equal(स्थिर काष्ठा in6_addr *a, स्थिर काष्ठा in6_addr *b) {
        निश्चित(a);
        निश्चित(b);

        वापस स_भेद(a, b, माप(काष्ठा in6_addr)) == 0;
}

पूर्णांक in_addr_equal(पूर्णांक family, स्थिर जोड़ in_addr_union *a, स्थिर जोड़ in_addr_union *b) {
        निश्चित(a);
        निश्चित(b);

        अगर (family == AF_INET)
                वापस in4_addr_equal(&a->in, &b->in);

        अगर (family == AF_INET6)
                वापस in6_addr_equal(&a->in6, &b->in6);

        वापस -EAFNOSUPPORT;
}

bool in4_addr_prefix_intersect(
                स्थिर काष्ठा in_addr *a,
                अचिन्हित aprefixlen,
                स्थिर काष्ठा in_addr *b,
                अचिन्हित bprefixlen) {

        निश्चित(a);
        निश्चित(b);

        अचिन्हित m = MIN3(aprefixlen, bprefixlen, (अचिन्हित) (माप(काष्ठा in_addr) * 8));
        अगर (m == 0)
                वापस true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        वापस (x & n) == 0;
}

bool in6_addr_prefix_intersect(
                स्थिर काष्ठा in6_addr *a,
                अचिन्हित aprefixlen,
                स्थिर काष्ठा in6_addr *b,
                अचिन्हित bprefixlen) {

        निश्चित(a);
        निश्चित(b);

        अचिन्हित m = MIN3(aprefixlen, bprefixlen, (अचिन्हित) (माप(काष्ठा in6_addr) * 8));
        अगर (m == 0)
                वापस true;

        क्रम (माप_प्रकार i = 0; i < माप(काष्ठा in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                अगर ((x & n) != 0)
                        वापस false;

                अगर (m <= 8)
                        अवरोध;

                m -= 8;
        }

        वापस true;
}

पूर्णांक in_addr_prefix_intersect(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *a,
                अचिन्हित aprefixlen,
                स्थिर जोड़ in_addr_union *b,
                अचिन्हित bprefixlen) {

        निश्चित(a);
        निश्चित(b);

        /* Checks whether there are any addresses that are in both networks. */

        अगर (family == AF_INET)
                वापस in4_addr_prefix_intersect(&a->in, aprefixlen, &b->in, bprefixlen);

        अगर (family == AF_INET6)
                वापस in6_addr_prefix_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        वापस -EAFNOSUPPORT;
}

पूर्णांक in_addr_random_prefix(
                पूर्णांक family,
                जोड़ in_addr_union *u,
                अचिन्हित prefixlen_fixed_part,
                अचिन्हित prefixlen) {

        निश्चित(u);

        /* Random network part of an address by one. */

        अगर (prefixlen <= 0)
                वापस 0;

        अगर (family == AF_INET) {
                uint32_t c, n;

                अगर (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                अगर (prefixlen > 32)
                        prefixlen = 32;
                अगर (prefixlen_fixed_part >= prefixlen)
                        वापस -EINVAL;

                c = be32toh(u->in.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_bytes(&n, माप(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->in.s_addr = htobe32(n | c);
                वापस 1;
        }

        अगर (family == AF_INET6) {
                काष्ठा in6_addr n;
                अचिन्हित i, j;

                अगर (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                अगर (prefixlen > 128)
                        prefixlen = 128;
                अगर (prefixlen_fixed_part >= prefixlen)
                        वापस -EINVAL;

                random_bytes(&n, माप(n));

                क्रम (i = 0; i < 16; i++) {
                        uint8_t mask_fixed_part = 0, mask = 0;

                        अगर (i < (prefixlen_fixed_part + 7) / 8) {
                                अगर (i < prefixlen_fixed_part / 8)
                                        mask_fixed_part = 0xffu;
                                अन्यथा {
                                        j = prefixlen_fixed_part % 8;
                                        mask_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        अगर (i < (prefixlen + 7) / 8) {
                                अगर (i < prefixlen / 8)
                                        mask = 0xffu ^ mask_fixed_part;
                                अन्यथा {
                                        j = prefixlen % 8;
                                        mask = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ mask_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= mask_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & mask;
                }

                वापस 1;
        }

        वापस -EAFNOSUPPORT;
}

पूर्णांक in_addr_prefix_range(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *in,
                अचिन्हित prefixlen,
                जोड़ in_addr_union *ret_start,
                जोड़ in_addr_union *ret_end) {

        पूर्णांक r;

        निश्चित(in);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        चयन (family) {
        हाल AF_INET:
                अगर (prefixlen > 32)
                        वापस -EINVAL;
                अवरोध;
        हाल AF_INET6:
                अगर (prefixlen > 128)
                        वापस -EINVAL;
                अवरोध;
        शेष:
                वापस -EAFNOSUPPORT;
        }

        अगर (prefixlen == 0) {
                /* shortcut for trivial case */
                अगर (ret_start)
                        *ret_start = IN_ADDR_NULL;
                अगर (ret_end)
                        *ret_end = IN_ADDR_NULL;
                वापस 0;
        }

        जोड़ in_addr_union start = *in;
        r = in_addr_mask(family, &start, prefixlen);
        अगर (r < 0)
                वापस r;

        जोड़ in_addr_union end = start;
        uint8_t add = UINT8_C(1) << ((FAMILY_ADDRESS_आकार(family) * 8 - prefixlen) % 8);
        क्रम (अचिन्हित i = DIV_ROUND_UP(prefixlen, 8); i-- > 0;)
                अगर (add > UINT8_MAX - end.bytes[i]) {
                        end.bytes[i] = 0;
                        add = 1;
                } अन्यथा {
                        end.bytes[i] += add;
                        अवरोध;
                }

        अगर (ret_start)
                *ret_start = start;
        अगर (ret_end)
                *ret_end = end;

        वापस 0;
}

पूर्णांक in_addr_to_string(पूर्णांक family, स्थिर जोड़ in_addr_union *u, अक्षर **ret) {
        _cleanup_free_ अक्षर *x = शून्य;
        माप_प्रकार l;

        निश्चित(u);
        निश्चित(ret);

        अगर (family == AF_INET)
                l = INET_ADDRSTRLEN;
        अन्यथा अगर (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        अन्यथा
                वापस -EAFNOSUPPORT;

        x = new(अक्षर, l);
        अगर (!x)
                वापस -ENOMEM;

        त्रुटि_सं = 0;
        अगर (!typesafe_inet_ntop(family, u, x, l))
                वापस errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        वापस 0;
}

स्थिर अक्षर* typesafe_inet_ntop(पूर्णांक family, स्थिर जोड़ in_addr_union *a, अक्षर *buf, माप_प्रकार len) {
        वापस inet_ntop(family, a, buf, len);
}

स्थिर अक्षर* typesafe_inet_ntop4(स्थिर काष्ठा in_addr *a, अक्षर *buf, माप_प्रकार len) {
        वापस inet_ntop(AF_INET, a, buf, len);
}

स्थिर अक्षर* typesafe_inet_ntop6(स्थिर काष्ठा in6_addr *a, अक्षर *buf, माप_प्रकार len) {
        वापस inet_ntop(AF_INET6, a, buf, len);
}

पूर्णांक in_addr_prefix_to_string(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *u,
                अचिन्हित prefixlen,
                अक्षर *buf,
                माप_प्रकार buf_len) {

        निश्चित(u);
        निश्चित(buf);

        अगर (!IN_SET(family, AF_INET, AF_INET6))
                वापस -EAFNOSUPPORT;

        त्रुटि_सं = 0;
        अगर (!typesafe_inet_ntop(family, u, buf, buf_len))
                वापस errno_or_else(ENOSPC);

        माप_प्रकार l = म_माप(buf);
        अगर (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                वापस -ENOSPC;
        वापस 0;
}

पूर्णांक in_addr_port_ifindex_name_to_string(पूर्णांक family, स्थिर जोड़ in_addr_union *u, uint16_t port, पूर्णांक ifindex, स्थिर अक्षर *server_name, अक्षर **ret) {
        _cleanup_free_ अक्षर *ip_str = शून्य, *x = शून्य;
        पूर्णांक r;

        निश्चित(IN_SET(family, AF_INET, AF_INET6));
        निश्चित(u);
        निश्चित(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = in_addr_to_string(family, u, &ip_str);
        अगर (r < 0)
                वापस r;

        अगर (family == AF_INET6) {
                r = in_addr_is_link_local(family, u);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        ifindex = 0;
        } अन्यथा
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        अगर (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                वापस 0;
        }

        स्थिर अक्षर *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        अगर (port > 0) {
                अगर (family == AF_INET6) {
                        अगर (ifindex > 0)
                                x = asprintf_safe("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        अन्यथा
                                x = asprintf_safe("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } अन्यथा
                        x = asprintf_safe("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } अन्यथा {
                अगर (ifindex > 0)
                        x = asprintf_safe("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                अन्यथा
                        x = strjoin(ip_str, separator, server_name);
        }
        अगर (!x)
                वापस -ENOMEM;

        *ret = TAKE_PTR(x);
        वापस 0;
}

पूर्णांक in_addr_from_string(पूर्णांक family, स्थिर अक्षर *s, जोड़ in_addr_union *ret) {
        जोड़ in_addr_union बफर;
        निश्चित(s);

        अगर (!IN_SET(family, AF_INET, AF_INET6))
                वापस -EAFNOSUPPORT;

        त्रुटि_सं = 0;
        अगर (inet_pton(family, s, ret ?: &बफर) <= 0)
                वापस errno_or_else(EINVAL);

        वापस 0;
}

पूर्णांक in_addr_from_string_auto(स्थिर अक्षर *s, पूर्णांक *ret_family, जोड़ in_addr_union *ret) {
        पूर्णांक r;

        निश्चित(s);

        r = in_addr_from_string(AF_INET, s, ret);
        अगर (r >= 0) {
                अगर (ret_family)
                        *ret_family = AF_INET;
                वापस 0;
        }

        r = in_addr_from_string(AF_INET6, s, ret);
        अगर (r >= 0) {
                अगर (ret_family)
                        *ret_family = AF_INET6;
                वापस 0;
        }

        वापस -EINVAL;
}

अचिन्हित अक्षर in4_addr_netmask_to_prefixlen(स्थिर काष्ठा in_addr *addr) {
        निश्चित(addr);

        वापस 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
काष्ठा in_addr* in4_addr_prefixlen_to_netmask(काष्ठा in_addr *addr, अचिन्हित अक्षर prefixlen) {
        निश्चित(addr);
        निश्चित(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        अगर (prefixlen == 0)
                addr->s_addr = 0;
        अन्यथा
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        वापस addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
काष्ठा in6_addr* in6_addr_prefixlen_to_netmask(काष्ठा in6_addr *addr, अचिन्हित अक्षर prefixlen) {
        निश्चित(addr);
        निश्चित(prefixlen <= 128);

        क्रम (अचिन्हित i = 0; i < 16; i++) {
                uint8_t mask;

                अगर (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } अन्यथा अगर (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } अन्यथा {
                        निश्चित(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] = mask;
        }

        वापस addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
पूर्णांक in_addr_prefixlen_to_netmask(पूर्णांक family, जोड़ in_addr_union *addr, अचिन्हित अक्षर prefixlen) {
        निश्चित(addr);

        चयन (family) {
        हाल AF_INET:
                in4_addr_prefixlen_to_netmask(&addr->in, prefixlen);
                वापस 0;
        हाल AF_INET6:
                in6_addr_prefixlen_to_netmask(&addr->in6, prefixlen);
                वापस 0;
        शेष:
                वापस -EAFNOSUPPORT;
        }
}

पूर्णांक in4_addr_default_prefixlen(स्थिर काष्ठा in_addr *addr, अचिन्हित अक्षर *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        निश्चित(addr);
        निश्चित(prefixlen);

        अगर (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        अन्यथा अगर (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        अन्यथा अगर (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        अन्यथा
                /* class D or E, no default prefixlen */
                वापस -दुस्फल;

        वापस 0;
}

पूर्णांक in4_addr_default_subnet_mask(स्थिर काष्ठा in_addr *addr, काष्ठा in_addr *mask) {
        अचिन्हित अक्षर prefixlen;
        पूर्णांक r;

        निश्चित(addr);
        निश्चित(mask);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        अगर (r < 0)
                वापस r;

        in4_addr_prefixlen_to_netmask(mask, prefixlen);
        वापस 0;
}

पूर्णांक in4_addr_mask(काष्ठा in_addr *addr, अचिन्हित अक्षर prefixlen) {
        काष्ठा in_addr mask;

        निश्चित(addr);

        अगर (!in4_addr_prefixlen_to_netmask(&mask, prefixlen))
                वापस -EINVAL;

        addr->s_addr &= mask.s_addr;
        वापस 0;
}

पूर्णांक in6_addr_mask(काष्ठा in6_addr *addr, अचिन्हित अक्षर prefixlen) {
        अचिन्हित i;

        निश्चित(addr);

        क्रम (i = 0; i < 16; i++) {
                uint8_t mask;

                अगर (prefixlen >= 8) {
                        mask = 0xFF;
                        prefixlen -= 8;
                } अन्यथा अगर (prefixlen > 0) {
                        mask = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } अन्यथा {
                        निश्चित(prefixlen == 0);
                        mask = 0;
                }

                addr->s6_addr[i] &= mask;
        }

        वापस 0;
}

पूर्णांक in_addr_mask(पूर्णांक family, जोड़ in_addr_union *addr, अचिन्हित अक्षर prefixlen) {
        निश्चित(addr);

        चयन (family) {
        हाल AF_INET:
                वापस in4_addr_mask(&addr->in, prefixlen);
        हाल AF_INET6:
                वापस in6_addr_mask(&addr->in6, prefixlen);
        शेष:
                वापस -EAFNOSUPPORT;
        }
}

पूर्णांक in4_addr_prefix_covers_full(
                स्थिर काष्ठा in_addr *prefix,
                अचिन्हित अक्षर prefixlen,
                स्थिर काष्ठा in_addr *address,
                अचिन्हित अक्षर address_prefixlen) {

        काष्ठा in_addr masked_prefix, masked_address;
        पूर्णांक r;

        निश्चित(prefix);
        निश्चित(address);

        अगर (prefixlen > address_prefixlen)
                वापस false;

        masked_prefix = *prefix;
        r = in4_addr_mask(&masked_prefix, prefixlen);
        अगर (r < 0)
                वापस r;

        masked_address = *address;
        r = in4_addr_mask(&masked_address, prefixlen);
        अगर (r < 0)
                वापस r;

        वापस in4_addr_equal(&masked_prefix, &masked_address);
}

पूर्णांक in6_addr_prefix_covers_full(
                स्थिर काष्ठा in6_addr *prefix,
                अचिन्हित अक्षर prefixlen,
                स्थिर काष्ठा in6_addr *address,
                अचिन्हित अक्षर address_prefixlen) {

        काष्ठा in6_addr masked_prefix, masked_address;
        पूर्णांक r;

        निश्चित(prefix);
        निश्चित(address);

        अगर (prefixlen > address_prefixlen)
                वापस false;

        masked_prefix = *prefix;
        r = in6_addr_mask(&masked_prefix, prefixlen);
        अगर (r < 0)
                वापस r;

        masked_address = *address;
        r = in6_addr_mask(&masked_address, prefixlen);
        अगर (r < 0)
                वापस r;

        वापस in6_addr_equal(&masked_prefix, &masked_address);
}

पूर्णांक in_addr_prefix_covers_full(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *prefix,
                अचिन्हित अक्षर prefixlen,
                स्थिर जोड़ in_addr_union *address,
                अचिन्हित अक्षर address_prefixlen) {

        निश्चित(prefix);
        निश्चित(address);

        चयन (family) {
        हाल AF_INET:
                वापस in4_addr_prefix_covers_full(&prefix->in, prefixlen, &address->in, address_prefixlen);
        हाल AF_INET6:
                वापस in6_addr_prefix_covers_full(&prefix->in6, prefixlen, &address->in6, address_prefixlen);
        शेष:
                वापस -EAFNOSUPPORT;
        }
}

पूर्णांक in_addr_parse_prefixlen(पूर्णांक family, स्थिर अक्षर *p, अचिन्हित अक्षर *ret) {
        uint8_t u;
        पूर्णांक r;

        निश्चित(ret);

        अगर (!IN_SET(family, AF_INET, AF_INET6))
                वापस -EAFNOSUPPORT;

        r = safe_atou8(p, &u);
        अगर (r < 0)
                वापस r;

        अगर (u > FAMILY_ADDRESS_आकार(family) * 8)
                वापस -दुस्फल;

        *ret = u;
        वापस 0;
}

पूर्णांक in_addr_prefix_from_string(
                स्थिर अक्षर *p,
                पूर्णांक family,
                जोड़ in_addr_union *ret_prefix,
                अचिन्हित अक्षर *ret_prefixlen) {

        _cleanup_free_ अक्षर *str = शून्य;
        जोड़ in_addr_union बफर;
        स्थिर अक्षर *e, *l;
        अचिन्हित अक्षर k;
        पूर्णांक r;

        निश्चित(p);

        अगर (!IN_SET(family, AF_INET, AF_INET6))
                वापस -EAFNOSUPPORT;

        e = म_अक्षर(p, '/');
        अगर (e) {
                str = strndup(p, e - p);
                अगर (!str)
                        वापस -ENOMEM;

                l = str;
        } अन्यथा
                l = p;

        r = in_addr_from_string(family, l, &बफर);
        अगर (r < 0)
                वापस r;

        अगर (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                अगर (r < 0)
                        वापस r;
        } अन्यथा
                k = FAMILY_ADDRESS_आकार(family) * 8;

        अगर (ret_prefix)
                *ret_prefix = बफर;
        अगर (ret_prefixlen)
                *ret_prefixlen = k;

        वापस 0;
}

पूर्णांक in_addr_prefix_from_string_auto_full(
                स्थिर अक्षर *p,
                InAddrPrefixLenMode mode,
                पूर्णांक *ret_family,
                जोड़ in_addr_union *ret_prefix,
                अचिन्हित अक्षर *ret_prefixlen) {

        _cleanup_free_ अक्षर *str = शून्य;
        जोड़ in_addr_union बफर;
        स्थिर अक्षर *e, *l;
        अचिन्हित अक्षर k;
        पूर्णांक family, r;

        निश्चित(p);

        e = म_अक्षर(p, '/');
        अगर (e) {
                str = strndup(p, e - p);
                अगर (!str)
                        वापस -ENOMEM;

                l = str;
        } अन्यथा
                l = p;

        r = in_addr_from_string_auto(l, &family, &बफर);
        अगर (r < 0)
                वापस r;

        अगर (e) {
                r = in_addr_parse_prefixlen(family, e+1, &k);
                अगर (r < 0)
                        वापस r;
        } अन्यथा
                चयन (mode) {
                हाल PREFIXLEN_FULL:
                        k = FAMILY_ADDRESS_आकार(family) * 8;
                        अवरोध;
                हाल PREFIXLEN_REFUSE:
                        वापस -ENOANO; /* To distinguish this error from others. */
                शेष:
                        assert_not_reached();
                }

        अगर (ret_family)
                *ret_family = family;
        अगर (ret_prefix)
                *ret_prefix = बफर;
        अगर (ret_prefixlen)
                *ret_prefixlen = k;

        वापस 0;
}

व्योम in_addr_hash_func(स्थिर जोड़ in_addr_union *u, पूर्णांक family, काष्ठा siphash *state) {
        निश्चित(u);
        निश्चित(state);

        siphash24_compress(u->bytes, FAMILY_ADDRESS_आकार(family), state);
}

व्योम in_addr_तथ्य_hash_func(स्थिर काष्ठा in_addr_तथ्य *a, काष्ठा siphash *state) {
        निश्चित(a);
        निश्चित(state);

        siphash24_compress_typesafe(a->family, state);
        in_addr_hash_func(&a->address, a->family, state);
}

पूर्णांक in_addr_तथ्य_compare_func(स्थिर काष्ठा in_addr_तथ्य *x, स्थिर काष्ठा in_addr_तथ्य *y) {
        पूर्णांक r;

        निश्चित(x);
        निश्चित(y);

        r = CMP(x->family, y->family);
        अगर (r != 0)
                वापस r;

        वापस स_भेद(&x->address, &y->address, FAMILY_ADDRESS_आकार(x->family));
}

DEFINE_HASH_OPS(
        in_addr_तथ्य_hash_ops,
        काष्ठा in_addr_तथ्य,
        in_addr_तथ्य_hash_func,
        in_addr_तथ्य_compare_func);

DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(
        in_addr_तथ्य_hash_ops_free,
        काष्ठा in_addr_तथ्य,
        in_addr_तथ्य_hash_func,
        in_addr_तथ्य_compare_func,
        मुक्त);

व्योम in6_addr_hash_func(स्थिर काष्ठा in6_addr *addr, काष्ठा siphash *state) {
        निश्चित(addr);
        निश्चित(state);

        siphash24_compress_typesafe(*addr, state);
}

पूर्णांक in6_addr_compare_func(स्थिर काष्ठा in6_addr *a, स्थिर काष्ठा in6_addr *b) {
        निश्चित(a);
        निश्चित(b);

        वापस स_भेद(a, b, माप(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        काष्ठा in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(
        in6_addr_hash_ops_free,
        काष्ठा in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        मुक्त);
