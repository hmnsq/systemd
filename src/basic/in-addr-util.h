/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <netinet/in.h>

#ضمن "forward.h"

اتحاد دننه_addr_union {
        بنية in_addr دننه;
        بنية in6_addr in6;
        uint8_t بایټونه[CONST_MAX(حجم(بنية in_addr), حجم(بنية in6_addr))];
};

بنية دننه_addr_معلومات {
        صحيح family;
        اتحاد دننه_addr_union پته;
};

bool in4_addr_is_null(ثابت بنية in_addr *a) خالص;
ساكن مضمن bool in4_addr_is_سېټ(ثابت بنية in_addr *a) {
        أرجع !in4_addr_is_null(a);
}
bool in6_addr_is_null(ثابت بنية in6_addr *a) خالص;
ساكن مضمن bool in6_addr_is_سېټ(ثابت بنية in6_addr *a) {
        أرجع !in6_addr_is_null(a);
}
صحيح دننه_addr_is_null(صحيح family, ثابت اتحاد دننه_addr_union *u) خالص;
ساكن مضمن bool دننه_addr_is_سېټ(صحيح family, ثابت اتحاد دننه_addr_union *u) {
        أرجع دننه_addr_is_null(family, u) == 0;
}
ساكن مضمن صحيح دننه_addr_معلومات_is_null(ثابت بنية دننه_addr_معلومات *a) {
        أكد(a);
        أرجع دننه_addr_is_null(a->family, &a->پته);
}
ساكن مضمن bool دننه_addr_معلومات_is_سېټ(ثابت بنية دننه_addr_معلومات *a) {
        أرجع دننه_addr_معلومات_is_null(a);
}

