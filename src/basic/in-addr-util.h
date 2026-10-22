/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <netinet/in.h>

#ضمن "forward.h"

اتحاد اندريون_addr_union {
        بنية in_addr اندريون;
        بنية in6_addr in6;
        uint8_t بائيٽون[CONST_MAX(حجم(بنية in_addr), حجم(بنية in6_addr))];
};

بنية اندريون_addr_معلومات {
        صحيح family;
        اتحاد اندريون_addr_union پتو;
};

bool in4_addr_is_null(ثابت بنية in_addr *a) خالص;
ساكن مضمن bool in4_addr_is_سيٽ(ثابت بنية in_addr *a) {
        أرجع !in4_addr_is_null(a);
}
bool in6_addr_is_null(ثابت بنية in6_addr *a) خالص;
ساكن مضمن bool in6_addr_is_سيٽ(ثابت بنية in6_addr *a) {
        أرجع !in6_addr_is_null(a);
}
صحيح اندريون_addr_is_null(صحيح family, ثابت اتحاد اندريون_addr_union *u) خالص;
ساكن مضمن bool اندريون_addr_is_سيٽ(صحيح family, ثابت اتحاد اندريون_addr_union *u) {
        أرجع اندريون_addr_is_null(family, u) == 0;
}
ساكن مضمن صحيح اندريون_addr_معلومات_is_null(ثابت بنية اندريون_addr_معلومات *a) {
        أكد(a);
        أرجع اندريون_addr_is_null(a->family, &a->پتو);
}
ساكن مضمن bool اندريون_addr_معلومات_is_سيٽ(ثابت بنية اندريون_addr_معلومات *a) {
        أرجع اندريون_addr_معلومات_is_null(a);
}

