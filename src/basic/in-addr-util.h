/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <netinet/in.h>

#అన్తర్భావ "forward.h"

సంఘం లోపల_addr_union {
        నిర్మాణం in_addr లోపల;
        నిర్మాణం in6_addr in6;
        uint8_t బైట్లు[CONST_MAX(పరిమాణం(నిర్మాణం in_addr), పరిమాణం(నిర్మాణం in6_addr))];
};

నిర్మాణం లోపల_addr_దత్తాంశం {
        పూర్ణాంకం family;
        సంఘం లోపల_addr_union చిరునామా;
};

bool in4_addr_is_null(స్థిరరాశి నిర్మాణం in_addr *a) శుద్ధ;
స్థిర అంతఃవరుస bool in4_addr_is_సమితి(స్థిరరాశి నిర్మాణం in_addr *a) {
        తిరిగి !in4_addr_is_null(a);
}
bool in6_addr_is_null(స్థిరరాశి నిర్మాణం in6_addr *a) శుద్ధ;
స్థిర అంతఃవరుస bool in6_addr_is_సమితి(స్థిరరాశి నిర్మాణం in6_addr *a) {
        తిరిగి !in6_addr_is_null(a);
}
పూర్ణాంకం లోపల_addr_is_null(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) శుద్ధ;
స్థిర అంతఃవరుస bool లోపల_addr_is_సమితి(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) {
        తిరిగి లోపల_addr_is_null(family, u) == 0;
}
స్థిర అంతఃవరుస పూర్ణాంకం లోపల_addr_దత్తాంశం_is_null(స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *a) {
        ప్రతిజ్ఞా(a);
        తిరిగి లోపల_addr_is_null(a->family, &a->చిరునామా);
}
స్థిర అంతఃవరుస bool లోపల_addr_దత్తాంశం_is_సమితి(స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *a) {
        తిరిగి లోపల_addr_దత్తాంశం_is_null(a);
}

bool in4_addr_is_multicast(స్థిరరాశి నిర్మాణం in_addr *a) శుద్ధ;
bool in6_addr_is_multicast(స్థిరరాశి నిర్మాణం in6_addr *a) శుద్ధ;
పూర్ణాంకం లోపల_addr_is_multicast(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) శుద్ధ;

bool in4_addr_is_link_local(స్థిరరాశి నిర్మాణం in_addr *a) శుద్ధ;
bool in4_addr_is_link_local_dynamic(స్థిరరాశి నిర్మాణం in_addr *a) శుద్ధ;
bool in6_addr_is_link_local(స్థిరరాశి నిర్మాణం in6_addr *a) శుద్ధ;
పూర్ణాంకం లోపల_addr_is_link_local(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) శుద్ధ;
bool in6_addr_is_link_local_all_nodes(స్థిరరాశి నిర్మాణం in6_addr *a) శుద్ధ;

bool in4_addr_is_localhost(స్థిరరాశి నిర్మాణం in_addr *a) శుద్ధ;
పూర్ణాంకం లోపల_addr_is_localhost(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) శుద్ధ;
పూర్ణాంకం లోపల_addr_is_localhost_one(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u) శుద్ధ;

bool in4_addr_is_local_multicast(స్థిరరాశి నిర్మాణం in_addr *a) శుద్ధ;
bool in4_addr_is_non_local(స్థిరరాశి నిర్మాణం in_addr *a) శుద్ధ;
bool in6_addr_is_ipv4_mapped_చిరునామా(స్థిరరాశి నిర్మాణం in6_addr *a) శుద్ధ;

