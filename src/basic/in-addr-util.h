/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <netinet/in.h>

#ضمن "forward.h"

اتحاد درون_addr_union {
        بنية in_addr درون;
        بنية in6_addr in6;
        uint8_t بایتها[CONST_MAX(حجم(بنية in_addr), حجم(بنية in6_addr))];
};

بنية درون_addr_داده {
        صحيح family;
        اتحاد درون_addr_union آدرس;
};

bool in4_addr_is_null(ثابت بنية in_addr *a) خالص;
ساكن مضمن bool in4_addr_is_مجموعه(ثابت بنية in_addr *a) {
        أرجع !in4_addr_is_null(a);
}
bool in6_addr_is_null(ثابت بنية in6_addr *a) خالص;
ساكن مضمن bool in6_addr_is_مجموعه(ثابت بنية in6_addr *a) {
        أرجع !in6_addr_is_null(a);
}
صحيح درون_addr_is_null(صحيح family, ثابت اتحاد درون_addr_union *u) خالص;
ساكن مضمن bool درون_addr_is_مجموعه(صحيح family, ثابت اتحاد درون_addr_union *u) {
        أرجع درون_addr_is_null(family, u) == 0;
}
ساكن مضمن صحيح درون_addr_داده_is_null(ثابت بنية درون_addr_داده *a) {
        أكد(a);
        أرجع درون_addr_is_null(a->family, &a->آدرس);
}
ساكن مضمن bool درون_addr_داده_is_مجموعه(ثابت بنية درون_addr_داده *a) {
        أرجع درون_addr_داده_is_null(a);
}