bool in4_addr_is_multicast(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_multicast(ثابت بنية in6_addr *a) خالص;
صحيح اندريون_addr_is_multicast(صحيح family, ثابت اتحاد اندريون_addr_union *u) خالص;

bool in4_addr_is_link_local(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_link_local_dynamic(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_link_local(ثابت بنية in6_addr *a) خالص;
صحيح اندريون_addr_is_link_local(صحيح family, ثابت اتحاد اندريون_addr_union *u) خالص;
bool in6_addr_is_link_local_all_nodes(ثابت بنية in6_addr *a) خالص;

bool in4_addr_is_localhost(ثابت بنية in_addr *a) خالص;
صحيح اندريون_addr_is_localhost(صحيح family, ثابت اتحاد اندريون_addr_union *u) خالص;
صحيح اندريون_addr_is_localhost_one(صحيح family, ثابت اتحاد اندريون_addr_union *u) خالص;

bool in4_addr_is_local_multicast(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_non_local(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_ipv4_mapped_پتو(ثابت بنية in6_addr *a) خالص;

bool in4_addr_equal(ثابت بنية in_addr *a, ثابت بنية in_addr *b) خالص;
bool in6_addr_equal(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) خالص;
صحيح اندريون_addr_equal(صحيح family, ثابت اتحاد اندريون_addr_union *a, ثابت اتحاد اندريون_addr_union *b) خالص;
bool in4_addr_اڳياڙي_intersect(
                ثابت بنية in_addr *a,
                بنانشان aprefixlen,
                ثابت بنية in_addr *b,
                بنانشان bprefixlen) خالص;
bool in6_addr_اڳياڙي_intersect(
                ثابت بنية in6_addr *a,
                بنانشان aprefixlen,
                ثابت بنية in6_addr *b,
                بنانشان bprefixlen) خالص;
صحيح اندريون_addr_اڳياڙي_intersect(
                صحيح family,
                ثابت اتحاد اندريون_addr_union *a,
                بنانشان aprefixlen,
                ثابت اتحاد اندريون_addr_union *b,
                بنانشان bprefixlen) خالص;
صحيح اندريون_addr_random_اڳياڙي(صحيح family, اتحاد اندريون_addr_union *u, بنانشان prefixlen_fixed_part, بنانشان prefixlen);
صحيح اندريون_addr_اڳياڙي_range(
                صحيح family,
                ثابت اتحاد اندريون_addr_union *اندريون,
                بنانشان prefixlen,
                اتحاد اندريون_addr_union *ret_شروعات,
                اتحاد اندريون_addr_union *ret_پڇاڙي);

صحيح اندريون_addr_منزل_string(صحيح family, ثابت اتحاد اندريون_addr_union *u, حرف **ret);
ساكن مضمن صحيح in6_addr_منزل_string(ثابت بنية in6_addr *u, حرف **ret) {
        أرجع اندريون_addr_منزل_string(AF_INET6, (ثابت اتحاد اندريون_addr_union*) u, ret);
}

ثابت حرف* typesafe_inet_ntop(صحيح family, ثابت اتحاد اندريون_addr_union *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop4(ثابت بنية in_addr *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop6(ثابت بنية in6_addr *a, حرف *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#وصف_ڪريو اندريون_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#وصف_ڪريو اندريون_ADDR_منزل_STRING(family, addr) typesafe_inet_ntop(family, addr, (حرف[اندريون_ADDR_MAX]){}, اندريون_ADDR_MAX)
#وصف_ڪريو IN4_ADDR_منزل_STRING(addr) typesafe_inet_ntop4(addr, (حرف[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#وصف_ڪريو IN6_ADDR_منزل_STRING(addr) typesafe_inet_ntop6(addr, (حرف[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

صحيح اندريون_addr_اڳياڙي_منزل_string(
                صحيح family,
                ثابت اتحاد اندريون_addr_union *u,
                بنانشان prefixlen,
                حرف *buf,
                size_t buf_len);

ساكن مضمن ثابت حرف* اندريون_addr_اڳياڙي_منزل_string_2(
                صحيح family,
                ثابت اتحاد اندريون_addr_union *u,
                بنانشان prefixlen,
                حرف *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(اندريون_addr_اڳياڙي_منزل_string(family, u, prefixlen, buf, buf_len) == 0);
        أرجع buf;
}
ساكن مضمن ثابت حرف* in4_addr_اڳياڙي_منزل_string(ثابت بنية in_addr *a, بنانشان prefixlen, حرف *buf, size_t buf_len) {
        أرجع اندريون_addr_اڳياڙي_منزل_string_2(AF_INET, (ثابت اتحاد اندريون_addr_union *) a, prefixlen, buf, buf_len);
}
ساكن مضمن ثابت حرف* in6_addr_اڳياڙي_منزل_string(ثابت بنية in6_addr *a, بنانشان prefixlen, حرف *buf, size_t buf_len) {
        أرجع اندريون_addr_اڳياڙي_منزل_string_2(AF_INET6, (ثابت اتحاد اندريون_addr_union *) a, prefixlen, buf, buf_len);
}

#وصف_ڪريو اڳياڙي_SUFFIX_MAX (1 + DECIMAL_STR_MAX(بنانشان))
#وصف_ڪريو اندريون_ADDR_اڳياڙي_منزل_STRING(family, addr, prefixlen) \
        اندريون_addr_اڳياڙي_منزل_string_2(family, addr, prefixlen, (حرف[اندريون_ADDR_MAX + اڳياڙي_SUFFIX_MAX]){}, اندريون_ADDR_MAX + اڳياڙي_SUFFIX_MAX)
#وصف_ڪريو IN4_ADDR_اڳياڙي_منزل_STRING(addr, prefixlen) \
        in4_addr_اڳياڙي_منزل_string(addr, prefixlen, (حرف[INET_ADDRSTRLEN + اڳياڙي_SUFFIX_MAX]){}, INET_ADDRSTRLEN + اڳياڙي_SUFFIX_MAX)
#وصف_ڪريو IN6_ADDR_اڳياڙي_منزل_STRING(addr, prefixlen) \
        in6_addr_اڳياڙي_منزل_string(addr, prefixlen, (حرف[INET6_ADDRSTRLEN + اڳياڙي_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + اڳياڙي_SUFFIX_MAX)

صحيح اندريون_addr_پورٽ_ifindex_name_منزل_string(صحيح family, ثابت اتحاد اندريون_addr_union *u, uint16_t port, صحيح ifindex, ثابت حرف *server_name, حرف **ret);
ساكن مضمن صحيح اندريون_addr_ifindex_منزل_string(صحيح family, ثابت اتحاد اندريون_addr_union *u, صحيح ifindex, حرف **ret) {
        أرجع اندريون_addr_پورٽ_ifindex_name_منزل_string(family, u, 0, ifindex, عدم, ret);
}
ساكن مضمن صحيح اندريون_addr_پورٽ_منزل_string(صحيح family, ثابت اتحاد اندريون_addr_union *u, uint16_t port, حرف **ret) {
        أرجع اندريون_addr_پورٽ_ifindex_name_منزل_string(family, u, port, 0, عدم, ret);
}
صحيح اندريون_addr_from_string(صحيح family, ثابت حرف *s, اتحاد اندريون_addr_union *ret);
صحيح اندريون_addr_from_string_auto(ثابت حرف *s, صحيح *ret_family, اتحاد اندريون_addr_union *ret);

بنانشان حرف in4_addr_netmask_منزل_prefixlen(ثابت بنية in_addr *addr);
بنية in_addr* in4_addr_prefixlen_منزل_netmask(بنية in_addr *addr, بنانشان حرف prefixlen);
بنية in6_addr* in6_addr_prefixlen_منزل_netmask(بنية in6_addr *addr, بنانشان حرف prefixlen);
صحيح اندريون_addr_prefixlen_منزل_netmask(صحيح family, اتحاد اندريون_addr_union *addr, بنانشان حرف prefixlen);
صحيح in4_addr_default_prefixlen(ثابت بنية in_addr *addr, بنانشان حرف *prefixlen);
صحيح in4_addr_default_subnet_نقاب(ثابت بنية in_addr *addr, بنية in_addr *نقاب_3);
صحيح in4_addr_نقاب(بنية in_addr *addr, بنانشان حرف prefixlen);
صحيح in6_addr_نقاب(بنية in6_addr *addr, بنانشان حرف prefixlen);
صحيح اندريون_addr_نقاب(صحيح family, اتحاد اندريون_addr_union *addr, بنانشان حرف prefixlen);
صحيح in4_addr_اڳياڙي_covers_ڀريل(ثابت بنية in_addr *اڳياڙي, بنانشان حرف prefixlen, ثابت بنية in_addr *پتو, بنانشان حرف پتو_prefixlen);
صحيح in6_addr_اڳياڙي_covers_ڀريل(ثابت بنية in6_addr *اڳياڙي, بنانشان حرف prefixlen, ثابت بنية in6_addr *پتو, بنانشان حرف پتو_prefixlen);
صحيح اندريون_addr_اڳياڙي_covers_ڀريل(صحيح family, ثابت اتحاد اندريون_addr_union *اڳياڙي, بنانشان حرف prefixlen, ثابت اتحاد اندريون_addr_union *پتو, بنانشان حرف پتو_prefixlen);
ساكن مضمن صحيح in4_addr_اڳياڙي_covers(ثابت بنية in_addr *اڳياڙي, بنانشان حرف prefixlen, ثابت بنية in_addr *پتو) {
        أرجع in4_addr_اڳياڙي_covers_ڀريل(اڳياڙي, prefixlen, پتو, 32);
}
ساكن مضمن صحيح in6_addr_اڳياڙي_covers(ثابت بنية in6_addr *اڳياڙي, بنانشان حرف prefixlen, ثابت بنية in6_addr *پتو) {
        أرجع in6_addr_اڳياڙي_covers_ڀريل(اڳياڙي, prefixlen, پتو, 128);
}
ساكن مضمن صحيح اندريون_addr_اڳياڙي_covers(صحيح family, ثابت اتحاد اندريون_addr_union *اڳياڙي, بنانشان حرف prefixlen, ثابت اتحاد اندريون_addr_union *پتو) {
        أرجع اندريون_addr_اڳياڙي_covers_ڀريل(family, اڳياڙي, prefixlen, پتو, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
صحيح اندريون_addr_parse_prefixlen(صحيح family, ثابت حرف *p, بنانشان حرف *ret);
صحيح اندريون_addr_اڳياڙي_from_string(ثابت حرف *p, صحيح family, اتحاد اندريون_addr_union *ret_اڳياڙي, بنانشان حرف *ret_prefixlen);

تعريفنوع تعداد اندريون_Addr_اڳياڙي_Len_گهڻائي {
        PREFIXLEN_ڀريل,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} اندريون_Addr_اڳياڙي_Len_گهڻائي;

صحيح اندريون_addr_اڳياڙي_from_string_auto_ڀريل(ثابت حرف *p, اندريون_Addr_اڳياڙي_Len_گهڻائي گهڻائي, صحيح *ret_family, اتحاد اندريون_addr_union *ret_اڳياڙي, بنانشان حرف *ret_prefixlen);
ساكن مضمن صحيح اندريون_addr_اڳياڙي_from_string_auto(ثابت حرف *p, صحيح *ret_family, اتحاد اندريون_addr_union *ret_اڳياڙي, بنانشان حرف *ret_prefixlen) {
        أرجع اندريون_addr_اڳياڙي_from_string_auto_ڀريل(p, PREFIXLEN_ڀريل, ret_family, ret_اڳياڙي, ret_prefixlen);
}

ساكن مضمن size_t FAMILY_پتو_سائيز(صحيح family) {
        بدل (family) {
                حالة AF_INET:
                        أرجع 4;
                حالة AF_INET6:
                        أرجع 16;
                افتراضي:
                        assert_not_reached();
        }
}

#وصف_ڪريو FAMILY_پتو_سائيز_محفوظ(f)                                     \
        ({                                                              \
                صحيح _f = (f);                                           \
                _f == AF_INET ? حجم(بنية in_addr) :                \
                _f == AF_INET6 ? حجم(بنية in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#وصف_ڪريو اندريون_ADDR_NULL ((اتحاد اندريون_addr_union) { .in6 = {} })

فراغ اندريون_addr_hash_func(ثابت اتحاد اندريون_addr_union *u, صحيح family, بنية siphash *state);
فراغ اندريون_addr_معلومات_hash_func(ثابت بنية اندريون_addr_معلومات *a, بنية siphash *state);
صحيح اندريون_addr_معلومات_compare_func(ثابت بنية اندريون_addr_معلومات *x, ثابت بنية اندريون_addr_معلومات *y);
فراغ in6_addr_hash_func(ثابت بنية in6_addr *addr, بنية siphash *state);
صحيح in6_addr_compare_func(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b);

خارجي ثابت بنية hash_ops اندريون_addr_معلومات_hash_ops;
خارجي ثابت بنية hash_ops اندريون_addr_معلومات_hash_ops_free;
خارجي ثابت بنية hash_ops in6_addr_hash_ops;
خارجي ثابت بنية hash_ops in6_addr_hash_ops_free;

ساكن مضمن فراغ PTR_منزل_IN4_ADDR(ثابت فراغ *p, بنية in_addr *ret) {
        أكد(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ساكن مضمن فراغ* IN4_ADDR_منزل_PTR(ثابت بنية in_addr *a) {
        أكد(a);
        أرجع (فراغ*) ((uintptr_t) a->s_addr);
}

#وصف_ڪريو IPV4_پتو_FMT_STR     "%u.%u.%u.%u"
#وصف_ڪريو IPV4_پتو_FMT_VAL(پتو)              \
        be32toh((پتو).s_addr) >> 24,           \
        (be32toh((پتو).s_addr) >> 16) & 0xFFu, \
        (be32toh((پتو).s_addr) >> 8) & 0xFFu,  \
        be32toh((پتو).s_addr) & 0xFFu