bool in4_addr_equal(స్థిరరాశి నిర్మాణం in_addr *a, స్థిరరాశి నిర్మాణం in_addr *b) శుద్ధ;
bool in6_addr_equal(స్థిరరాశి నిర్మాణం in6_addr *a, స్థిరరాశి నిర్మాణం in6_addr *b) శుద్ధ;
పూర్ణాంకం లోపల_addr_equal(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *a, స్థిరరాశి సంఘం లోపల_addr_union *b) శుద్ధ;
bool in4_addr_ఉపసర్గ_intersect(
                స్థిరరాశి నిర్మాణం in_addr *a,
                గుర్తులేని aprefixlen,
                స్థిరరాశి నిర్మాణం in_addr *b,
                గుర్తులేని bprefixlen) శుద్ధ;
bool in6_addr_ఉపసర్గ_intersect(
                స్థిరరాశి నిర్మాణం in6_addr *a,
                గుర్తులేని aprefixlen,
                స్థిరరాశి నిర్మాణం in6_addr *b,
                గుర్తులేని bprefixlen) శుద్ధ;
పూర్ణాంకం లోపల_addr_ఉపసర్గ_intersect(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *a,
                గుర్తులేని aprefixlen,
                స్థిరరాశి సంఘం లోపల_addr_union *b,
                గుర్తులేని bprefixlen) శుద్ధ;
పూర్ణాంకం లోపల_addr_random_ఉపసర్గ(పూర్ణాంకం family, సంఘం లోపల_addr_union *u, గుర్తులేని prefixlen_fixed_part, గుర్తులేని prefixlen);
పూర్ణాంకం లోపల_addr_ఉపసర్గ_range(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *లోపల,
                గుర్తులేని prefixlen,
                సంఘం లోపల_addr_union *ret_ప్రారంభం,
                సంఘం లోపల_addr_union *ret_చివర);

పూర్ణాంకం లోపల_addr_గమ్యం_string(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u, అక్షరం **ret);
స్థిర అంతఃవరుస పూర్ణాంకం in6_addr_గమ్యం_string(స్థిరరాశి నిర్మాణం in6_addr *u, అక్షరం **ret) {
        తిరిగి లోపల_addr_గమ్యం_string(AF_INET6, (స్థిరరాశి సంఘం లోపల_addr_union*) u, ret);
}

