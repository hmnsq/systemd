/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <netinet/in.h>

#ضمن "forward.h"

اتحاد وچ_addr_union {
        بنية in_addr وچ;
        بنية in6_addr in6;
        uint8_t بائٹاں[CONST_MAX(حجم(بنية in_addr), حجم(بنية in6_addr))];
};

بنية وچ_addr_انکڑے {
        صحيح family;
        اتحاد وچ_addr_union پتہ;
};

bool in4_addr_is_null(ثابت بنية in_addr *a) خالص;
ساكن مضمن bool in4_addr_is_سیٹ(ثابت بنية in_addr *a) {
        أرجع !in4_addr_is_null(a);
}
bool in6_addr_is_null(ثابت بنية in6_addr *a) خالص;
ساكن مضمن bool in6_addr_is_سیٹ(ثابت بنية in6_addr *a) {
        أرجع !in6_addr_is_null(a);
}
صحيح وچ_addr_is_null(صحيح family, ثابت اتحاد وچ_addr_union *u) خالص;
ساكن مضمن bool وچ_addr_is_سیٹ(صحيح family, ثابت اتحاد وچ_addr_union *u) {
        أرجع وچ_addr_is_null(family, u) == 0;
}
ساكن مضمن صحيح وچ_addr_انکڑے_is_null(ثابت بنية وچ_addr_انکڑے *a) {
        أكد(a);
        أرجع وچ_addr_is_null(a->family, &a->پتہ);
}
ساكن مضمن bool وچ_addr_انکڑے_is_سیٹ(ثابت بنية وچ_addr_انکڑے *a) {
        أرجع وچ_addr_انکڑے_is_null(a);
}

