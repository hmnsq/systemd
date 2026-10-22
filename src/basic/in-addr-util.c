/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <arpa/inet.h>
#అన్తర్భావ <endian.h>
#అన్తర్భావ <stdio.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "hash-funcs.h"
#అన్తర్భావ "in-addr-util.h"
#అన్తర్భావ "logarithm.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "random-util.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"

bool in4_addr_is_null(స్థిరరాశి నిర్మాణం in_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి a->s_addr == 0;
}

bool in6_addr_is_null(స్థిరరాశి నిర్మాణం in6_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి eqzero(a->s6_addr32);
}

పూర్ణాంకం లోపల_addr_is_null(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) {
        ప్రతిజ్ఞా(u);

        ఒకవేళ (family == AF_INET)
                తిరిగి in4_addr_is_null(&u->లోపల);

        ఒకవేళ (family == AF_INET6)
                తిరిగి in6_addr_is_null(&u->in6);

        తిరిగి -EAFNOSUPPORT;
}

bool in4_addr_is_link_local(స్థిరరాశి నిర్మాణం in_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి (be32toh(a->s_addr) & UINT32_C(0xFFFF0000)) == (UINT32_C(169) << 24 | UINT32_C(254) << 16);
}

bool in4_addr_is_link_local_dynamic(స్థిరరాశి నిర్మాణం in_addr *a) {
        ప్రతిజ్ఞా(a);

        ఒకవేళ (!in4_addr_is_link_local(a))
                తిరిగి false;

        /* 169.254.0.0/24 and 169.254.255.0/24 must not be used for the dynamic IPv4LL assignment.
         * See RFC 3927 Section 2.1:
         * The IPv4 prefix 169.254/16 is registered with the IANA for this purpose. The first 256 and last
         * 256 addresses in the 169.254/16 prefix are reserved for future use and MUST NOT be selected by a
         * host using this dynamic configuration mechanism. */
        తిరిగి !లోపల_సమితి(be32toh(a->s_addr) & 0x0000FF00U, 0x0000U, 0xFF00U);
}

bool in6_addr_is_link_local(స్థిరరాశి నిర్మాణం in6_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి (a->s6_addr32[0] & htobe32(0xffc00000)) == htobe32(0xfe800000);
}

పూర్ణాంకం లోపల_addr_is_link_local(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) {
        ప్రతిజ్ఞా(u);

        ఒకవేళ (family == AF_INET)
                తిరిగి in4_addr_is_link_local(&u->లోపల);

        ఒకవేళ (family == AF_INET6)
                తిరిగి in6_addr_is_link_local(&u->in6);

        తిరిగి -EAFNOSUPPORT;
}

bool in6_addr_is_link_local_all_nodes(స్థిరరాశి నిర్మాణం in6_addr *a) {
        ప్రతిజ్ఞా(a);

        /* ff02::1 */
        తిరిగి be32toh(a->s6_addr32[0]) == UINT32_C(0xff020000) &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == 0 &&
                be32toh(a->s6_addr32[3]) == UINT32_C(0x00000001);
}

bool in4_addr_is_multicast(స్థిరరాశి నిర్మాణం in_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి IN_MULTICAST(be32toh(a->s_addr));
}

bool in6_addr_is_multicast(స్థిరరాశి నిర్మాణం in6_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి a->s6_addr[0] == 0xff;
}

పూర్ణాంకం లోపల_addr_is_multicast(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) {
        ప్రతిజ్ఞా(u);

        ఒకవేళ (family == AF_INET)
                తిరిగి in4_addr_is_multicast(&u->లోపల);

        ఒకవేళ (family == AF_INET6)
                తిరిగి in6_addr_is_multicast(&u->in6);

        తిరిగి -EAFNOSUPPORT;
}

bool in4_addr_is_local_multicast(స్థిరరాశి నిర్మాణం in_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి (be32toh(a->s_addr) & UINT32_C(0xffffff00)) == UINT32_C(0xe0000000);
}

bool in4_addr_is_localhost(స్థిరరాశి నిర్మాణం in_addr *a) {
        ప్రతిజ్ఞా(a);

        /* All of 127.x.x.x is localhost. */
        తిరిగి (be32toh(a->s_addr) & UINT32_C(0xFF000000)) == UINT32_C(127) << 24;
}

bool in4_addr_is_non_local(స్థిరరాశి నిర్మాణం in_addr *a) {
        /* Whether the address is not null and not localhost.
         *
         * As such, it is suitable to configure as DNS/NTP server from DHCP. */
        తిరిగి !in4_addr_is_null(a) &&
               !in4_addr_is_localhost(a);
}

