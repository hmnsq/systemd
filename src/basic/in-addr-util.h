/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <netinet/in.h>

#شامل "forward.h"

اتحاد اندر_addr_union {
        ساخت in_addr اندر;
        ساخت in6_addr in6;
        uint8_t بائٹیں[CONST_MAX(حجم(ساخت in_addr), حجم(ساخت in6_addr))];
};

ساخت اندر_addr_ڈیٹا {
        صحیح family;
        اتحاد اندر_addr_union پتہ;
};

bool in4_addr_is_null(ثابت ساخت in_addr *a) خالص;
ساکن اندرونی bool in4_addr_is_سیٹ(ثابت ساخت in_addr *a) {
        واپس !in4_addr_is_null(a);
}
bool in6_addr_is_null(ثابت ساخت in6_addr *a) خالص;
ساکن اندرونی bool in6_addr_is_سیٹ(ثابت ساخت in6_addr *a) {
        واپس !in6_addr_is_null(a);
}
صحیح اندر_addr_is_null(صحیح family, ثابت اتحاد اندر_addr_union *u) خالص;
ساکن اندرونی bool اندر_addr_is_سیٹ(صحیح family, ثابت اتحاد اندر_addr_union *u) {
        واپس اندر_addr_is_null(family, u) == 0;
}
ساکن اندرونی صحیح اندر_addr_ڈیٹا_is_null(ثابت ساخت اندر_addr_ڈیٹا *a) {
        أكد(a);
        واپس اندر_addr_is_null(a->family, &a->پتہ);
}
ساکن اندرونی bool اندر_addr_ڈیٹا_is_سیٹ(ثابت ساخت اندر_addr_ڈیٹا *a) {
        واپس اندر_addr_ڈیٹا_is_null(a);
}

bool in4_addr_is_multicast(ثابت ساخت in_addr *a) خالص;
bool in6_addr_is_multicast(ثابت ساخت in6_addr *a) خالص;
صحیح اندر_addr_is_multicast(صحیح family, ثابت اتحاد اندر_addr_union *u) خالص;

bool in4_addr_is_link_local(ثابت ساخت in_addr *a) خالص;
bool in4_addr_is_link_local_dynamic(ثابت ساخت in_addr *a) خالص;
bool in6_addr_is_link_local(ثابت ساخت in6_addr *a) خالص;
صحیح اندر_addr_is_link_local(صحیح family, ثابت اتحاد اندر_addr_union *u) خالص;
bool in6_addr_is_link_local_all_nodes(ثابت ساخت in6_addr *a) خالص;

bool in4_addr_is_localhost(ثابت ساخت in_addr *a) خالص;
صحیح اندر_addr_is_localhost(صحیح family, ثابت اتحاد اندر_addr_union *u) خالص;
صحیح اندر_addr_is_localhost_one(صحیح family, ثابت اتحاد اندر_addr_union *u) خالص;

bool in4_addr_is_local_multicast(ثابت ساخت in_addr *a) خالص;
bool in4_addr_is_non_local(ثابت ساخت in_addr *a) خالص;
bool in6_addr_is_ipv4_mapped_پتہ(ثابت ساخت in6_addr *a) خالص;

bool in4_addr_equal(ثابت ساخت in_addr *a, ثابت ساخت in_addr *b) خالص;
bool in6_addr_equal(ثابت ساخت in6_addr *a, ثابت ساخت in6_addr *b) خالص;
صحیح اندر_addr_equal(صحیح family, ثابت اتحاد اندر_addr_union *a, ثابت اتحاد اندر_addr_union *b) خالص;
bool in4_addr_سابقہ_intersect(
                ثابت ساخت in_addr *a,
                غیرعلامتی aprefixlen,
                ثابت ساخت in_addr *b,
                غیرعلامتی bprefixlen) خالص;
bool in6_addr_سابقہ_intersect(
                ثابت ساخت in6_addr *a,
                غیرعلامتی aprefixlen,
                ثابت ساخت in6_addr *b,
                غیرعلامتی bprefixlen) خالص;