bool in4_addr_is_multicast(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_multicast(ثابت بنية in6_addr *a) خالص;
صحيح درون_addr_is_multicast(صحيح family, ثابت اتحاد درون_addr_union *u) خالص;

bool in4_addr_is_link_local(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_link_local_dynamic(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_link_local(ثابت بنية in6_addr *a) خالص;
صحيح درون_addr_is_link_local(صحيح family, ثابت اتحاد درون_addr_union *u) خالص;
bool in6_addr_is_link_local_all_nodes(ثابت بنية in6_addr *a) خالص;

bool in4_addr_is_localhost(ثابت بنية in_addr *a) خالص;
صحيح درون_addr_is_localhost(صحيح family, ثابت اتحاد درون_addr_union *u) خالص;
صحيح درون_addr_is_localhost_one(صحيح family, ثابت اتحاد درون_addr_union *u) خالص;

bool in4_addr_is_local_multicast(ثابت بنية in_addr *a) خالص;
bool in4_addr_is_non_local(ثابت بنية in_addr *a) خالص;
bool in6_addr_is_ipv4_mapped_آدرس(ثابت بنية in6_addr *a) خالص;

bool in4_addr_equal(ثابت بنية in_addr *a, ثابت بنية in_addr *b) خالص;
bool in6_addr_equal(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b) خالص;
صحيح درون_addr_equal(صحيح family, ثابت اتحاد درون_addr_union *a, ثابت اتحاد درون_addr_union *b) خالص;
bool in4_addr_پیشوند_intersect(
                ثابت بنية in_addr *a,
                بیعلامت aprefixlen,
                ثابت بنية in_addr *b,
                بیعلامت bprefixlen) خالص;
bool in6_addr_پیشوند_intersect(
                ثابت بنية in6_addr *a,
                بیعلامت aprefixlen,
                ثابت بنية in6_addr *b,
                بیعلامت bprefixlen) خالص;
صحيح درون_addr_پیشوند_intersect(
                صحيح family,
                ثابت اتحاد درون_addr_union *a,
                بیعلامت aprefixlen,
                ثابت اتحاد درون_addr_union *b,
                بیعلامت bprefixlen) خالص;
صحيح درون_addr_random_پیشوند(صحيح family, اتحاد درون_addr_union *u, بیعلامت prefixlen_fixed_part, بیعلامت prefixlen);
صحيح درون_addr_پیشوند_range(
                صحيح family,
                ثابت اتحاد درون_addr_union *درون,
                بیعلامت prefixlen,
                اتحاد درون_addr_union *ret_شروع,
                اتحاد درون_addr_union *ret_انتها);

صحيح درون_addr_مقصد_string(صحيح family, ثابت اتحاد درون_addr_union *u, حرف **ret);
ساكن مضمن صحيح in6_addr_مقصد_string(ثابت بنية in6_addr *u, حرف **ret) {
        أرجع درون_addr_مقصد_string(AF_INET6, (ثابت اتحاد درون_addr_union*) u, ret);
}

ثابت حرف* typesafe_inet_ntop(صحيح family, ثابت اتحاد درون_addr_union *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop4(ثابت بنية in_addr *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop6(ثابت بنية in6_addr *a, حرف *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کن درون_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#تعریف_کن درون_ADDR_مقصد_STRING(family, addr) typesafe_inet_ntop(family, addr, (حرف[درون_ADDR_MAX]){}, درون_ADDR_MAX)
#تعریف_کن IN4_ADDR_مقصد_STRING(addr) typesafe_inet_ntop4(addr, (حرف[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#تعریف_کن IN6_ADDR_مقصد_STRING(addr) typesafe_inet_ntop6(addr, (حرف[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

صحيح درون_addr_پیشوند_مقصد_string(
                صحيح family,
                ثابت اتحاد درون_addr_union *u,
                بیعلامت prefixlen,
                حرف *buf,
                size_t buf_len);

ساكن مضمن ثابت حرف* درون_addr_پیشوند_مقصد_string_2(
                صحيح family,
                ثابت اتحاد درون_addr_union *u,
                بیعلامت prefixlen,
                حرف *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(درون_addr_پیشوند_مقصد_string(family, u, prefixlen, buf, buf_len) == 0);
        أرجع buf;
}
ساكن مضمن ثابت حرف* in4_addr_پیشوند_مقصد_string(ثابت بنية in_addr *a, بیعلامت prefixlen, حرف *buf, size_t buf_len) {
        أرجع درون_addr_پیشوند_مقصد_string_2(AF_INET, (ثابت اتحاد درون_addr_union *) a, prefixlen, buf, buf_len);
}
ساكن مضمن ثابت حرف* in6_addr_پیشوند_مقصد_string(ثابت بنية in6_addr *a, بیعلامت prefixlen, حرف *buf, size_t buf_len) {
        أرجع درون_addr_پیشوند_مقصد_string_2(AF_INET6, (ثابت اتحاد درون_addr_union *) a, prefixlen, buf, buf_len);
}

#تعریف_کن پیشوند_SUFFIX_MAX (1 + DECIMAL_STR_MAX(بیعلامت))
#تعریف_کن درون_ADDR_پیشوند_مقصد_STRING(family, addr, prefixlen) \
        درون_addr_پیشوند_مقصد_string_2(family, addr, prefixlen, (حرف[درون_ADDR_MAX + پیشوند_SUFFIX_MAX]){}, درون_ADDR_MAX + پیشوند_SUFFIX_MAX)
#تعریف_کن IN4_ADDR_پیشوند_مقصد_STRING(addr, prefixlen) \
        in4_addr_پیشوند_مقصد_string(addr, prefixlen, (حرف[INET_ADDRSTRLEN + پیشوند_SUFFIX_MAX]){}, INET_ADDRSTRLEN + پیشوند_SUFFIX_MAX)
#تعریف_کن IN6_ADDR_پیشوند_مقصد_STRING(addr, prefixlen) \
        in6_addr_پیشوند_مقصد_string(addr, prefixlen, (حرف[INET6_ADDRSTRLEN + پیشوند_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + پیشوند_SUFFIX_MAX)

صحيح درون_addr_پورت_ifindex_name_مقصد_string(صحيح family, ثابت اتحاد درون_addr_union *u, uint16_t port, صحيح ifindex, ثابت حرف *server_name, حرف **ret);
ساكن مضمن صحيح درون_addr_ifindex_مقصد_string(صحيح family, ثابت اتحاد درون_addr_union *u, صحيح ifindex, حرف **ret) {
        أرجع درون_addr_پورت_ifindex_name_مقصد_string(family, u, 0, ifindex, عدم, ret);
}
ساكن مضمن صحيح درون_addr_پورت_مقصد_string(صحيح family, ثابت اتحاد درون_addr_union *u, uint16_t port, حرف **ret) {
        أرجع درون_addr_پورت_ifindex_name_مقصد_string(family, u, port, 0, عدم, ret);
}
صحيح درون_addr_from_string(صحيح family, ثابت حرف *s, اتحاد درون_addr_union *ret);
صحيح درون_addr_from_string_auto(ثابت حرف *s, صحيح *ret_family, اتحاد درون_addr_union *ret);

بیعلامت حرف in4_addr_netmask_مقصد_prefixlen(ثابت بنية in_addr *addr);
بنية in_addr* in4_addr_prefixlen_مقصد_netmask(بنية in_addr *addr, بیعلامت حرف prefixlen);
بنية in6_addr* in6_addr_prefixlen_مقصد_netmask(بنية in6_addr *addr, بیعلامت حرف prefixlen);
صحيح درون_addr_prefixlen_مقصد_netmask(صحيح family, اتحاد درون_addr_union *addr, بیعلامت حرف prefixlen);
صحيح in4_addr_default_prefixlen(ثابت بنية in_addr *addr, بیعلامت حرف *prefixlen);
صحيح in4_addr_default_subnet_ماسک(ثابت بنية in_addr *addr, بنية in_addr *ماسک_3);
صحيح in4_addr_ماسک(بنية in_addr *addr, بیعلامت حرف prefixlen);
صحيح in6_addr_ماسک(بنية in6_addr *addr, بیعلامت حرف prefixlen);
صحيح درون_addr_ماسک(صحيح family, اتحاد درون_addr_union *addr, بیعلامت حرف prefixlen);
صحيح in4_addr_پیشوند_covers_پر(ثابت بنية in_addr *پیشوند, بیعلامت حرف prefixlen, ثابت بنية in_addr *آدرس, بیعلامت حرف آدرس_prefixlen);
صحيح in6_addr_پیشوند_covers_پر(ثابت بنية in6_addr *پیشوند, بیعلامت حرف prefixlen, ثابت بنية in6_addr *آدرس, بیعلامت حرف آدرس_prefixlen);
صحيح درون_addr_پیشوند_covers_پر(صحيح family, ثابت اتحاد درون_addr_union *پیشوند, بیعلامت حرف prefixlen, ثابت اتحاد درون_addr_union *آدرس, بیعلامت حرف آدرس_prefixlen);
ساكن مضمن صحيح in4_addr_پیشوند_covers(ثابت بنية in_addr *پیشوند, بیعلامت حرف prefixlen, ثابت بنية in_addr *آدرس) {
        أرجع in4_addr_پیشوند_covers_پر(پیشوند, prefixlen, آدرس, 32);
}
ساكن مضمن صحيح in6_addr_پیشوند_covers(ثابت بنية in6_addr *پیشوند, بیعلامت حرف prefixlen, ثابت بنية in6_addr *آدرس) {
        أرجع in6_addr_پیشوند_covers_پر(پیشوند, prefixlen, آدرس, 128);
}
ساكن مضمن صحيح درون_addr_پیشوند_covers(صحيح family, ثابت اتحاد درون_addr_union *پیشوند, بیعلامت حرف prefixlen, ثابت اتحاد درون_addr_union *آدرس) {
        أرجع درون_addr_پیشوند_covers_پر(family, پیشوند, prefixlen, آدرس, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
صحيح درون_addr_parse_prefixlen(صحيح family, ثابت حرف *p, بیعلامت حرف *ret);
صحيح درون_addr_پیشوند_from_string(ثابت حرف *p, صحيح family, اتحاد درون_addr_union *ret_پیشوند, بیعلامت حرف *ret_prefixlen);

تعريفنوع تعداد درون_Addr_پیشوند_Len_نما {
        PREFIXLEN_پر,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} درون_Addr_پیشوند_Len_نما;

صحيح درون_addr_پیشوند_from_string_auto_پر(ثابت حرف *p, درون_Addr_پیشوند_Len_نما نما, صحيح *ret_family, اتحاد درون_addr_union *ret_پیشوند, بیعلامت حرف *ret_prefixlen);
ساكن مضمن صحيح درون_addr_پیشوند_from_string_auto(ثابت حرف *p, صحيح *ret_family, اتحاد درون_addr_union *ret_پیشوند, بیعلامت حرف *ret_prefixlen) {
        أرجع درون_addr_پیشوند_from_string_auto_پر(p, PREFIXLEN_پر, ret_family, ret_پیشوند, ret_prefixlen);
}

ساكن مضمن size_t FAMILY_آدرس_اندازه(صحيح family) {
        بدل (family) {
                حالة AF_INET:
                        أرجع 4;
                حالة AF_INET6:
                        أرجع 16;
                افتراضي:
                        assert_not_reached();
        }
}

#تعریف_کن FAMILY_آدرس_اندازه_امن(f)                                     \
        ({                                                              \
                صحيح _f = (f);                                           \
                _f == AF_INET ? حجم(بنية in_addr) :                \
                _f == AF_INET6 ? حجم(بنية in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#تعریف_کن درون_ADDR_NULL ((اتحاد درون_addr_union) { .in6 = {} })

فراغ درون_addr_hash_func(ثابت اتحاد درون_addr_union *u, صحيح family, بنية siphash *state);
فراغ درون_addr_داده_hash_func(ثابت بنية درون_addr_داده *a, بنية siphash *state);
صحيح درون_addr_داده_compare_func(ثابت بنية درون_addr_داده *x, ثابت بنية درون_addr_داده *y);
فراغ in6_addr_hash_func(ثابت بنية in6_addr *addr, بنية siphash *state);
صحيح in6_addr_compare_func(ثابت بنية in6_addr *a, ثابت بنية in6_addr *b);

خارجي ثابت بنية hash_ops درون_addr_داده_hash_ops;
خارجي ثابت بنية hash_ops درون_addr_داده_hash_ops_free;
خارجي ثابت بنية hash_ops in6_addr_hash_ops;
خارجي ثابت بنية hash_ops in6_addr_hash_ops_free;

ساكن مضمن فراغ PTR_مقصد_IN4_ADDR(ثابت فراغ *p, بنية in_addr *ret) {
        أكد(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ساكن مضمن فراغ* IN4_ADDR_مقصد_PTR(ثابت بنية in_addr *a) {
        أكد(a);
        أرجع (فراغ*) ((uintptr_t) a->s_addr);
}

#تعریف_کن IPV4_آدرس_FMT_STR     "%u.%u.%u.%u"
#تعریف_کن IPV4_آدرس_FMT_VAL(آدرس)              \
        be32toh((آدرس).s_addr) >> 24,           \
        (be32toh((آدرس).s_addr) >> 16) & 0xFFu, \
        (be32toh((آدرس).s_addr) >> 8) & 0xFFu,  \
        be32toh((آدرس).s_addr) & 0xFFu