స్థిర bool in6_addr_is_loopback(స్థిరరాశి నిర్మాణం in6_addr *a) {
        తిరిగి memcmp(a, &(నిర్మాణం in6_addr) IN6ADDR_LOOPBACK_INIT, పరిమాణం(నిర్మాణం in6_addr)) == 0;
}

పూర్ణాంకం లోపల_addr_is_localhost(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) {
        ప్రతిజ్ఞా(u);

        ఒకవేళ (family == AF_INET)
                తిరిగి in4_addr_is_localhost(&u->లోపల);

        ఒకవేళ (family == AF_INET6)
                తిరిగి in6_addr_is_loopback(&u->in6);

        తిరిగి -EAFNOSUPPORT;
}

పూర్ణాంకం లోపల_addr_is_localhost_one(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) {
        ప్రతిజ్ఞా(u);

        ఒకవేళ (family == AF_INET)
                /* 127.0.0.1 */
                తిరిగి be32toh(u->లోపల.s_addr) == UINT32_C(0x7F000001);

        ఒకవేళ (family == AF_INET6)
                తిరిగి in6_addr_is_loopback(&u->in6);

        తిరిగి -EAFNOSUPPORT;
}

bool in6_addr_is_ipv4_mapped_చిరునామా(స్థిరరాశి నిర్మాణం in6_addr *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి a->s6_addr32[0] == 0 &&
                a->s6_addr32[1] == 0 &&
                a->s6_addr32[2] == htobe32(UINT32_C(0x0000ffff));
}

bool in4_addr_equal(స్థిరరాశి నిర్మాణం in_addr *a, స్థిరరాశి నిర్మాణం in_addr *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి a->s_addr == b->s_addr;
}

bool in6_addr_equal(స్థిరరాశి నిర్మాణం in6_addr *a, స్థిరరాశి నిర్మాణం in6_addr *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి memcmp(a, b, పరిమాణం(నిర్మాణం in6_addr)) == 0;
}

పూర్ణాంకం లోపల_addr_equal(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *a, స్థిరరాశి సంఘం లోపల_addr_union *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        ఒకవేళ (family == AF_INET)
                తిరిగి in4_addr_equal(&a->లోపల, &b->లోపల);

        ఒకవేళ (family == AF_INET6)
                తిరిగి in6_addr_equal(&a->in6, &b->in6);

        తిరిగి -EAFNOSUPPORT;
}

bool in4_addr_ఉపసర్గ_intersect(
                స్థిరరాశి నిర్మాణం in_addr *a,
                గుర్తులేని aprefixlen,
                స్థిరరాశి నిర్మాణం in_addr *b,
                గుర్తులేని bprefixlen) {

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        గుర్తులేని m = MIN3(aprefixlen, bprefixlen, (గుర్తులేని) (పరిమాణం(నిర్మాణం in_addr) * 8));
        ఒకవేళ (m == 0)
                తిరిగి true; /* Let's return earlier, to avoid shift by 32. */

        uint32_t x = be32toh(a->s_addr ^ b->s_addr);
        uint32_t n = 0xFFFFFFFFUL << (32 - m);
        తిరిగి (x & n) == 0;
}

bool in6_addr_ఉపసర్గ_intersect(
                స్థిరరాశి నిర్మాణం in6_addr *a,
                గుర్తులేని aprefixlen,
                స్థిరరాశి నిర్మాణం in6_addr *b,
                గుర్తులేని bprefixlen) {

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        గుర్తులేని m = MIN3(aprefixlen, bprefixlen, (గుర్తులేని) (పరిమాణం(నిర్మాణం in6_addr) * 8));
        ఒకవేళ (m == 0)
                తిరిగి true;

        కొరకు (size_t i = 0; i < పరిమాణం(నిర్మాణం in6_addr); i++) {
                uint8_t x = a->s6_addr[i] ^ b->s6_addr[i];
                uint8_t n = m < 8 ? (0xFF << (8 - m)) : 0xFF;
                ఒకవేళ ((x & n) != 0)
                        తిరిగి false;

                ఒకవేళ (m <= 8)
                        ఆపు;

                m -= 8;
        }

        తిరిగి true;
}

