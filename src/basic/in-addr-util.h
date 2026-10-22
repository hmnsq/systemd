/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <netinet/in.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಒಕ್ಕೂಟ ಒಳ_addr_union {
        ರಚನೆ in_addr ಒಳ;
        ರಚನೆ in6_addr in6;
        uint8_t ಬೈಟುಗಳು[CONST_MAX(ಗಾತ್ರ(ರಚನೆ in_addr), ಗಾತ್ರ(ರಚನೆ in6_addr))];
};

ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ {
        ಪೂರ್ಣಾಂಕ family;
        ಒಕ್ಕೂಟ ಒಳ_addr_union ವಿಳಾಸ;
};

bool in4_addr_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool in4_addr_is_ಗಣ(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಹಿಂತಿರುಗಿ !in4_addr_is_null(a);
}
bool in6_addr_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool in6_addr_is_ಗಣ(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) {
        ಹಿಂತಿರುಗಿ !in6_addr_is_null(a);
}
ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_null(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಒಳ_addr_is_ಗಣ(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_is_null(family, u) == 0;
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಒಳ_addr_ದತ್ತಾಂಶ_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *a) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಹಿಂತಿರುಗಿ ಒಳ_addr_is_null(a->family, &a->ವಿಳಾಸ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಒಳ_addr_ದತ್ತಾಂಶ_is_ಗಣ(ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *a) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ದತ್ತಾಂಶ_is_null(a);
}

bool in4_addr_is_multicast(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) ಶುದ್ಧ;
bool in6_addr_is_multicast(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_multicast(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) ಶುದ್ಧ;

bool in4_addr_is_link_local(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) ಶುದ್ಧ;
bool in4_addr_is_link_local_dynamic(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) ಶುದ್ಧ;
bool in6_addr_is_link_local(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_link_local(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) ಶುದ್ಧ;
bool in6_addr_is_link_local_all_nodes(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) ಶುದ್ಧ;

bool in4_addr_is_localhost(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_localhost(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಒಳ_addr_is_localhost_one(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u) ಶುದ್ಧ;

bool in4_addr_is_local_multicast(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) ಶುದ್ಧ;
bool in4_addr_is_non_local(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) ಶುದ್ಧ;
bool in6_addr_is_ipv4_mapped_ವಿಳಾಸ(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a) ಶುದ್ಧ;

bool in4_addr_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *b) ಶುದ್ಧ;
bool in6_addr_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *b) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಒಳ_addr_equal(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *b) ಶುದ್ಧ;
bool in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(
                ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a,
                ಚಿಹ್ನೆರಹಿತ aprefixlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *b,
                ಚಿಹ್ನೆರಹಿತ bprefixlen) ಶುದ್ಧ;
bool in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(
                ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a,
                ಚಿಹ್ನೆರಹಿತ aprefixlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *b,
                ಚಿಹ್ನೆರಹಿತ bprefixlen) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_intersect(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a,
                ಚಿಹ್ನೆರಹಿತ aprefixlen,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *b,
                ಚಿಹ್ನೆರಹಿತ bprefixlen) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಒಳ_addr_random_ಪೂರ್ವಪ್ರತ್ಯಯ(ಪೂರ್ಣಾಂಕ family, ಒಕ್ಕೂಟ ಒಳ_addr_union *u, ಚಿಹ್ನೆರಹಿತ prefixlen_fixed_part, ಚಿಹ್ನೆರಹಿತ prefixlen);
ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_range(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ಒಳ,
                ಚಿಹ್ನೆರಹಿತ prefixlen,
                ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಆರಂಭ,
                ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಕೊನೆ);

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ in6_addr_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *u, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ಗಮ್ಯ_string(AF_INET6, (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union*) u, ret);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* typesafe_inet_ntop(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a, ಅಕ್ಷರ *buf, size_t len);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* typesafe_inet_ntop4(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a, ಅಕ್ಷರ *buf, size_t len);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* typesafe_inet_ntop6(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a, ಅಕ್ಷರ *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ADDR_ಗಮ್ಯ_STRING(family, addr) typesafe_inet_ntop(family, addr, (ಅಕ್ಷರ[ಒಳ_ADDR_MAX]){}, ಒಳ_ADDR_MAX)
#ವ್ಯಾಖ್ಯಾನಿಸು IN4_ADDR_ಗಮ್ಯ_STRING(addr) typesafe_inet_ntop4(addr, (ಅಕ್ಷರ[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#ವ್ಯಾಖ್ಯಾನಿಸು IN6_ADDR_ಗಮ್ಯ_STRING(addr) typesafe_inet_ntop6(addr, (ಅಕ್ಷರ[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u,
                ಚಿಹ್ನೆರಹಿತ prefixlen,
                ಅಕ್ಷರ *buf,
                size_t buf_len);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string_2(
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u,
                ಚಿಹ್ನೆರಹಿತ prefixlen,
                ಅಕ್ಷರ *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string(family, u, prefixlen, buf, buf_len) == 0);
        ಹಿಂತಿರುಗಿ buf;
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a, ಚಿಹ್ನೆರಹಿತ prefixlen, ಅಕ್ಷರ *buf, size_t buf_len) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string_2(AF_INET, (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *) a, prefixlen, buf, buf_len);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a, ಚಿಹ್ನೆರಹಿತ prefixlen, ಅಕ್ಷರ *buf, size_t buf_len) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string_2(AF_INET6, (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *) a, prefixlen, buf, buf_len);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಪೂರ್ವಪ್ರತ್ಯಯ_SUFFIX_MAX (1 + DECIMAL_STR_MAX(ಚಿಹ್ನೆರಹಿತ))
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ADDR_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_STRING(family, addr, prefixlen) \
        ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string_2(family, addr, prefixlen, (ಅಕ್ಷರ[ಒಳ_ADDR_MAX + ಪೂರ್ವಪ್ರತ್ಯಯ_SUFFIX_MAX]){}, ಒಳ_ADDR_MAX + ಪೂರ್ವಪ್ರತ್ಯಯ_SUFFIX_MAX)
#ವ್ಯಾಖ್ಯಾನಿಸು IN4_ADDR_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_STRING(addr, prefixlen) \
        in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string(addr, prefixlen, (ಅಕ್ಷರ[INET_ADDRSTRLEN + ಪೂರ್ವಪ್ರತ್ಯಯ_SUFFIX_MAX]){}, INET_ADDRSTRLEN + ಪೂರ್ವಪ್ರತ್ಯಯ_SUFFIX_MAX)
#ವ್ಯಾಖ್ಯಾನಿಸು IN6_ADDR_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_STRING(addr, prefixlen) \
        in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_ಗಮ್ಯ_string(addr, prefixlen, (ಅಕ್ಷರ[INET6_ADDRSTRLEN + ಪೂರ್ವಪ್ರತ್ಯಯ_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + ಪೂರ್ವಪ್ರತ್ಯಯ_SUFFIX_MAX)

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೋರ್ಟ್_ifindex_name_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, uint16_t port, ಪೂರ್ಣಾಂಕ ifindex, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *server_name, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಒಳ_addr_ifindex_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, ಪೂರ್ಣಾಂಕ ifindex, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ಪೋರ್ಟ್_ifindex_name_ಗಮ್ಯ_string(family, u, 0, ifindex, ಖಾಲಿ, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೋರ್ಟ್_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, uint16_t port, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ಪೋರ್ಟ್_ifindex_name_ಗಮ್ಯ_string(family, u, port, 0, ಖಾಲಿ, ret);
}
ಪೂರ್ಣಾಂಕ ಒಳ_addr_from_string(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಒಕ್ಕೂಟ ಒಳ_addr_union *ret);
ಪೂರ್ಣಾಂಕ ಒಳ_addr_from_string_auto(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *ret_family, ಒಕ್ಕೂಟ ಒಳ_addr_union *ret);

ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ in4_addr_netmask_ಗಮ್ಯ_prefixlen(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *addr);
ರಚನೆ in_addr* in4_addr_prefixlen_ಗಮ್ಯ_netmask(ರಚನೆ in_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen);
ರಚನೆ in6_addr* in6_addr_prefixlen_ಗಮ್ಯ_netmask(ರಚನೆ in6_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen);
ಪೂರ್ಣಾಂಕ ಒಳ_addr_prefixlen_ಗಮ್ಯ_netmask(ಪೂರ್ಣಾಂಕ family, ಒಕ್ಕೂಟ ಒಳ_addr_union *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen);
ಪೂರ್ಣಾಂಕ in4_addr_default_prefixlen(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *prefixlen);
ಪೂರ್ಣಾಂಕ in4_addr_default_subnet_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *addr, ರಚನೆ in_addr *ಮುಖವಾಡ_3);
ಪೂರ್ಣಾಂಕ in4_addr_ಮುಖವಾಡ(ರಚನೆ in_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen);
ಪೂರ್ಣಾಂಕ in6_addr_ಮುಖವಾಡ(ರಚನೆ in6_addr *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen);
ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಮುಖವಾಡ(ಪೂರ್ಣಾಂಕ family, ಒಕ್ಕೂಟ ಒಳ_addr_union *addr, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen);
ಪೂರ್ಣಾಂಕ in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen, ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *ವಿಳಾಸ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಿಳಾಸ_prefixlen);
ಪೂರ್ಣಾಂಕ in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen, ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *ವಿಳಾಸ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಿಳಾಸ_prefixlen);
ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ವಿಳಾಸ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಿಳಾಸ_prefixlen);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen, ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *ವಿಳಾಸ) {
        ಹಿಂತಿರುಗಿ in4_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(ಪೂರ್ವಪ್ರತ್ಯಯ, prefixlen, ವಿಳಾಸ, 32);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen, ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *ವಿಳಾಸ) {
        ಹಿಂತಿರುಗಿ in6_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(ಪೂರ್ವಪ್ರತ್ಯಯ, prefixlen, ವಿಳಾಸ, 128);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ prefixlen, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *ವಿಳಾಸ) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_covers_ತುಂಬಿದ(family, ಪೂರ್ವಪ್ರತ್ಯಯ, prefixlen, ವಿಳಾಸ, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
ಪೂರ್ಣಾಂಕ ಒಳ_addr_parse_prefixlen(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ret);
ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ family, ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ret_prefixlen);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಒಳ_Addr_ಪೂರ್ವಪ್ರತ್ಯಯ_Len_ಬಹುಲಕ {
        PREFIXLEN_ತುಂಬಿದ,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} ಒಳ_Addr_ಪೂರ್ವಪ್ರತ್ಯಯ_Len_ಬಹುಲಕ;

ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_from_string_auto_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಒಳ_Addr_ಪೂರ್ವಪ್ರತ್ಯಯ_Len_ಬಹುಲಕ ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ *ret_family, ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ret_prefixlen);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_from_string_auto(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ *ret_family, ಒಕ್ಕೂಟ ಒಳ_addr_union *ret_ಪೂರ್ವಪ್ರತ್ಯಯ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ret_prefixlen) {
        ಹಿಂತಿರುಗಿ ಒಳ_addr_ಪೂರ್ವಪ್ರತ್ಯಯ_from_string_auto_ತುಂಬಿದ(p, PREFIXLEN_ತುಂಬಿದ, ret_family, ret_ಪೂರ್ವಪ್ರತ್ಯಯ, ret_prefixlen);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ family) {
        ಆಯ್ಕೆ (family) {
                ಸ್ಥಿತಿ AF_INET:
                        ಹಿಂತಿರುಗಿ 4;
                ಸ್ಥಿತಿ AF_INET6:
                        ಹಿಂತಿರುಗಿ 16;
                ಪೂರ್ವನಿಯೋಜಿತ:
                        assert_not_reached();
        }
}