bool in4_addr_is_multicast(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_multicast(ثابت بنية in6_addr *a) خالص;
صحيح دننه_addr_is_multicast(صحيح family, ثابت اتحاد دننه_addr_union *u) خالص;

bool in4_addr_is_link_local(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_link_local_dynamic(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_link_local(ثابت بنية in6_addr *a) خالص;
صحيح دننه_addr_is_link_local(صحيح family, ثابت اتحاد دننه_addr_union *u) خالص;
bool in6_addr_is_link_local_all_nodes(ثابت بنية in6_addr *a) خالص;

bool in4_addr_is_localhost(ثابت بنية in_addr *a) خالص;
صحيح دننه_addr_is_localhost(صحيح family, ثابت اتحاد دننه_addr_union *u) خالص;
صحيح دننه_addr_is_localhost_one(صحيح family, ثابت اتحاد دننه_addr_union *u) خالص;

bool in4_addr_is_local_multicast(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_non_local(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_ipv4_mapped_پته(ثابت بنية in6_addr *a) خالص;

bool in4_addr_equal(ثابت بنية in_addr *a, ثابت بنية in_addr *b) خالص;
bool in6_addr_equal(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) خالص;
صحيح دننه_addr_equal(صحيح family, ثابت اتحاد دننه_addr_union *a, ثابت اتحاد دننه_addr_union *b) خالص;
bool in4_addr_مختاړی_intersect(
                ثابت بنية in_addr *a,
                بینښه aprefixlen,
                ثابت بنية in_addr *b,
                بینښه bprefixlen) خالص;
bool in6_addr_مختاړی_intersect(
                ثابت بنية in6_addr *a,
                بینښه aprefixlen,
                ثابت بنية in6_addr *b,
                بینښه bprefixlen) خالص;
صحيح دننه_addr_مختاړی_intersect(
                صحيح family,
                ثابت اتحاد دننه_addr_union *a,
                بینښه aprefixlen,
                ثابت اتحاد دننه_addr_union *b,
                بینښه bprefixlen) خالص;
صحيح دننه_addr_random_مختاړی(صحيح family, اتحاد دننه_addr_union *u, بینښه prefixlen_fixed_part, بینښه prefixlen);
صحيح دننه_addr_مختاړی_range(
                صحيح family,
                ثابت اتحاد دننه_addr_union *دننه,
                بینښه prefixlen,
                اتحاد دننه_addr_union *ret_پیل,
                اتحاد دننه_addr_union *ret_پای_ټکی);

صحيح دننه_addr_منزل_string(صحيح family, ثابت اتحاد دننه_addr_union *u, حرف **ret);
ساكن مضمن صحيح in6_addr_منزل_string(ثابت بنية in6_addr *u, حرف **ret) {
        أرجع دننه_addr_منزل_string(AF_INET6, (ثابت اتحاد دننه_addr_union*) u, ret);
}

ثابت حرف* typesafe_inet_ntop(صحيح family, ثابت اتحاد دننه_addr_union *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop4(ثابت بنية in_addr *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop6(ثابت بنية in6_addr *a, حرف *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کړه دننه_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#تعریف_کړه دننه_ADDR_منزل_STRING(family, addr) typesafe_inet_ntop(family, addr, (حرف[دننه_ADDR_MAX]){}, دننه_ADDR_MAX)
#تعریف_کړه IN4_ADDR_منزل_STRING(addr) typesafe_inet_ntop4(addr, (حرف[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#تعریف_کړه IN6_ADDR_منزل_STRING(addr) typesafe_inet_ntop6(addr, (حرف[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

صحيح دننه_addr_مختاړی_منزل_string(
                صحيح family,
                ثابت اتحاد دننه_addr_union *u,
                بینښه prefixlen,
                حرف *buf,
                size_t buf_len);

ساكن مضمن ثابت حرف* دننه_addr_مختاړی_منزل_string_2(
                صحيح family,
                ثابت اتحاد دننه_addr_union *u,
                بینښه prefixlen,
                حرف *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(دننه_addr_مختاړی_منزل_string(family, u, prefixlen, buf, buf_len) == 0);
        أرجع buf;
}
ساكن مضمن ثابت حرف* in4_addr_مختاړی_منزل_string(ثابت بنية in_addr *a, بینښه prefixlen, حرف *buf, size_t buf_len) {
        أرجع دننه_addr_مختاړی_منزل_string_2(AF_INET, (ثابت اتحاد دننه_addr_union *) a, prefixlen, buf, buf_len);
}
ساكن مضمن ثابت حرف* in6_addr_مختاړی_منزل_string(ثابت بنية in6_addr *a, بینښه prefixlen, حرف *buf, size_t buf_len) {
        أرجع دننه_addr_مختاړی_منزل_string_2(AF_INET6, (ثابت اتحاد دننه_addr_union *) a, prefixlen, buf, buf_len);
}

#تعریف_کړه مختاړی_SUFFIX_MAX (1 + DECIMAL_STR_MAX(بینښه))
#تعریف_کړه دننه_ADDR_مختاړی_منزل_STRING(family, addr, prefixlen) \
        دننه_addr_مختاړی_منزل_string_2(family, addr, prefixlen, (حرف[دننه_ADDR_MAX + مختاړی_SUFFIX_MAX]){}, دننه_ADDR_MAX + مختاړی_SUFFIX_MAX)
#تعریف_کړه IN4_ADDR_مختاړی_منزل_STRING(addr, prefixlen) \
        in4_addr_مختاړی_منزل_string(addr, prefixlen, (حرف[INET_ADDRSTRLEN + مختاړی_SUFFIX_MAX]){}, INET_ADDRSTRLEN + مختاړی_SUFFIX_MAX)
#تعریف_کړه IN6_ADDR_مختاړی_منزل_STRING(addr, prefixlen) \
        in6_addr_مختاړی_منزل_string(addr, prefixlen, (حرف[INET6_ADDRSTRLEN + مختاړی_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + مختاړی_SUFFIX_MAX)

صحيح دننه_addr_پورټ_ifindex_name_منزل_string(صحيح family, ثابت اتحاد دننه_addr_union *u, uint16_t port, صحيح ifindex, ثابت حرف *server_name, حرف **ret);
ساكن مضمن صحيح دننه_addr_ifindex_منزل_string(صحيح family, ثابت اتحاد دننه_addr_union *u, صحيح ifindex, حرف **ret) {
        أرجع دننه_addr_پورټ_ifindex_name_منزل_string(family, u, 0, ifindex, عدم, ret);
}
ساكن مضمن صحيح دننه_addr_پورټ_منزل_string(صحيح family, ثابت اتحاد دننه_addr_union *u, uint16_t port, حرف **ret) {
        أرجع دننه_addr_پورټ_ifindex_name_منزل_string(family, u, port, 0, عدم, ret);
}
صحيح دننه_addr_from_string(صحيح family, ثابت حرف *s, اتحاد دننه_addr_union *ret);
صحيح دننه_addr_from_string_auto(ثابت حرف *s, صحيح *ret_family, اتحاد دننه_addr_union *ret);

بینښه حرف in4_addr_netmask_منزل_prefixlen(ثابت بنية in_addr *addr);
بنية in_addr* in4_addr_prefixlen_منزل_netmask(بنية in_addr *addr, بینښه حرف prefixlen);
بنية in6_addr* in6_addr_prefixlen_منزل_netmask(بنية in6_addr *addr, بینښه حرف prefixlen);
صحيح دننه_addr_prefixlen_منزل_netmask(صحيح family, اتحاد دننه_addr_union *addr, بینښه حرف prefixlen);
صحيح in4_addr_default_prefixlen(ثابت بنية in_addr *addr, بینښه حرف *prefixlen);
صحيح in4_addr_default_subnet_ماسک(ثابت بنية in_addr *addr, بنية in_addr *ماسک_3);
صحيح in4_addr_ماسک(بنية in_addr *addr, بینښه حرف prefixlen);
صحيح in6_addr_ماسک(بنية in6_addr *addr, بینښه حرف prefixlen);
صحيح دننه_addr_ماسک(صحيح family, اتحاد دننه_addr_union *addr, بینښه حرف prefixlen);
صحيح in4_addr_مختاړی_covers_ډک(ثابت بنية in_addr *مختاړی, بینښه حرف prefixlen, ثابت بنية in_addr *پته, بینښه حرف پته_prefixlen);
صحيح in6_addr_مختاړی_covers_ډک(ثابت بنية in6_addr *مختاړی, بینښه حرف prefixlen, ثابت بنية in6_addr *پته, بینښه حرف پته_prefixlen);
صحيح دننه_addr_مختاړی_covers_ډک(صحيح family, ثابت اتحاد دننه_addr_union *مختاړی, بینښه حرف prefixlen, ثابت اتحاد دننه_addr_union *پته, بینښه حرف پته_prefixlen);
ساكن مضمن صحيح in4_addr_مختاړی_covers(ثابت بنية in_addr *مختاړی, بینښه حرف prefixlen, ثابت بنية in_addr *پته) {
        أرجع in4_addr_مختاړی_covers_ډک(مختاړی, prefixlen, پته, 32);
}
ساكن مضمن صحيح in6_addr_مختاړی_covers(ثابت بنية in6_addr *مختاړی, بینښه حرف prefixlen, ثابت بنية in6_addr *پته) {
        أرجع in6_addr_مختاړی_covers_ډک(مختاړی, prefixlen, پته, 128);
}
ساكن مضمن صحيح دننه_addr_مختاړی_covers(صحيح family, ثابت اتحاد دننه_addr_union *مختاړی, بینښه حرف prefixlen, ثابت اتحاد دننه_addr_union *پته) {
        أرجع دننه_addr_مختاړی_covers_ډک(family, مختاړی, prefixlen, پته, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
صحيح دننه_addr_parse_prefixlen(صحيح family, ثابت حرف *p, بینښه حرف *ret);
صحيح دننه_addr_مختاړی_from_string(ثابت حرف *p, صحيح family, اتحاد دننه_addr_union *ret_مختاړی, بینښه حرف *ret_prefixlen);

تعريفنوع تعداد دننه_Addr_مختاړی_Len_ډېرپېښ {
        PREFIXLEN_ډک,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} دننه_Addr_مختاړی_Len_ډېرپېښ;

صحيح دننه_addr_مختاړی_from_string_auto_ډک(ثابت حرف *p, دننه_Addr_مختاړی_Len_ډېرپېښ ډېرپېښ, صحيح *ret_family, اتحاد دننه_addr_union *ret_مختاړی, بینښه حرف *ret_prefixlen);
ساكن مضمن صحيح دننه_addr_مختاړی_from_string_auto(ثابت حرف *p, صحيح *ret_family, اتحاد دننه_addr_union *ret_مختاړی, بینښه حرف *ret_prefixlen) {
        أرجع دننه_addr_مختاړی_from_string_auto_ډک(p, PREFIXLEN_ډک, ret_family, ret_مختاړی, ret_prefixlen);
}

ساكن مضمن size_t FAMILY_پته_اندازه(صحيح family) {
        بدل (family) {
                حالة AF_INET:
                        أرجع 4;
                حالة AF_INET6:
                        أرجع 16;
                افتراضي:
                        assert_not_reached();
        }
}

#تعریف_کړه FAMILY_پته_اندازه_خوندي(f)                                     \
        ({                                                              \
                صحيح _f = (f);                                           \
                _f == AF_INET ? حجم(بنية in_addr) :                \
                _f == AF_INET6 ? حجم(بنية in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#تعریف_کړه دننه_ADDR_NULL ((اتحاد دننه_addr_union) { .in6 = {} })

فراغ دننه_addr_hash_func(ثابت اتحاد دننه_addr_union *u, صحيح family, بنية siphash *state);
فراغ دننه_addr_معلومات_hash_func(ثابت بنية دننه_addr_معلومات *a, بنية siphash *state);
صحيح دننه_addr_معلومات_compare_func(ثابت بنية دننه_addr_معلومات *x, ثابت بنية دننه_addr_معلومات *y);
فراغ in6_addr_hash_func(ثابت بنية in6_addr *addr, بنية siphash *state);
صحيح in6_addr_compare_func(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b);

خارجي ثابت بنية hash_ops دننه_addr_معلومات_hash_ops;
خارجي ثابت بنية hash_ops دننه_addr_معلومات_hash_ops_free;
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

#تعریف_کړه IPV4_پته_FMT_STR     "%u.%u.%u.%u"
#تعریف_کړه IPV4_پته_FMT_VAL(پته)              \
        be32toh((پته).s_addr) >> 24,           \
        (be32toh((پته).s_addr) >> 16) & 0xFFu, \
        (be32toh((پته).s_addr) >> 8) & 0xFFu,  \
        be32toh((پته).s_addr) & 0xFFu