పూర్ణాంకం లోపల_addr_ఉపసర్గ_intersect(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *a,
                గుర్తులేని aprefixlen,
                స్థిరరాశి సంఘం లోపల_addr_union *b,
                గుర్తులేని bprefixlen) {

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        /* Checks whether there are any addresses that are in both networks. */

        ఒకవేళ (family == AF_INET)
                తిరిగి in4_addr_ఉపసర్గ_intersect(&a->లోపల, aprefixlen, &b->లోపల, bprefixlen);

        ఒకవేళ (family == AF_INET6)
                తిరిగి in6_addr_ఉపసర్గ_intersect(&a->in6, aprefixlen, &b->in6, bprefixlen);

        తిరిగి -EAFNOSUPPORT;
}

పూర్ణాంకం లోపల_addr_random_ఉపసర్గ(
                పూర్ణాంకం family,
                సంఘం లోపల_addr_union *u,
                గుర్తులేని prefixlen_fixed_part,
                గుర్తులేని prefixlen) {

        ప్రతిజ్ఞా(u);

        /* Random network part of an address by one. */

        ఒకవేళ (prefixlen <= 0)
                తిరిగి 0;

        ఒకవేళ (family == AF_INET) {
                uint32_t c, n;

                ఒకవేళ (prefixlen_fixed_part > 32)
                        prefixlen_fixed_part = 32;
                ఒకవేళ (prefixlen > 32)
                        prefixlen = 32;
                ఒకవేళ (prefixlen_fixed_part >= prefixlen)
                        తిరిగి -EINVAL;

                c = be32toh(u->లోపల.s_addr);
                c &= ((UINT32_C(1) << prefixlen_fixed_part) - 1) << (32 - prefixlen_fixed_part);

                random_బైట్లు(&n, పరిమాణం(n));
                n &= ((UINT32_C(1) << (prefixlen - prefixlen_fixed_part)) - 1) << (32 - prefixlen);

                u->లోపల.s_addr = htobe32(n | c);
                తిరిగి 1;
        }

        ఒకవేళ (family == AF_INET6) {
                నిర్మాణం in6_addr n;
                గుర్తులేని i, j;

                ఒకవేళ (prefixlen_fixed_part > 128)
                        prefixlen_fixed_part = 128;
                ఒకవేళ (prefixlen > 128)
                        prefixlen = 128;
                ఒకవేళ (prefixlen_fixed_part >= prefixlen)
                        తిరిగి -EINVAL;

                random_బైట్లు(&n, పరిమాణం(n));

                కొరకు (i = 0; i < 16; i++) {
                        uint8_t ముసుగు_fixed_part = 0, ముసుగు_3 = 0;

                        ఒకవేళ (i < (prefixlen_fixed_part + 7) / 8) {
                                ఒకవేళ (i < prefixlen_fixed_part / 8)
                                        ముసుగు_fixed_part = 0xffu;
                                లేకపోతే {
                                        j = prefixlen_fixed_part % 8;
                                        ముసుగు_fixed_part = ((UINT8_C(1) << (j + 1)) - 1) << (8 - j);
                                }
                        }

                        ఒకవేళ (i < (prefixlen + 7) / 8) {
                                ఒకవేళ (i < prefixlen / 8)
                                        ముసుగు_3 = 0xffu ^ ముసుగు_fixed_part;
                                లేకపోతే {
                                        j = prefixlen % 8;
                                        ముసుగు_3 = (((UINT8_C(1) << (j + 1)) - 1) << (8 - j)) ^ ముసుగు_fixed_part;
                                }
                        }

                        u->in6.s6_addr[i] &= ముసుగు_fixed_part;
                        u->in6.s6_addr[i] |= n.s6_addr[i] & ముసుగు_3;
                }

                తిరిగి 1;
        }

        తిరిగి -EAFNOSUPPORT;
}