#ವ್ಯಾಖ್ಯಾನಿಸು FAMILY_ವಿಳಾಸ_ಗಾತ್ರ_ಸುರಕ್ಷಿತ(f)                                     \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ _f = (f);                                           \
                _f == AF_INET ? ಗಾತ್ರ(ರಚನೆ in_addr) :                \
                _f == AF_INET6 ? ಗಾತ್ರ(ರಚನೆ in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ADDR_NULL ((ಒಕ್ಕೂಟ ಒಳ_addr_union) { .in6 = {} })

ಶೂನ್ಯ ಒಳ_addr_hash_func(ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *u, ಪೂರ್ಣಾಂಕ family, ರಚನೆ siphash *state);
ಶೂನ್ಯ ಒಳ_addr_ದತ್ತಾಂಶ_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *a, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ ಒಳ_addr_ದತ್ತಾಂಶ_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *x, ಸ್ಥಿರಾಂಕ ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ *y);
ಶೂನ್ಯ in6_addr_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *addr, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ in6_addr_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ in6_addr *b);

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops ಒಳ_addr_ದತ್ತಾಂಶ_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops ಒಳ_addr_ದತ್ತಾಂಶ_hash_ops_free;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops in6_addr_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops in6_addr_hash_ops_free;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ PTR_ಗಮ್ಯ_IN4_ADDR(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, ರಚನೆ in_addr *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* IN4_ADDR_ಗಮ್ಯ_PTR(ಸ್ಥಿರಾಂಕ ರಚನೆ in_addr *a) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*) ((uintptr_t) a->s_addr);
}

#ವ್ಯಾಖ್ಯಾನಿಸು IPV4_ವಿಳಾಸ_FMT_STR     "%u.%u.%u.%u"
#ವ್ಯಾಖ್ಯಾನಿಸು IPV4_ವಿಳಾಸ_FMT_VAL(ವಿಳಾಸ)              \
        be32toh((ವಿಳಾಸ).s_addr) >> 24,           \
        (be32toh((ವಿಳಾಸ).s_addr) >> 16) & 0xFFu, \
        (be32toh((ವಿಳಾಸ).s_addr) >> 8) & 0xFFu,  \
        be32toh((ವಿಳಾಸ).s_addr) & 0xFFu