bool in4_addr_is_multicast(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_multicast(ثابت بنية in6_addr *a) خالص;
صحيح وچ_addr_is_multicast(صحيح family, ثابت اتحاد وچ_addr_union *u) خالص;

bool in4_addr_is_link_local(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_link_local_dynamic(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_link_local(ثابت بنية in6_addr *a) خالص;
صحيح وچ_addr_is_link_local(صحيح family, ثابت اتحاد وچ_addr_union *u) خالص;
bool in6_addr_is_link_local_all_nodes(ثابت بنية in6_addr *a) خالص;

bool in4_addr_is_localhost(ثابت بنية in_addr *a) خالص;
صحيح وچ_addr_is_localhost(صحيح family, ثابت اتحاد وچ_addr_union *u) خالص;
صحيح وچ_addr_is_localhost_one(صحيح family, ثابت اتحاد وچ_addr_union *u) خالص;

bool in4_addr_is_local_multicast(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_non_local(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_ipv4_mapped_پتہ(ثابت بنية in6_addr *a) خالص;

bool in4_addr_equal(ثابت بنية in_addr *a, ثابت بنية in_addr *b) خالص;
bool in6_addr_equal(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) خالص;
صحيح وچ_addr_equal(صحيح family, ثابت اتحاد وچ_addr_union *a, ثابت اتحاد وچ_addr_union *b) خالص;
bool in4_addr_اگیتر_intersect(
                ثابت بنية in_addr *a,
                بنانشانی aprefixlen,
                ثابت بنية in_addr *b,
                بنانشانی bprefixlen) خالص;
bool in6_addr_اگیتر_intersect(
                ثابت بنية in6_addr *a,
                بنانشانی aprefixlen,
                ثابت بنية in6_addr *b,
                بنانشانی bprefixlen) خالص;
صحيح وچ_addr_اگیتر_intersect(
                صحيح family,
                ثابت اتحاد وچ_addr_union *a,
                بنانشانی aprefixlen,
                ثابت اتحاد وچ_addr_union *b,
                بنانشانی bprefixlen) خالص;
صحيح وچ_addr_random_اگیتر(صحيح family, اتحاد وچ_addr_union *u, بنانشانی prefixlen_fixed_part, بنانشانی prefixlen);
صحيح وچ_addr_اگیتر_range(
                صحيح family,
                ثابت اتحاد وچ_addr_union *وچ,
                بنانشانی prefixlen,
                اتحاد وچ_addr_union *ret_شروع,
                اتحاد وچ_addr_union *ret_سرا);

صحيح وچ_addr_منزل_string(صحيح family, ثابت اتحاد وچ_addr_union *u, حرف **ret);
ساكن مضمن صحيح in6_addr_منزل_string(ثابت بنية in6_addr *u, حرف **ret) {
        أرجع وچ_addr_منزل_string(AF_INET6, (ثابت اتحاد وچ_addr_union*) u, ret);
}

ثابت حرف* typesafe_inet_ntop(صحيح family, ثابت اتحاد وچ_addr_union *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop4(ثابت بنية in_addr *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop6(ثابت بنية in6_addr *a, حرف *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو وچ_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#تعریف_کرو وچ_ADDR_منزل_STRING(family, addr) typesafe_inet_ntop(family, addr, (حرف[وچ_ADDR_MAX]){}, وچ_ADDR_MAX)
#تعریف_کرو IN4_ADDR_منزل_STRING(addr) typesafe_inet_ntop4(addr, (حرف[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#تعریف_کرو IN6_ADDR_منزل_STRING(addr) typesafe_inet_ntop6(addr, (حرف[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

صحيح وچ_addr_اگیتر_منزل_string(
                صحيح family,
                ثابت اتحاد وچ_addr_union *u,
                بنانشانی prefixlen,
                حرف *buf,
                size_t buf_len);

ساكن مضمن ثابت حرف* وچ_addr_اگیتر_منزل_string_2(
                صحيح family,
                ثابت اتحاد وچ_addr_union *u,
                بنانشانی prefixlen,
                حرف *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(وچ_addr_اگیتر_منزل_string(family, u, prefixlen, buf, buf_len) == 0);
        أرجع buf;
}
ساكن مضمن ثابت حرف* in4_addr_اگیتر_منزل_string(ثابت بنية in_addr *a, بنانشانی prefixlen, حرف *buf, size_t buf_len) {
        أرجع وچ_addr_اگیتر_منزل_string_2(AF_INET, (ثابت اتحاد وچ_addr_union *) a, prefixlen, buf, buf_len);
}
ساكن مضمن ثابت حرف* in6_addr_اگیتر_منزل_string(ثابت بنية in6_addr *a, بنانشانی prefixlen, حرف *buf, size_t buf_len) {
        أرجع وچ_addr_اگیتر_منزل_string_2(AF_INET6, (ثابت اتحاد وچ_addr_union *) a, prefixlen, buf, buf_len);
}

#تعریف_کرو اگیتر_SUFFIX_MAX (1 + DECIMAL_STR_MAX(بنانشانی))
#تعریف_کرو وچ_ADDR_اگیتر_منزل_STRING(family, addr, prefixlen) \
        وچ_addr_اگیتر_منزل_string_2(family, addr, prefixlen, (حرف[وچ_ADDR_MAX + اگیتر_SUFFIX_MAX]){}, وچ_ADDR_MAX + اگیتر_SUFFIX_MAX)
#تعریف_کرو IN4_ADDR_اگیتر_منزل_STRING(addr, prefixlen) \
        in4_addr_اگیتر_منزل_string(addr, prefixlen, (حرف[INET_ADDRSTRLEN + اگیتر_SUFFIX_MAX]){}, INET_ADDRSTRLEN + اگیتر_SUFFIX_MAX)
#تعریف_کرو IN6_ADDR_اگیتر_منزل_STRING(addr, prefixlen) \
        in6_addr_اگیتر_منزل_string(addr, prefixlen, (حرف[INET6_ADDRSTRLEN + اگیتر_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + اگیتر_SUFFIX_MAX)

صحيح وچ_addr_پورٹ_ifindex_name_منزل_string(صحيح family, ثابت اتحاد وچ_addr_union *u, uint16_t port, صحيح ifindex, ثابت حرف *server_name, حرف **ret);
ساكن مضمن صحيح وچ_addr_ifindex_منزل_string(صحيح family, ثابت اتحاد وچ_addr_union *u, صحيح ifindex, حرف **ret) {
        أرجع وچ_addr_پورٹ_ifindex_name_منزل_string(family, u, 0, ifindex, عدم, ret);
}
ساكن مضمن صحيح وچ_addr_پورٹ_منزل_string(صحيح family, ثابت اتحاد وچ_addr_union *u, uint16_t port, حرف **ret) {
        أرجع وچ_addr_پورٹ_ifindex_name_منزل_string(family, u, port, 0, عدم, ret);
}
صحيح وچ_addr_from_string(صحيح family, ثابت حرف *s, اتحاد وچ_addr_union *ret);
صحيح وچ_addr_from_string_auto(ثابت حرف *s, صحيح *ret_family, اتحاد وچ_addr_union *ret);

بنانشانی حرف in4_addr_netmask_منزل_prefixlen(ثابت بنية in_addr *addr);
بنية in_addr* in4_addr_prefixlen_منزل_netmask(بنية in_addr *addr, بنانشانی حرف prefixlen);
بنية in6_addr* in6_addr_prefixlen_منزل_netmask(بنية in6_addr *addr, بنانشانی حرف prefixlen);
صحيح وچ_addr_prefixlen_منزل_netmask(صحيح family, اتحاد وچ_addr_union *addr, بنانشانی حرف prefixlen);
صحيح in4_addr_default_prefixlen(ثابت بنية in_addr *addr, بنانشانی حرف *prefixlen);
صحيح in4_addr_default_subnet_نقاب(ثابت بنية in_addr *addr, بنية in_addr *نقاب_3);
صحيح in4_addr_نقاب(بنية in_addr *addr, بنانشانی حرف prefixlen);
صحيح in6_addr_نقاب(بنية in6_addr *addr, بنانشانی حرف prefixlen);
صحيح وچ_addr_نقاب(صحيح family, اتحاد وچ_addr_union *addr, بنانشانی حرف prefixlen);
صحيح in4_addr_اگیتر_covers_بھریا(ثابت بنية in_addr *اگیتر, بنانشانی حرف prefixlen, ثابت بنية in_addr *پتہ, بنانشانی حرف پتہ_prefixlen);
صحيح in6_addr_اگیتر_covers_بھریا(ثابت بنية in6_addr *اگیتر, بنانشانی حرف prefixlen, ثابت بنية in6_addr *پتہ, بنانشانی حرف پتہ_prefixlen);
صحيح وچ_addr_اگیتر_covers_بھریا(صحيح family, ثابت اتحاد وچ_addr_union *اگیتر, بنانشانی حرف prefixlen, ثابت اتحاد وچ_addr_union *پتہ, بنانشانی حرف پتہ_prefixlen);
ساكن مضمن صحيح in4_addr_اگیتر_covers(ثابت بنية in_addr *اگیتر, بنانشانی حرف prefixlen, ثابت بنية in_addr *پتہ) {
        أرجع in4_addr_اگیتر_covers_بھریا(اگیتر, prefixlen, پتہ, 32);
}
ساكن مضمن صحيح in6_addr_اگیتر_covers(ثابت بنية in6_addr *اگیتر, بنانشانی حرف prefixlen, ثابت بنية in6_addr *پتہ) {
        أرجع in6_addr_اگیتر_covers_بھریا(اگیتر, prefixlen, پتہ, 128);
}
ساكن مضمن صحيح وچ_addr_اگیتر_covers(صحيح family, ثابت اتحاد وچ_addr_union *اگیتر, بنانشانی حرف prefixlen, ثابت اتحاد وچ_addr_union *پتہ) {
        أرجع وچ_addr_اگیتر_covers_بھریا(family, اگیتر, prefixlen, پتہ, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
صحيح وچ_addr_parse_prefixlen(صحيح family, ثابت حرف *p, بنانشانی حرف *ret);
صحيح وچ_addr_اگیتر_from_string(ثابت حرف *p, صحيح family, اتحاد وچ_addr_union *ret_اگیتر, بنانشانی حرف *ret_prefixlen);

تعريفنوع تعداد وچ_Addr_اگیتر_Len_منوال {
        PREFIXLEN_بھریا,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} وچ_Addr_اگیتر_Len_منوال;

صحيح وچ_addr_اگیتر_from_string_auto_بھریا(ثابت حرف *p, وچ_Addr_اگیتر_Len_منوال منوال, صحيح *ret_family, اتحاد وچ_addr_union *ret_اگیتر, بنانشانی حرف *ret_prefixlen);
ساكن مضمن صحيح وچ_addr_اگیتر_from_string_auto(ثابت حرف *p, صحيح *ret_family, اتحاد وچ_addr_union *ret_اگیتر, بنانشانی حرف *ret_prefixlen) {
        أرجع وچ_addr_اگیتر_from_string_auto_بھریا(p, PREFIXLEN_بھریا, ret_family, ret_اگیتر, ret_prefixlen);
}

ساكن مضمن size_t FAMILY_پتہ_ناپ(صحيح family) {
        بدل (family) {
                حالة AF_INET:
                        أرجع 4;
                حالة AF_INET6:
                        أرجع 16;
                افتراضي:
                        assert_not_reached();
        }
}

#تعریف_کرو FAMILY_پتہ_ناپ_محفوظ(f)                                     \
        ({                                                              \
                صحيح _f = (f);                                           \
                _f == AF_INET ? حجم(بنية in_addr) :                \
                _f == AF_INET6 ? حجم(بنية in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#تعریف_کرو وچ_ADDR_NULL ((اتحاد وچ_addr_union) { .in6 = {} })

فراغ وچ_addr_hash_func(ثابت اتحاد وچ_addr_union *u, صحيح family, بنية siphash *state);
فراغ وچ_addr_انکڑے_hash_func(ثابت بنية وچ_addr_انکڑے *a, بنية siphash *state);
صحيح وچ_addr_انکڑے_compare_func(ثابت بنية وچ_addr_انکڑے *x, ثابت بنية وچ_addr_انکڑے *y);
فراغ in6_addr_hash_func(ثابت بنية in6_addr *addr, بنية siphash *state);
صحيح in6_addr_compare_func(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b);

خارجي ثابت بنية hash_ops وچ_addr_انکڑے_hash_ops;
خارجي ثابت بنية hash_ops وچ_addr_انکڑے_hash_ops_free;
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

#تعریف_کرو IPV4_پتہ_FMT_STR     "%u.%u.%u.%u"
#تعریف_کرو IPV4_پتہ_FMT_VAL(پتہ)              \
        be32toh((پتہ).s_addr) >> 24,           \
        (be32toh((پتہ).s_addr) >> 16) & 0xFFu, \
        (be32toh((پتہ).s_addr) >> 8) & 0xFFu,  \
        be32toh((پتہ).s_addr) & 0xFFu