స్థిరరాశి అక్షరం* typesafe_inet_ntop(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *a, అక్షరం *buf, size_t len);
స్థిరరాశి అక్షరం* typesafe_inet_ntop4(స్థిరరాశి నిర్మాణం in_addr *a, అక్షరం *buf, size_t len);
స్థిరరాశి అక్షరం* typesafe_inet_ntop6(స్థిరరాశి నిర్మాణం in6_addr *a, అక్షరం *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#నిర్వచించు లోపల_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#నిర్వచించు లోపల_ADDR_గమ్యం_STRING(family, addr) typesafe_inet_ntop(family, addr, (అక్షరం[లోపల_ADDR_MAX]){}, లోపల_ADDR_MAX)
#నిర్వచించు IN4_ADDR_గమ్యం_STRING(addr) typesafe_inet_ntop4(addr, (అక్షరం[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#నిర్వచించు IN6_ADDR_గమ్యం_STRING(addr) typesafe_inet_ntop6(addr, (అక్షరం[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

పూర్ణాంకం లోపల_addr_ఉపసర్గ_గమ్యం_string(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *u,
                గుర్తులేని prefixlen,
                అక్షరం *buf,
                size_t buf_len);

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* లోపల_addr_ఉపసర్గ_గమ్యం_string_2(
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *u,
                గుర్తులేని prefixlen,
                అక్షరం *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(లోపల_addr_ఉపసర్గ_గమ్యం_string(family, u, prefixlen, buf, buf_len) == 0);
        తిరిగి buf;
}
స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* in4_addr_ఉపసర్గ_గమ్యం_string(స్థిరరాశి నిర్మాణం in_addr *a, గుర్తులేని prefixlen, అక్షరం *buf, size_t buf_len) {
        తిరిగి లోపల_addr_ఉపసర్గ_గమ్యం_string_2(AF_INET, (స్థిరరాశి సంఘం లోపల_addr_union *) a, prefixlen, buf, buf_len);
}
స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* in6_addr_ఉపసర్గ_గమ్యం_string(స్థిరరాశి నిర్మాణం in6_addr *a, గుర్తులేని prefixlen, అక్షరం *buf, size_t buf_len) {
        తిరిగి లోపల_addr_ఉపసర్గ_గమ్యం_string_2(AF_INET6, (స్థిరరాశి సంఘం లోపల_addr_union *) a, prefixlen, buf, buf_len);
}

#నిర్వచించు ఉపసర్గ_SUFFIX_MAX (1 + DECIMAL_STR_MAX(గుర్తులేని))
#నిర్వచించు లోపల_ADDR_ఉపసర్గ_గమ్యం_STRING(family, addr, prefixlen) \
        లోపల_addr_ఉపసర్గ_గమ్యం_string_2(family, addr, prefixlen, (అక్షరం[లోపల_ADDR_MAX + ఉపసర్గ_SUFFIX_MAX]){}, లోపల_ADDR_MAX + ఉపసర్గ_SUFFIX_MAX)
#నిర్వచించు IN4_ADDR_ఉపసర్గ_గమ్యం_STRING(addr, prefixlen) \
        in4_addr_ఉపసర్గ_గమ్యం_string(addr, prefixlen, (అక్షరం[INET_ADDRSTRLEN + ఉపసర్గ_SUFFIX_MAX]){}, INET_ADDRSTRLEN + ఉపసర్గ_SUFFIX_MAX)
#నిర్వచించు IN6_ADDR_ఉపసర్గ_గమ్యం_STRING(addr, prefixlen) \
        in6_addr_ఉపసర్గ_గమ్యం_string(addr, prefixlen, (అక్షరం[INET6_ADDRSTRLEN + ఉపసర్గ_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + ఉపసర్గ_SUFFIX_MAX)

పూర్ణాంకం లోపల_addr_పోర్టు_ifindex_name_గమ్యం_string(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u, uint16_t port, పూర్ణాంకం ifindex, స్థిరరాశి అక్షరం *server_name, అక్షరం **ret);
స్థిర అంతఃవరుస పూర్ణాంకం లోపల_addr_ifindex_గమ్యం_string(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u, పూర్ణాంకం ifindex, అక్షరం **ret) {
        తిరిగి లోపల_addr_పోర్టు_ifindex_name_గమ్యం_string(family, u, 0, ifindex, ఖాళీ, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం లోపల_addr_పోర్టు_గమ్యం_string(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *u, uint16_t port, అక్షరం **ret) {
        తిరిగి లోపల_addr_పోర్టు_ifindex_name_గమ్యం_string(family, u, port, 0, ఖాళీ, ret);
}
పూర్ణాంకం లోపల_addr_from_string(పూర్ణాంకం family, స్థిరరాశి అక్షరం *s, సంఘం లోపల_addr_union *ret);
పూర్ణాంకం లోపల_addr_from_string_auto(స్థిరరాశి అక్షరం *s, పూర్ణాంకం *ret_family, సంఘం లోపల_addr_union *ret);

గుర్తులేని అక్షరం in4_addr_netmask_గమ్యం_prefixlen(స్థిరరాశి నిర్మాణం in_addr *addr);
నిర్మాణం in_addr* in4_addr_prefixlen_గమ్యం_netmask(నిర్మాణం in_addr *addr, గుర్తులేని అక్షరం prefixlen);
నిర్మాణం in6_addr* in6_addr_prefixlen_గమ్యం_netmask(నిర్మాణం in6_addr *addr, గుర్తులేని అక్షరం prefixlen);
పూర్ణాంకం లోపల_addr_prefixlen_గమ్యం_netmask(పూర్ణాంకం family, సంఘం లోపల_addr_union *addr, గుర్తులేని అక్షరం prefixlen);
పూర్ణాంకం in4_addr_default_prefixlen(స్థిరరాశి నిర్మాణం in_addr *addr, గుర్తులేని అక్షరం *prefixlen);
పూర్ణాంకం in4_addr_default_subnet_ముసుగు(స్థిరరాశి నిర్మాణం in_addr *addr, నిర్మాణం in_addr *ముసుగు_3);
పూర్ణాంకం in4_addr_ముసుగు(నిర్మాణం in_addr *addr, గుర్తులేని అక్షరం prefixlen);
పూర్ణాంకం in6_addr_ముసుగు(నిర్మాణం in6_addr *addr, గుర్తులేని అక్షరం prefixlen);
పూర్ణాంకం లోపల_addr_ముసుగు(పూర్ణాంకం family, సంఘం లోపల_addr_union *addr, గుర్తులేని అక్షరం prefixlen);
పూర్ణాంకం in4_addr_ఉపసర్గ_covers_నిండిన(స్థిరరాశి నిర్మాణం in_addr *ఉపసర్గ, గుర్తులేని అక్షరం prefixlen, స్థిరరాశి నిర్మాణం in_addr *చిరునామా, గుర్తులేని అక్షరం చిరునామా_prefixlen);
పూర్ణాంకం in6_addr_ఉపసర్గ_covers_నిండిన(స్థిరరాశి నిర్మాణం in6_addr *ఉపసర్గ, గుర్తులేని అక్షరం prefixlen, స్థిరరాశి నిర్మాణం in6_addr *చిరునామా, గుర్తులేని అక్షరం చిరునామా_prefixlen);
పూర్ణాంకం లోపల_addr_ఉపసర్గ_covers_నిండిన(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *ఉపసర్గ, గుర్తులేని అక్షరం prefixlen, స్థిరరాశి సంఘం లోపల_addr_union *చిరునామా, గుర్తులేని అక్షరం చిరునామా_prefixlen);
స్థిర అంతఃవరుస పూర్ణాంకం in4_addr_ఉపసర్గ_covers(స్థిరరాశి నిర్మాణం in_addr *ఉపసర్గ, గుర్తులేని అక్షరం prefixlen, స్థిరరాశి నిర్మాణం in_addr *చిరునామా) {
        తిరిగి in4_addr_ఉపసర్గ_covers_నిండిన(ఉపసర్గ, prefixlen, చిరునామా, 32);
}
స్థిర అంతఃవరుస పూర్ణాంకం in6_addr_ఉపసర్గ_covers(స్థిరరాశి నిర్మాణం in6_addr *ఉపసర్గ, గుర్తులేని అక్షరం prefixlen, స్థిరరాశి నిర్మాణం in6_addr *చిరునామా) {
        తిరిగి in6_addr_ఉపసర్గ_covers_నిండిన(ఉపసర్గ, prefixlen, చిరునామా, 128);
}
స్థిర అంతఃవరుస పూర్ణాంకం లోపల_addr_ఉపసర్గ_covers(పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *ఉపసర్గ, గుర్తులేని అక్షరం prefixlen, స్థిరరాశి సంఘం లోపల_addr_union *చిరునామా) {
        తిరిగి లోపల_addr_ఉపసర్గ_covers_నిండిన(family, ఉపసర్గ, prefixlen, చిరునామా, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
పూర్ణాంకం లోపల_addr_parse_prefixlen(పూర్ణాంకం family, స్థిరరాశి అక్షరం *p, గుర్తులేని అక్షరం *ret);
పూర్ణాంకం లోపల_addr_ఉపసర్గ_from_string(స్థిరరాశి అక్షరం *p, పూర్ణాంకం family, సంఘం లోపల_addr_union *ret_ఉపసర్గ, గుర్తులేని అక్షరం *ret_prefixlen);

రకనిర్వచనం గణన లోపల_Addr_ఉపసర్గ_Len_బహుళకం {
        PREFIXLEN_నిండిన,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} లోపల_Addr_ఉపసర్గ_Len_బహుళకం;

పూర్ణాంకం లోపల_addr_ఉపసర్గ_from_string_auto_నిండిన(స్థిరరాశి అక్షరం *p, లోపల_Addr_ఉపసర్గ_Len_బహుళకం బహుళకం, పూర్ణాంకం *ret_family, సంఘం లోపల_addr_union *ret_ఉపసర్గ, గుర్తులేని అక్షరం *ret_prefixlen);
స్థిర అంతఃవరుస పూర్ణాంకం లోపల_addr_ఉపసర్గ_from_string_auto(స్థిరరాశి అక్షరం *p, పూర్ణాంకం *ret_family, సంఘం లోపల_addr_union *ret_ఉపసర్గ, గుర్తులేని అక్షరం *ret_prefixlen) {
        తిరిగి లోపల_addr_ఉపసర్గ_from_string_auto_నిండిన(p, PREFIXLEN_నిండిన, ret_family, ret_ఉపసర్గ, ret_prefixlen);
}

స్థిర అంతఃవరుస size_t FAMILY_చిరునామా_కొలత(పూర్ణాంకం family) {
        ఎంపిక (family) {
                స్థితి AF_INET:
                        తిరిగి 4;
                స్థితి AF_INET6:
                        తిరిగి 16;
                అప్రమేయ:
                        assert_not_reached();
        }
}

#నిర్వచించు FAMILY_చిరునామా_కొలత_సురక్షిత(f)                                     \
        ({                                                              \
                పూర్ణాంకం _f = (f);                                           \
                _f == AF_INET ? పరిమాణం(నిర్మాణం in_addr) :                \
                _f == AF_INET6 ? పరిమాణం(నిర్మాణం in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#నిర్వచించు లోపల_ADDR_NULL ((సంఘం లోపల_addr_union) { .in6 = {} })

శూన్య లోపల_addr_hash_func(స్థిరరాశి సంఘం లోపల_addr_union *u, పూర్ణాంకం family, నిర్మాణం siphash *state);
శూన్య లోపల_addr_దత్తాంశం_hash_func(స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *a, నిర్మాణం siphash *state);
పూర్ణాంకం లోపల_addr_దత్తాంశం_compare_func(స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *x, స్థిరరాశి నిర్మాణం లోపల_addr_దత్తాంశం *y);
శూన్య in6_addr_hash_func(స్థిరరాశి నిర్మాణం in6_addr *addr, నిర్మాణం siphash *state);
పూర్ణాంకం in6_addr_compare_func(స్థిరరాశి నిర్మాణం in6_addr *a, స్థిరరాశి నిర్మాణం in6_addr *b);

బాహ్య స్థిరరాశి నిర్మాణం hash_ops లోపల_addr_దత్తాంశం_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops లోపల_addr_దత్తాంశం_hash_ops_free;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops in6_addr_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops in6_addr_hash_ops_free;

స్థిర అంతఃవరుస శూన్య PTR_గమ్యం_IN4_ADDR(స్థిరరాశి శూన్య *p, నిర్మాణం in_addr *ret) {
        ప్రతిజ్ఞా(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

స్థిర అంతఃవరుస శూన్య* IN4_ADDR_గమ్యం_PTR(స్థిరరాశి నిర్మాణం in_addr *a) {
        ప్రతిజ్ఞా(a);
        తిరిగి (శూన్య*) ((uintptr_t) a->s_addr);
}

#నిర్వచించు IPV4_చిరునామా_FMT_STR     "%u.%u.%u.%u"
#నిర్వచించు IPV4_చిరునామా_FMT_VAL(చిరునామా)              \
        be32toh((చిరునామా).s_addr) >> 24,           \
        (be32toh((చిరునామా).s_addr) >> 16) & 0xFFu, \
        (be32toh((చిరునామా).s_addr) >> 8) & 0xFFu,  \
        be32toh((చిరునామా).s_addr) & 0xFFu