పూర్ణాంకం లోపల_addr_ఉపసర్గ_range(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *లోపల,
                గుర్తులేని prefixlen,
                సంఘం లోపల_addr_union *ret_ప్రారంభం,
                సంఘం లోపల_addr_union *ret_చివర) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(లోపల);

        /* Note, on overflow, e.g. input is 255.255.255.255, then ret_end will be a null address. */

        ఎంపిక (family) {
        స్థితి AF_INET:
                ఒకవేళ (prefixlen > 32)
                        తిరిగి -EINVAL;
                ఆపు;
        స్థితి AF_INET6:
                ఒకవేళ (prefixlen > 128)
                        తిరిగి -EINVAL;
                ఆపు;
        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }

        ఒకవేళ (prefixlen == 0) {
                /* shortcut for trivial case */
                ఒకవేళ (ret_ప్రారంభం)
                        *ret_ప్రారంభం = లోపల_ADDR_NULL;
                ఒకవేళ (ret_చివర)
                        *ret_చివర = లోపల_ADDR_NULL;
                తిరిగి 0;
        }

        సంఘం లోపల_addr_union ప్రారంభం = *లోపల;
        r = లోపల_addr_ముసుగు(family, &ప్రారంభం, prefixlen);
        ఒకవేళ (r < 0)
                తిరిగి r;

        సంఘం లోపల_addr_union చివర = ప్రారంభం;
        uint8_t కూడు = UINT8_C(1) << ((FAMILY_చిరునామా_కొలత(family) * 8 - prefixlen) % 8);
        కొరకు (గుర్తులేని i = DIV_విడత_పైకి(prefixlen, 8); i-- > 0;)
                ఒకవేళ (కూడు > UINT8_MAX - చివర.బైట్లు[i]) {
                        చివర.బైట్లు[i] = 0;
                        కూడు = 1;
                } లేకపోతే {
                        చివర.బైట్లు[i] += కూడు;
                        ఆపు;
                }

        ఒకవేళ (ret_ప్రారంభం)
                *ret_ప్రారంభం = ప్రారంభం;
        ఒకవేళ (ret_చివర)
                *ret_చివర = చివర;

        తిరిగి 0;
}

పూర్ణాంకం లోపల_addr_గమ్యం_string(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *x = ఖాళీ;
        size_t l;

        ప్రతిజ్ఞా(u);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (family == AF_INET)
                l = INET_ADDRSTRLEN;
        లేకపోతే ఒకవేళ (family == AF_INET6)
                l = INET6_ADDRSTRLEN;
        లేకపోతే
                తిరిగి -EAFNOSUPPORT;

        x = new(అక్షరం, l);
        ఒకవేళ (!x)
                తిరిగి -ENOMEM;

        errno = 0;
        ఒకవేళ (!typesafe_inet_ntop(family, u, x, l))
                తిరిగి errno_or_else(EINVAL);

        *ret = TAKE_PTR(x);
        తిరిగి 0;
}

స్థిరరాశి అక్షరం* typesafe_inet_ntop(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *a, అక్షరం *buf, size_t len) {
        తిరిగి inet_ntop(family, a, buf, len);
}

స్థిరరాశి అక్షరం* typesafe_inet_ntop4(స్థిరరాశి నిర్మాణం in_addr *a, అక్షరం *buf, size_t len) {
        తిరిగి inet_ntop(AF_INET, a, buf, len);
}

స్థిరరాశి అక్షరం* typesafe_inet_ntop6(స్థిరరాశి నిర్మాణం in6_addr *a, అక్షరం *buf, size_t len) {
        తిరిగి inet_ntop(AF_INET6, a, buf, len);
}

పూర్ణాంకం లోపల_addr_ఉపసర్గ_గమ్యం_string(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *u,
                గుర్తులేని prefixlen,
                అక్షరం *buf,
                size_t buf_len) {

        ప్రతిజ్ఞా(u);
        ప్రతిజ్ఞా(buf);

        ఒకవేళ (!లోపల_సమితి(family, AF_INET, AF_INET6))
                తిరిగి -EAFNOSUPPORT;

        errno = 0;
        ఒకవేళ (!typesafe_inet_ntop(family, u, buf, buf_len))
                తిరిగి errno_or_else(ENOSPC);

        size_t l = అక్షరశ్రేణి_పొడవు(buf);
        ఒకవేళ (!snprintf_ok(buf + l, buf_len - l, "/%u", prefixlen))
                తిరిగి -ENOSPC;
        తిరిగి 0;
}