صحیح اندر_addr_سابقہ_intersect(
                صحیح family,
                ثابت اتحاد اندر_addr_union *a,
                غیرعلامتی aprefixlen,
                ثابت اتحاد اندر_addr_union *b,
                غیرعلامتی bprefixlen) خالص;
صحیح اندر_addr_random_سابقہ(صحیح family, اتحاد اندر_addr_union *u, غیرعلامتی prefixlen_fixed_part, غیرعلامتی prefixlen);
صحیح اندر_addr_سابقہ_range(
                صحیح family,
                ثابت اتحاد اندر_addr_union *اندر,
                غیرعلامتی prefixlen,
                اتحاد اندر_addr_union *ret_آغاز,
                اتحاد اندر_addr_union *ret_انتہا);

صحیح اندر_addr_مقصد_string(صحیح family, ثابت اتحاد اندر_addr_union *u, حرف **ret);
ساکن اندرونی صحیح in6_addr_مقصد_string(ثابت ساخت in6_addr *u, حرف **ret) {
        واپس اندر_addr_مقصد_string(AF_INET6, (ثابت اتحاد اندر_addr_union*) u, ret);
}

ثابت حرف* typesafe_inet_ntop(صحیح family, ثابت اتحاد اندر_addr_union *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop4(ثابت ساخت in_addr *a, حرف *buf, size_t len);
ثابت حرف* typesafe_inet_ntop6(ثابت ساخت in6_addr *a, حرف *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو اندر_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#تعریف_کرو اندر_ADDR_مقصد_STRING(family, addr) typesafe_inet_ntop(family, addr, (حرف[اندر_ADDR_MAX]){}, اندر_ADDR_MAX)
#تعریف_کرو IN4_ADDR_مقصد_STRING(addr) typesafe_inet_ntop4(addr, (حرف[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#تعریف_کرو IN6_ADDR_مقصد_STRING(addr) typesafe_inet_ntop6(addr, (حرف[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

صحیح اندر_addr_سابقہ_مقصد_string(
                صحیح family,
                ثابت اتحاد اندر_addr_union *u,
                غیرعلامتی prefixlen,
                حرف *buf,
                size_t buf_len);

ساکن اندرونی ثابت حرف* اندر_addr_سابقہ_مقصد_string_2(
                صحیح family,
                ثابت اتحاد اندر_addr_union *u,
                غیرعلامتی prefixlen,
                حرف *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(اندر_addr_سابقہ_مقصد_string(family, u, prefixlen, buf, buf_len) == 0);
        واپس buf;
}
ساکن اندرونی ثابت حرف* in4_addr_سابقہ_مقصد_string(ثابت ساخت in_addr *a, غیرعلامتی prefixlen, حرف *buf, size_t buf_len) {
        واپس اندر_addr_سابقہ_مقصد_string_2(AF_INET, (ثابت اتحاد اندر_addr_union *) a, prefixlen, buf, buf_len);
}
ساکن اندرونی ثابت حرف* in6_addr_سابقہ_مقصد_string(ثابت ساخت in6_addr *a, غیرعلامتی prefixlen, حرف *buf, size_t buf_len) {
        واپس اندر_addr_سابقہ_مقصد_string_2(AF_INET6, (ثابت اتحاد اندر_addr_union *) a, prefixlen, buf, buf_len);
}

#تعریف_کرو سابقہ_SUFFIX_MAX (1 + DECIMAL_STR_MAX(غیرعلامتی))
#تعریف_کرو اندر_ADDR_سابقہ_مقصد_STRING(family, addr, prefixlen) \
        اندر_addr_سابقہ_مقصد_string_2(family, addr, prefixlen, (حرف[اندر_ADDR_MAX + سابقہ_SUFFIX_MAX]){}, اندر_ADDR_MAX + سابقہ_SUFFIX_MAX)
#تعریف_کرو IN4_ADDR_سابقہ_مقصد_STRING(addr, prefixlen) \
        in4_addr_سابقہ_مقصد_string(addr, prefixlen, (حرف[INET_ADDRSTRLEN + سابقہ_SUFFIX_MAX]){}, INET_ADDRSTRLEN + سابقہ_SUFFIX_MAX)
#تعریف_کرو IN6_ADDR_سابقہ_مقصد_STRING(addr, prefixlen) \
        in6_addr_سابقہ_مقصد_string(addr, prefixlen, (حرف[INET6_ADDRSTRLEN + سابقہ_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + سابقہ_SUFFIX_MAX)

صحیح اندر_addr_پورٹ_ifindex_name_مقصد_string(صحیح family, ثابت اتحاد اندر_addr_union *u, uint16_t port, صحیح ifindex, ثابت حرف *server_name, حرف **ret);
ساکن اندرونی صحیح اندر_addr_ifindex_مقصد_string(صحیح family, ثابت اتحاد اندر_addr_union *u, صحیح ifindex, حرف **ret) {
        واپس اندر_addr_پورٹ_ifindex_name_مقصد_string(family, u, 0, ifindex, تہی, ret);
}
ساکن اندرونی صحیح اندر_addr_پورٹ_مقصد_string(صحیح family, ثابت اتحاد اندر_addr_union *u, uint16_t port, حرف **ret) {
        واپس اندر_addr_پورٹ_ifindex_name_مقصد_string(family, u, port, 0, تہی, ret);
}
صحیح اندر_addr_from_string(صحیح family, ثابت حرف *s, اتحاد اندر_addr_union *ret);
صحیح اندر_addr_from_string_auto(ثابت حرف *s, صحیح *ret_family, اتحاد اندر_addr_union *ret);

غیرعلامتی حرف in4_addr_netmask_مقصد_prefixlen(ثابت ساخت in_addr *addr);
ساخت in_addr* in4_addr_prefixlen_مقصد_netmask(ساخت in_addr *addr, غیرعلامتی حرف prefixlen);
ساخت in6_addr* in6_addr_prefixlen_مقصد_netmask(ساخت in6_addr *addr, غیرعلامتی حرف prefixlen);
صحیح اندر_addr_prefixlen_مقصد_netmask(صحیح family, اتحاد اندر_addr_union *addr, غیرعلامتی حرف prefixlen);
صحیح in4_addr_default_prefixlen(ثابت ساخت in_addr *addr, غیرعلامتی حرف *prefixlen);
صحیح in4_addr_default_subnet_نقاب(ثابت ساخت in_addr *addr, ساخت in_addr *نقاب_3);
صحیح in4_addr_نقاب(ساخت in_addr *addr, غیرعلامتی حرف prefixlen);
صحیح in6_addr_نقاب(ساخت in6_addr *addr, غیرعلامتی حرف prefixlen);
صحیح اندر_addr_نقاب(صحیح family, اتحاد اندر_addr_union *addr, غیرعلامتی حرف prefixlen);
صحیح in4_addr_سابقہ_covers_بھرا(ثابت ساخت in_addr *سابقہ, غیرعلامتی حرف prefixlen, ثابت ساخت in_addr *پتہ, غیرعلامتی حرف پتہ_prefixlen);
صحیح in6_addr_سابقہ_covers_بھرا(ثابت ساخت in6_addr *سابقہ, غیرعلامتی حرف prefixlen, ثابت ساخت in6_addr *پتہ, غیرعلامتی حرف پتہ_prefixlen);
صحیح اندر_addr_سابقہ_covers_بھرا(صحیح family, ثابت اتحاد اندر_addr_union *سابقہ, غیرعلامتی حرف prefixlen, ثابت اتحاد اندر_addr_union *پتہ, غیرعلامتی حرف پتہ_prefixlen);
ساکن اندرونی صحیح in4_addr_سابقہ_covers(ثابت ساخت in_addr *سابقہ, غیرعلامتی حرف prefixlen, ثابت ساخت in_addr *پتہ) {
        واپس in4_addr_سابقہ_covers_بھرا(سابقہ, prefixlen, پتہ, 32);
}
ساکن اندرونی صحیح in6_addr_سابقہ_covers(ثابت ساخت in6_addr *سابقہ, غیرعلامتی حرف prefixlen, ثابت ساخت in6_addr *پتہ) {
        واپس in6_addr_سابقہ_covers_بھرا(سابقہ, prefixlen, پتہ, 128);
}
ساکن اندرونی صحیح اندر_addr_سابقہ_covers(صحیح family, ثابت اتحاد اندر_addr_union *سابقہ, غیرعلامتی حرف prefixlen, ثابت اتحاد اندر_addr_union *پتہ) {
        واپس اندر_addr_سابقہ_covers_بھرا(family, سابقہ, prefixlen, پتہ, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
صحیح اندر_addr_parse_prefixlen(صحیح family, ثابت حرف *p, غیرعلامتی حرف *ret);
صحیح اندر_addr_سابقہ_from_string(ثابت حرف *p, صحیح family, اتحاد اندر_addr_union *ret_سابقہ, غیرعلامتی حرف *ret_prefixlen);

قسمتعریف شمار اندر_Addr_سابقہ_Len_منوال {
        PREFIXLEN_بھرا,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} اندر_Addr_سابقہ_Len_منوال;

صحیح اندر_addr_سابقہ_from_string_auto_بھرا(ثابت حرف *p, اندر_Addr_سابقہ_Len_منوال منوال, صحیح *ret_family, اتحاد اندر_addr_union *ret_سابقہ, غیرعلامتی حرف *ret_prefixlen);
ساکن اندرونی صحیح اندر_addr_سابقہ_from_string_auto(ثابت حرف *p, صحیح *ret_family, اتحاد اندر_addr_union *ret_سابقہ, غیرعلامتی حرف *ret_prefixlen) {
        واپس اندر_addr_سابقہ_from_string_auto_بھرا(p, PREFIXLEN_بھرا, ret_family, ret_سابقہ, ret_prefixlen);
}

ساکن اندرونی size_t FAMILY_پتہ_سائز(صحیح family) {
        انتخاب (family) {
                حالت AF_INET:
                        واپس 4;
                حالت AF_INET6:
                        واپس 16;
                طےشدہ:
                        assert_not_reached();
        }
}

#تعریف_کرو FAMILY_پتہ_سائز_محفوظ(f)                                     \
        ({                                                              \
                صحیح _f = (f);                                           \
                _f == AF_INET ? حجم(ساخت in_addr) :                \
                _f == AF_INET6 ? حجم(ساخت in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#تعریف_کرو اندر_ADDR_NULL ((اتحاد اندر_addr_union) { .in6 = {} })

خالی اندر_addr_hash_func(ثابت اتحاد اندر_addr_union *u, صحیح family, ساخت siphash *state);
خالی اندر_addr_ڈیٹا_hash_func(ثابت ساخت اندر_addr_ڈیٹا *a, ساخت siphash *state);
صحیح اندر_addr_ڈیٹا_compare_func(ثابت ساخت اندر_addr_ڈیٹا *x, ثابت ساخت اندر_addr_ڈیٹا *y);
خالی in6_addr_hash_func(ثابت ساخت in6_addr *addr, ساخت siphash *state);
صحیح in6_addr_compare_func(ثابت ساخت in6_addr *a, ثابت ساخت in6_addr *b);

خارجی ثابت ساخت hash_ops اندر_addr_ڈیٹا_hash_ops;
خارجی ثابت ساخت hash_ops اندر_addr_ڈیٹا_hash_ops_free;
خارجی ثابت ساخت hash_ops in6_addr_hash_ops;
خارجی ثابت ساخت hash_ops in6_addr_hash_ops_free;

ساکن اندرونی خالی PTR_مقصد_IN4_ADDR(ثابت خالی *p, ساخت in_addr *ret) {
        أكد(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

ساکن اندرونی خالی* IN4_ADDR_مقصد_PTR(ثابت ساخت in_addr *a) {
        أكد(a);
        واپس (خالی*) ((uintptr_t) a->s_addr);
}

#تعریف_کرو IPV4_پتہ_FMT_STR     "%u.%u.%u.%u"
#تعریف_کرو IPV4_پتہ_FMT_VAL(پتہ)              \
        be32toh((پتہ).s_addr) >> 24,           \
        (be32toh((پتہ).s_addr) >> 16) & 0xFFu, \
        (be32toh((پتہ).s_addr) >> 8) & 0xFFu,  \
        be32toh((پتہ).s_addr) & 0xFFu
