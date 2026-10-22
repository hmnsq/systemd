/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਸੰਘ ਵਿੱਚ_addr_union {
        ਬਣਤਰ in_addr ਵਿੱਚ;
        ਬਣਤਰ in6_addr in6;
        uint8_t ਬਾਈਟਾਂ[CONST_MAX(ਆਕਾਰ(ਬਣਤਰ in_addr), ਆਕਾਰ(ਬਣਤਰ in6_addr))];
};

ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ {
        ਪੂਰਨ family;
        ਸੰਘ ਵਿੱਚ_addr_union ਪਤਾ;
};

bool in4_addr_is_null(ਅਚਲ ਬਣਤਰ in_addr *a) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool in4_addr_is_ਸੈੱਟ(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਵਾਪਸ !in4_addr_is_null(a);
}
bool in6_addr_is_null(ਅਚਲ ਬਣਤਰ in6_addr *a) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool in6_addr_is_ਸੈੱਟ(ਅਚਲ ਬਣਤਰ in6_addr *a) {
        ਵਾਪਸ !in6_addr_is_null(a);
}
ਪੂਰਨ ਵਿੱਚ_addr_is_null(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਵਿੱਚ_addr_is_ਸੈੱਟ(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) {
        ਵਾਪਸ ਵਿੱਚ_addr_is_null(family, u) == 0;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਵਿੱਚ_addr_ਅੰਕੜੇ_is_null(ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਵਾਪਸ ਵਿੱਚ_addr_is_null(a->family, &a->ਪਤਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਵਿੱਚ_addr_ਅੰਕੜੇ_is_ਸੈੱਟ(ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *a) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਅੰਕੜੇ_is_null(a);
}

bool in4_addr_is_multicast(ਅਚਲ ਬਣਤਰ in_addr *a) ਸ਼ੁੱਧ;
bool in6_addr_is_multicast(ਅਚਲ ਬਣਤਰ in6_addr *a) ਸ਼ੁੱਧ;
ਪੂਰਨ ਵਿੱਚ_addr_is_multicast(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) ਸ਼ੁੱਧ;

bool in4_addr_is_link_local(ਅਚਲ ਬਣਤਰ in_addr *a) ਸ਼ੁੱਧ;
bool in4_addr_is_link_local_dynamic(ਅਚਲ ਬਣਤਰ in_addr *a) ਸ਼ੁੱਧ;
bool in6_addr_is_link_local(ਅਚਲ ਬਣਤਰ in6_addr *a) ਸ਼ੁੱਧ;
ਪੂਰਨ ਵਿੱਚ_addr_is_link_local(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) ਸ਼ੁੱਧ;
bool in6_addr_is_link_local_all_nodes(ਅਚਲ ਬਣਤਰ in6_addr *a) ਸ਼ੁੱਧ;

bool in4_addr_is_localhost(ਅਚਲ ਬਣਤਰ in_addr *a) ਸ਼ੁੱਧ;
ਪੂਰਨ ਵਿੱਚ_addr_is_localhost(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) ਸ਼ੁੱਧ;
ਪੂਰਨ ਵਿੱਚ_addr_is_localhost_one(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u) ਸ਼ੁੱਧ;

bool in4_addr_is_local_multicast(ਅਚਲ ਬਣਤਰ in_addr *a) ਸ਼ੁੱਧ;
bool in4_addr_is_non_local(ਅਚਲ ਬਣਤਰ in_addr *a) ਸ਼ੁੱਧ;
bool in6_addr_is_ipv4_mapped_ਪਤਾ(ਅਚਲ ਬਣਤਰ in6_addr *a) ਸ਼ੁੱਧ;

bool in4_addr_equal(ਅਚਲ ਬਣਤਰ in_addr *a, ਅਚਲ ਬਣਤਰ in_addr *b) ਸ਼ੁੱਧ;
bool in6_addr_equal(ਅਚਲ ਬਣਤਰ in6_addr *a, ਅਚਲ ਬਣਤਰ in6_addr *b) ਸ਼ੁੱਧ;
ਪੂਰਨ ਵਿੱਚ_addr_equal(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *b) ਸ਼ੁੱਧ;
bool in4_addr_ਅਗੇਤਰ_intersect(
                ਅਚਲ ਬਣਤਰ in_addr *a,
                ਬਿਨਾਨਿਸ਼ਾਨੀ aprefixlen,
                ਅਚਲ ਬਣਤਰ in_addr *b,
                ਬਿਨਾਨਿਸ਼ਾਨੀ bprefixlen) ਸ਼ੁੱਧ;
bool in6_addr_ਅਗੇਤਰ_intersect(
                ਅਚਲ ਬਣਤਰ in6_addr *a,
                ਬਿਨਾਨਿਸ਼ਾਨੀ aprefixlen,
                ਅਚਲ ਬਣਤਰ in6_addr *b,
                ਬਿਨਾਨਿਸ਼ਾਨੀ bprefixlen) ਸ਼ੁੱਧ;
ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_intersect(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a,
                ਬਿਨਾਨਿਸ਼ਾਨੀ aprefixlen,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *b,
                ਬਿਨਾਨਿਸ਼ਾਨੀ bprefixlen) ਸ਼ੁੱਧ;
ਪੂਰਨ ਵਿੱਚ_addr_random_ਅਗੇਤਰ(ਪੂਰਨ family, ਸੰਘ ਵਿੱਚ_addr_union *u, ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen_fixed_part, ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen);
ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_range(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਵਿੱਚ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen,
                ਸੰਘ ਵਿੱਚ_addr_union *ret_ਸ਼ੁਰੂ,
                ਸੰਘ ਵਿੱਚ_addr_union *ret_ਸਿਰਾ);

ਪੂਰਨ ਵਿੱਚ_addr_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ in6_addr_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਬਣਤਰ in6_addr *u, ਅੱਖਰ **ret) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਮੰਜ਼ਿਲ_string(AF_INET6, (ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union*) u, ret);
}

ਅਚਲ ਅੱਖਰ* typesafe_inet_ntop(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a, ਅੱਖਰ *buf, size_t len);
ਅਚਲ ਅੱਖਰ* typesafe_inet_ntop4(ਅਚਲ ਬਣਤਰ in_addr *a, ਅੱਖਰ *buf, size_t len);
ਅਚਲ ਅੱਖਰ* typesafe_inet_ntop6(ਅਚਲ ਬਣਤਰ in6_addr *a, ਅੱਖਰ *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ADDR_ਮੰਜ਼ਿਲ_STRING(family, addr) typesafe_inet_ntop(family, addr, (ਅੱਖਰ[ਵਿੱਚ_ADDR_MAX]){}, ਵਿੱਚ_ADDR_MAX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IN4_ADDR_ਮੰਜ਼ਿਲ_STRING(addr) typesafe_inet_ntop4(addr, (ਅੱਖਰ[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IN6_ADDR_ਮੰਜ਼ਿਲ_STRING(addr) typesafe_inet_ntop6(addr, (ਅੱਖਰ[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u,
                ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen,
                ਅੱਖਰ *buf,
                size_t buf_len);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ਵਿੱਚ_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string_2(
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u,
                ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen,
                ਅੱਖਰ *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(ਵਿੱਚ_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string(family, u, prefixlen, buf, buf_len) == 0);
        ਵਾਪਸ buf;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* in4_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਬਣਤਰ in_addr *a, ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen, ਅੱਖਰ *buf, size_t buf_len) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string_2(AF_INET, (ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *) a, prefixlen, buf, buf_len);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* in6_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਬਣਤਰ in6_addr *a, ਬਿਨਾਨਿਸ਼ਾਨੀ prefixlen, ਅੱਖਰ *buf, size_t buf_len) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string_2(AF_INET6, (ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *) a, prefixlen, buf, buf_len);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਗੇਤਰ_SUFFIX_MAX (1 + DECIMAL_STR_MAX(ਬਿਨਾਨਿਸ਼ਾਨੀ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ADDR_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_STRING(family, addr, prefixlen) \
        ਵਿੱਚ_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string_2(family, addr, prefixlen, (ਅੱਖਰ[ਵਿੱਚ_ADDR_MAX + ਅਗੇਤਰ_SUFFIX_MAX]){}, ਵਿੱਚ_ADDR_MAX + ਅਗੇਤਰ_SUFFIX_MAX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IN4_ADDR_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_STRING(addr, prefixlen) \
        in4_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string(addr, prefixlen, (ਅੱਖਰ[INET_ADDRSTRLEN + ਅਗੇਤਰ_SUFFIX_MAX]){}, INET_ADDRSTRLEN + ਅਗੇਤਰ_SUFFIX_MAX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IN6_ADDR_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_STRING(addr, prefixlen) \
        in6_addr_ਅਗੇਤਰ_ਮੰਜ਼ਿਲ_string(addr, prefixlen, (ਅੱਖਰ[INET6_ADDRSTRLEN + ਅਗੇਤਰ_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + ਅਗੇਤਰ_SUFFIX_MAX)

ਪੂਰਨ ਵਿੱਚ_addr_ਪੋਰਟ_ifindex_name_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, uint16_t port, ਪੂਰਨ ifindex, ਅਚਲ ਅੱਖਰ *server_name, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਵਿੱਚ_addr_ifindex_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, ਪੂਰਨ ifindex, ਅੱਖਰ **ret) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਪੋਰਟ_ifindex_name_ਮੰਜ਼ਿਲ_string(family, u, 0, ifindex, ਨਿਰਾ, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਵਿੱਚ_addr_ਪੋਰਟ_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, uint16_t port, ਅੱਖਰ **ret) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਪੋਰਟ_ifindex_name_ਮੰਜ਼ਿਲ_string(family, u, port, 0, ਨਿਰਾ, ret);
}
ਪੂਰਨ ਵਿੱਚ_addr_from_string(ਪੂਰਨ family, ਅਚਲ ਅੱਖਰ *s, ਸੰਘ ਵਿੱਚ_addr_union *ret);
ਪੂਰਨ ਵਿੱਚ_addr_from_string_auto(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret_family, ਸੰਘ ਵਿੱਚ_addr_union *ret);

ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ in4_addr_netmask_ਮੰਜ਼ਿਲ_prefixlen(ਅਚਲ ਬਣਤਰ in_addr *addr);
ਬਣਤਰ in_addr* in4_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(ਬਣਤਰ in_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen);
ਬਣਤਰ in6_addr* in6_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(ਬਣਤਰ in6_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen);
ਪੂਰਨ ਵਿੱਚ_addr_prefixlen_ਮੰਜ਼ਿਲ_netmask(ਪੂਰਨ family, ਸੰਘ ਵਿੱਚ_addr_union *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen);
ਪੂਰਨ in4_addr_default_prefixlen(ਅਚਲ ਬਣਤਰ in_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *prefixlen);
ਪੂਰਨ in4_addr_default_subnet_ਨਕਾਬ(ਅਚਲ ਬਣਤਰ in_addr *addr, ਬਣਤਰ in_addr *ਨਕਾਬ_3);
ਪੂਰਨ in4_addr_ਨਕਾਬ(ਬਣਤਰ in_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen);
ਪੂਰਨ in6_addr_ਨਕਾਬ(ਬਣਤਰ in6_addr *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen);
ਪੂਰਨ ਵਿੱਚ_addr_ਨਕਾਬ(ਪੂਰਨ family, ਸੰਘ ਵਿੱਚ_addr_union *addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen);
ਪੂਰਨ in4_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(ਅਚਲ ਬਣਤਰ in_addr *ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen, ਅਚਲ ਬਣਤਰ in_addr *ਪਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪਤਾ_prefixlen);
ਪੂਰਨ in6_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(ਅਚਲ ਬਣਤਰ in6_addr *ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen, ਅਚਲ ਬਣਤਰ in6_addr *ਪਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪਤਾ_prefixlen);
ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਪਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪਤਾ_prefixlen);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ in4_addr_ਅਗੇਤਰ_covers(ਅਚਲ ਬਣਤਰ in_addr *ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen, ਅਚਲ ਬਣਤਰ in_addr *ਪਤਾ) {
        ਵਾਪਸ in4_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(ਅਗੇਤਰ, prefixlen, ਪਤਾ, 32);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ in6_addr_ਅਗੇਤਰ_covers(ਅਚਲ ਬਣਤਰ in6_addr *ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen, ਅਚਲ ਬਣਤਰ in6_addr *ਪਤਾ) {
        ਵਾਪਸ in6_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(ਅਗੇਤਰ, prefixlen, ਪਤਾ, 128);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_covers(ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ prefixlen, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *ਪਤਾ) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਅਗੇਤਰ_covers_ਭਰਿਆ(family, ਅਗੇਤਰ, prefixlen, ਪਤਾ, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
ਪੂਰਨ ਵਿੱਚ_addr_parse_prefixlen(ਪੂਰਨ family, ਅਚਲ ਅੱਖਰ *p, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ret);
ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_from_string(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ family, ਸੰਘ ਵਿੱਚ_addr_union *ret_ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ret_prefixlen);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਵਿੱਚ_Addr_ਅਗੇਤਰ_Len_ਬਹੁਲਕ {
        PREFIXLEN_ਭਰਿਆ,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} ਵਿੱਚ_Addr_ਅਗੇਤਰ_Len_ਬਹੁਲਕ;

ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_from_string_auto_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *p, ਵਿੱਚ_Addr_ਅਗੇਤਰ_Len_ਬਹੁਲਕ ਬਹੁਲਕ, ਪੂਰਨ *ret_family, ਸੰਘ ਵਿੱਚ_addr_union *ret_ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ret_prefixlen);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਵਿੱਚ_addr_ਅਗੇਤਰ_from_string_auto(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ *ret_family, ਸੰਘ ਵਿੱਚ_addr_union *ret_ਅਗੇਤਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ret_prefixlen) {
        ਵਾਪਸ ਵਿੱਚ_addr_ਅਗੇਤਰ_from_string_auto_ਭਰਿਆ(p, PREFIXLEN_ਭਰਿਆ, ret_family, ret_ਅਗੇਤਰ, ret_prefixlen);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ size_t FAMILY_ਪਤਾ_ਮਾਪ(ਪੂਰਨ family) {
        ਚੋਣ (family) {
                ਹਾਲਤ AF_INET:
                        ਵਾਪਸ 4;
                ਹਾਲਤ AF_INET6:
                        ਵਾਪਸ 16;
                ਮੂਲ:
                        assert_not_reached();
        }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FAMILY_ਪਤਾ_ਮਾਪ_ਸੁਰੱਖਿਅਤ(f)                                     \
        ({                                                              \
                ਪੂਰਨ _f = (f);                                           \
                _f == AF_INET ? ਆਕਾਰ(ਬਣਤਰ in_addr) :                \
                _f == AF_INET6 ? ਆਕਾਰ(ਬਣਤਰ in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ADDR_NULL ((ਸੰਘ ਵਿੱਚ_addr_union) { .in6 = {} })

ਖਾਲੀ ਵਿੱਚ_addr_hash_func(ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *u, ਪੂਰਨ family, ਬਣਤਰ siphash *state);
ਖਾਲੀ ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_func(ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *a, ਬਣਤਰ siphash *state);
ਪੂਰਨ ਵਿੱਚ_addr_ਅੰਕੜੇ_compare_func(ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *x, ਅਚਲ ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ *y);
ਖਾਲੀ in6_addr_hash_func(ਅਚਲ ਬਣਤਰ in6_addr *addr, ਬਣਤਰ siphash *state);
ਪੂਰਨ in6_addr_compare_func(ਅਚਲ ਬਣਤਰ in6_addr *a, ਅਚਲ ਬਣਤਰ in6_addr *b);

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops ਵਿੱਚ_addr_ਅੰਕੜੇ_hash_ops_free;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops in6_addr_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops in6_addr_hash_ops_free;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ PTR_ਮੰਜ਼ਿਲ_IN4_ADDR(ਅਚਲ ਖਾਲੀ *p, ਬਣਤਰ in_addr *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* IN4_ADDR_ਮੰਜ਼ਿਲ_PTR(ਅਚਲ ਬਣਤਰ in_addr *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਵਾਪਸ (ਖਾਲੀ*) ((uintptr_t) a->s_addr);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV4_ਪਤਾ_FMT_STR     "%u.%u.%u.%u"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV4_ਪਤਾ_FMT_VAL(ਪਤਾ)              \
        be32toh((ਪਤਾ).s_addr) >> 24,           \
        (be32toh((ਪਤਾ).s_addr) >> 16) & 0xFFu, \
        (be32toh((ਪਤਾ).s_addr) >> 8) & 0xFFu,  \
        be32toh((ਪਤਾ).s_addr) & 0xFFu