పూర్ణాంకం లోపల_addr_పోర్టు_ifindex_name_గమ్యం_string(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u, uint16_t port, పూర్ణాంకం ifindex, స్థిరరాశి అక్షరం *server_name, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *ip_str = ఖాళీ, *x = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(లోపల_సమితి(family, AF_INET, AF_INET6));
        ప్రతిజ్ఞా(u);
        ప్రతిజ్ఞా(ret);

        /* Much like in_addr_to_string(), but optionally appends the zone interface index to the address, to properly
         * handle IPv6 link-local addresses. */

        r = లోపల_addr_గమ్యం_string(family, u, &ip_str);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (family == AF_INET6) {
                r = లోపల_addr_is_link_local(family, u);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ifindex = 0;
        } లేకపోతే
                ifindex = 0; /* For IPv4 address, ifindex is always ignored. */

        ఒకవేళ (port == 0 && ifindex == 0 && isempty(server_name)) {
                *ret = TAKE_PTR(ip_str);
                తిరిగి 0;
        }

        స్థిరరాశి అక్షరం *separator = isempty(server_name) ? "" : "#";
        server_name = strempty(server_name);

        ఒకవేళ (port > 0) {
                ఒకవేళ (family == AF_INET6) {
                        ఒకవేళ (ifindex > 0)
                                x = asprintf_సురక్షిత("[%s]:%"PRIu16"%%%i%s%s", ip_str, port, ifindex, separator, server_name);
                        లేకపోతే
                                x = asprintf_సురక్షిత("[%s]:%"PRIu16"%s%s", ip_str, port, separator, server_name);
                } లేకపోతే
                        x = asprintf_సురక్షిత("%s:%"PRIu16"%s%s", ip_str, port, separator, server_name);
        } లేకపోతే {
                ఒకవేళ (ifindex > 0)
                        x = asprintf_సురక్షిత("%s%%%i%s%s", ip_str, ifindex, separator, server_name);
                లేకపోతే
                        x = strjoin(ip_str, separator, server_name);
        }
        ఒకవేళ (!x)
                తిరిగి -ENOMEM;

        *ret = TAKE_PTR(x);
        తిరిగి 0;
}

పూర్ణాంకం లోపల_addr_from_string(పూర్ణాంకం family, స్థిరరాశి అక్షరం *s, సంఘం లోపల_addr_union *ret) {
        సంఘం లోపల_addr_union బఫరు;
        ప్రతిజ్ఞా(s);

        ఒకవేళ (!లోపల_సమితి(family, AF_INET, AF_INET6))
                తిరిగి -EAFNOSUPPORT;

        errno = 0;
        ఒకవేళ (inet_pton(family, s, ret ?: &బఫరు) <= 0)
                తిరిగి errno_or_else(EINVAL);

        తిరిగి 0;
}

పూర్ణాంకం లోపల_addr_from_string_auto(స్థిరరాశి అక్షరం *s, పూర్ణాంకం *ret_family, సంఘం లోపల_addr_union *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        r = లోపల_addr_from_string(AF_INET, s, ret);
        ఒకవేళ (r >= 0) {
                ఒకవేళ (ret_family)
                        *ret_family = AF_INET;
                తిరిగి 0;
        }

        r = లోపల_addr_from_string(AF_INET6, s, ret);
        ఒకవేళ (r >= 0) {
                ఒకవేళ (ret_family)
                        *ret_family = AF_INET6;
                తిరిగి 0;
        }

        తిరిగి -EINVAL;
}

గుర్తులేని అక్షరం in4_addr_netmask_గమ్యం_prefixlen(స్థిరరాశి నిర్మాణం in_addr *addr) {
        ప్రతిజ్ఞా(addr);

        తిరిగి 32U - u32ctz(be32toh(addr->s_addr));
}

/* Calculate an IPv4 netmask from prefix length, for example /8 -> 255.0.0.0. */
నిర్మాణం in_addr* in4_addr_prefixlen_గమ్యం_netmask(నిర్మాణం in_addr *addr, గుర్తులేని అక్షరం prefixlen) {
        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(prefixlen <= 32);

        /* Shifting beyond 32 is not defined, handle this specially. */
        ఒకవేళ (prefixlen == 0)
                addr->s_addr = 0;
        లేకపోతే
                addr->s_addr = htobe32((0xffffffff << (32 - prefixlen)) & 0xffffffff);

        తిరిగి addr;
}

/* Calculate an IPv6 netmask from prefix length, for example /16 -> ffff::. */
నిర్మాణం in6_addr* in6_addr_prefixlen_గమ్యం_netmask(నిర్మాణం in6_addr *addr, గుర్తులేని అక్షరం prefixlen) {
        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(prefixlen <= 128);

        కొరకు (గుర్తులేని i = 0; i < 16; i++) {
                uint8_t ముసుగు_3;

                ఒకవేళ (prefixlen >= 8) {
                        ముసుగు_3 = 0xFF;
                        prefixlen -= 8;
                } లేకపోతే ఒకవేళ (prefixlen > 0) {
                        ముసుగు_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } లేకపోతే {
                        ప్రతిజ్ఞా(prefixlen == 0);
                        ముసుగు_3 = 0;
                }

                addr->s6_addr[i] = ముసుగు_3;
        }

        తిరిగి addr;
}

/* Calculate an IPv4 or IPv6 netmask from prefix length, for example /8 -> 255.0.0.0 or /16 -> ffff::. */
పూర్ణాంకం లోపల_addr_prefixlen_గమ్యం_netmask(పూర్ణాంకం family, సంఘం లోపల_addr_union *addr, గుర్తులేని అక్షరం prefixlen) {
        ప్రతిజ్ఞా(addr);

        ఎంపిక (family) {
        స్థితి AF_INET:
                in4_addr_prefixlen_గమ్యం_netmask(&addr->లోపల, prefixlen);
                తిరిగి 0;
        స్థితి AF_INET6:
                in6_addr_prefixlen_గమ్యం_netmask(&addr->in6, prefixlen);
                తిరిగి 0;
        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

పూర్ణాంకం in4_addr_default_prefixlen(స్థిరరాశి నిర్మాణం in_addr *addr, గుర్తులేని అక్షరం *prefixlen) {
        uint8_t msb_octet = *(uint8_t*) addr;

        /* addr may not be aligned, so make sure we only access it byte-wise */

        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(prefixlen);

        ఒకవేళ (msb_octet < 128)
                /* class A, leading bits: 0 */
                *prefixlen = 8;
        లేకపోతే ఒకవేళ (msb_octet < 192)
                /* class B, leading bits 10 */
                *prefixlen = 16;
        లేకపోతే ఒకవేళ (msb_octet < 224)
                /* class C, leading bits 110 */
                *prefixlen = 24;
        లేకపోతే
                /* class D or E, no default prefixlen */
                తిరిగి -ERANGE;

        తిరిగి 0;
}

పూర్ణాంకం in4_addr_default_subnet_ముసుగు(స్థిరరాశి నిర్మాణం in_addr *addr, నిర్మాణం in_addr *ముసుగు_3) {
        గుర్తులేని అక్షరం prefixlen;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(ముసుగు_3);

        r = in4_addr_default_prefixlen(addr, &prefixlen);
        ఒకవేళ (r < 0)
                తిరిగి r;

        in4_addr_prefixlen_గమ్యం_netmask(ముసుగు_3, prefixlen);
        తిరిగి 0;
}

పూర్ణాంకం in4_addr_ముసుగు(నిర్మాణం in_addr *addr, గుర్తులేని అక్షరం prefixlen) {
        నిర్మాణం in_addr ముసుగు_3;

        ప్రతిజ్ఞా(addr);

        ఒకవేళ (!in4_addr_prefixlen_గమ్యం_netmask(&ముసుగు_3, prefixlen))
                తిరిగి -EINVAL;

        addr->s_addr &= ముసుగు_3.s_addr;
        తిరిగి 0;
}

పూర్ణాంకం in6_addr_ముసుగు(నిర్మాణం in6_addr *addr, గుర్తులేని అక్షరం prefixlen) {
        గుర్తులేని i;

        ప్రతిజ్ఞా(addr);

        కొరకు (i = 0; i < 16; i++) {
                uint8_t ముసుగు_3;

                ఒకవేళ (prefixlen >= 8) {
                        ముసుగు_3 = 0xFF;
                        prefixlen -= 8;
                } లేకపోతే ఒకవేళ (prefixlen > 0) {
                        ముసుగు_3 = 0xFF << (8 - prefixlen);
                        prefixlen = 0;
                } లేకపోతే {
                        ప్రతిజ్ఞా(prefixlen == 0);
                        ముసుగు_3 = 0;
                }

                addr->s6_addr[i] &= ముసుగు_3;
        }

        తిరిగి 0;
}

పూర్ణాంకం లోపల_addr_ముసుగు(పూర్ణాంకం family, సంఘం లోపల_addr_union *addr, గుర్తులేని అక్షరం prefixlen) {
        ప్రతిజ్ఞా(addr);

        ఎంపిక (family) {
        స్థితి AF_INET:
                తిరిగి in4_addr_ముసుగు(&addr->లోపల, prefixlen);
        స్థితి AF_INET6:
                తిరిగి in6_addr_ముసుగు(&addr->in6, prefixlen);
        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

పూర్ణాంకం in4_addr_ఉపసర్గ_covers_నిండిన(
                స్థిరరాశి నిర్మాణం in_addr *ఉపసర్గ,
                గుర్తులేని అక్షరం prefixlen,
                స్థిరరాశి నిర్మాణం in_addr *చిరునామా,
                గుర్తులేని అక్షరం చిరునామా_prefixlen) {

        నిర్మాణం in_addr masked_ఉపసర్గ, masked_చిరునామా;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ఉపసర్గ);
        ప్రతిజ్ఞా(చిరునామా);

        ఒకవేళ (prefixlen > చిరునామా_prefixlen)
                తిరిగి false;

        masked_ఉపసర్గ = *ఉపసర్గ;
        r = in4_addr_ముసుగు(&masked_ఉపసర్గ, prefixlen);
        ఒకవేళ (r < 0)
                తిరిగి r;

        masked_చిరునామా = *చిరునామా;
        r = in4_addr_ముసుగు(&masked_చిరునామా, prefixlen);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి in4_addr_equal(&masked_ఉపసర్గ, &masked_చిరునామా);
}

పూర్ణాంకం in6_addr_ఉపసర్గ_covers_నిండిన(
                స్థిరరాశి నిర్మాణం in6_addr *ఉపసర్గ,
                గుర్తులేని అక్షరం prefixlen,
                స్థిరరాశి నిర్మాణం in6_addr *చిరునామా,
                గుర్తులేని అక్షరం చిరునామా_prefixlen) {

        నిర్మాణం in6_addr masked_ఉపసర్గ, masked_చిరునామా;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ఉపసర్గ);
        ప్రతిజ్ఞా(చిరునామా);

        ఒకవేళ (prefixlen > చిరునామా_prefixlen)
                తిరిగి false;

        masked_ఉపసర్గ = *ఉపసర్గ;
        r = in6_addr_ముసుగు(&masked_ఉపసర్గ, prefixlen);
        ఒకవేళ (r < 0)
                తిరిగి r;

        masked_చిరునామా = *చిరునామా;
        r = in6_addr_ముసుగు(&masked_చిరునామా, prefixlen);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి in6_addr_equal(&masked_ఉపసర్గ, &masked_చిరునామా);
}

పూర్ణాంకం లోపల_addr_ఉపసర్గ_covers_నిండిన(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *ఉపసర్గ,
                గుర్తులేని అక్షరం prefixlen,
                స్థిరరాశి సంఘం లోపల_addr_union *చిరునామా,
                గుర్తులేని అక్షరం చిరునామా_prefixlen) {

        ప్రతిజ్ఞా(ఉపసర్గ);
        ప్రతిజ్ఞా(చిరునామా);

        ఎంపిక (family) {
        స్థితి AF_INET:
                తిరిగి in4_addr_ఉపసర్గ_covers_నిండిన(&ఉపసర్గ->లోపల, prefixlen, &చిరునామా->లోపల, చిరునామా_prefixlen);
        స్థితి AF_INET6:
                తిరిగి in6_addr_ఉపసర్గ_covers_నిండిన(&ఉపసర్గ->in6, prefixlen, &చిరునామా->in6, చిరునామా_prefixlen);
        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

పూర్ణాంకం లోపల_addr_parse_prefixlen(పూర్ణాంకం family, స్థిరరాశి అక్షరం *p, గుర్తులేని అక్షరం *ret) {
        uint8_t u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!లోపల_సమితి(family, AF_INET, AF_INET6))
                తిరిగి -EAFNOSUPPORT;

        r = సురక్షిత_atou8(p, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (u > FAMILY_చిరునామా_కొలత(family) * 8)
                తిరిగి -ERANGE;

        *ret = u;
        తిరిగి 0;
}

పూర్ణాంకం లోపల_addr_ఉపసర్గ_from_string(
                స్థిరరాశి అక్షరం *p,
                పూర్ణాంకం family,
                సంఘం లోపల_addr_union *ret_ఉపసర్గ,
                గుర్తులేని అక్షరం *ret_prefixlen) {

        _cleanup_free_ అక్షరం *str = ఖాళీ;
        సంఘం లోపల_addr_union బఫరు;
        స్థిరరాశి అక్షరం *e, *l;
        గుర్తులేని అక్షరం k;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);

        ఒకవేళ (!లోపల_సమితి(family, AF_INET, AF_INET6))
                తిరిగి -EAFNOSUPPORT;

        e = strchr(p, '/');
        ఒకవేళ (e) {
                str = strndup(p, e - p);
                ఒకవేళ (!str)
                        తిరిగి -ENOMEM;

                l = str;
        } లేకపోతే
                l = p;

        r = లోపల_addr_from_string(family, l, &బఫరు);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (e) {
                r = లోపల_addr_parse_prefixlen(family, e+1, &k);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        } లేకపోతే
                k = FAMILY_చిరునామా_కొలత(family) * 8;

        ఒకవేళ (ret_ఉపసర్గ)
                *ret_ఉపసర్గ = బఫరు;
        ఒకవేళ (ret_prefixlen)
                *ret_prefixlen = k;

        తిరిగి 0;
}

పూర్ణాంకం లోపల_addr_ఉపసర్గ_from_string_auto_నిండిన(
                స్థిరరాశి అక్షరం *p,
                లోపల_Addr_ఉపసర్గ_Len_బహుళకం బహుళకం,
                పూర్ణాంకం *ret_family,
                సంఘం లోపల_addr_union *ret_ఉపసర్గ,
                గుర్తులేని అక్షరం *ret_prefixlen) {

        _cleanup_free_ అక్షరం *str = ఖాళీ;
        సంఘం లోపల_addr_union బఫరు;
        స్థిరరాశి అక్షరం *e, *l;
        గుర్తులేని అక్షరం k;
        పూర్ణాంకం family, r;

        ప్రతిజ్ఞా(p);

        e = strchr(p, '/');
        ఒకవేళ (e) {
                str = strndup(p, e - p);
                ఒకవేళ (!str)
                        తిరిగి -ENOMEM;

                l = str;
        } లేకపోతే
                l = p;

        r = లోపల_addr_from_string_auto(l, &family, &బఫరు);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (e) {
                r = లోపల_addr_parse_prefixlen(family, e+1, &k);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        } లేకపోతే
                ఎంపిక (బహుళకం) {
                స్థితి PREFIXLEN_నిండిన:
                        k = FAMILY_చిరునామా_కొలత(family) * 8;
                        ఆపు;
                స్థితి PREFIXLEN_REFUSE:
                        తిరిగి -ENOANO; /* To distinguish this error from others. */
                అప్రమేయ:
                        assert_not_reached();
                }

        ఒకవేళ (ret_family)
                *ret_family = family;
        ఒకవేళ (ret_ఉపసర్గ)
                *ret_ఉపసర్గ = బఫరు;
        ఒకవేళ (ret_prefixlen)
                *ret_prefixlen = k;

        తిరిగి 0;
}

శూన్య లోపల_addr_hash_func(స్థిరరాశి సంఘం లోపల_addr_union *u, పూర్ణాంకం family, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(u);
        ప్రతిజ్ఞా(state);

        siphash24_compress(u->బైట్లు, FAMILY_చిరునామా_కొలత(family), state);
}

శూన్య లోపల_addr_దత్తాంశం_hash_func(స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *a, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(state);

        siphash24_compress_typesafe(a->family, state);
        లోపల_addr_hash_func(&a->చిరునామా, a->family, state);
}

పూర్ణాంకం లోపల_addr_దత్తాంశం_compare_func(స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *x, స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *y) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(x);
        ప్రతిజ్ఞా(y);

        r = CMP(x->family, y->family);
        ఒకవేళ (r != 0)
                తిరిగి r;

        తిరిగి memcmp(&x->చిరునామా, &y->చిరునామా, FAMILY_చిరునామా_కొలత(x->family));
}

DEFINE_HASH_OPS(
        లోపల_addr_దత్తాంశం_hash_ops,
        నిర్మాణం లోపల_addr_దత్తాంశం,
        లోపల_addr_దత్తాంశం_hash_func,
        లోపల_addr_దత్తాంశం_compare_func);

DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(
        లోపల_addr_దత్తాంశం_hash_ops_free,
        నిర్మాణం లోపల_addr_దత్తాంశం,
        లోపల_addr_దత్తాంశం_hash_func,
        లోపల_addr_దత్తాంశం_compare_func,
        విడుదల);

శూన్య in6_addr_hash_func(స్థిరరాశి నిర్మాణం in6_addr *addr, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(state);

        siphash24_compress_typesafe(*addr, state);
}

పూర్ణాంకం in6_addr_compare_func(స్థిరరాశి నిర్మాణం in6_addr *a, స్థిరరాశి నిర్మాణం in6_addr *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి memcmp(a, b, పరిమాణం(*a));
}

DEFINE_HASH_OPS(
        in6_addr_hash_ops,
        నిర్మాణం in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func);

DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(
        in6_addr_hash_ops_free,
        నిర్మాణం in6_addr,
        in6_addr_hash_func,
        in6_addr_compare_func,
        విడుదల);
