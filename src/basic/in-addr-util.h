/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <netinet/in.h>

#अन्तर्भाव "forward.h"

सङ्घ अन्तर्_addr_union {
        संरचना in_addr अन्तर्;
        संरचना in6_addr in6;
        uint8_t अष्टकानि[CONST_MAX(परिमाण(संरचना in_addr), परिमाण(संरचना in6_addr))];
};

संरचना अन्तर्_addr_दत्त {
        पूर्णाङ्क family;
        सङ्घ अन्तर्_addr_union स्थानाङ्कः;
};

bool in4_addr_is_null(ध्रुव संरचना in_addr *a) विशुद्धम्;
स्थिर अन्तःस्थ bool in4_addr_is_समुच्चयः(ध्रुव संरचना in_addr *a) {
        प्रत्यागम !in4_addr_is_null(a);
}
bool in6_addr_is_null(ध्रुव संरचना in6_addr *a) विशुद्धम्;
स्थिर अन्तःस्थ bool in6_addr_is_समुच्चयः(ध्रुव संरचना in6_addr *a) {
        प्रत्यागम !in6_addr_is_null(a);
}
पूर्णाङ्क अन्तर्_addr_is_null(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u) विशुद्धम्;
स्थिर अन्तःस्थ bool अन्तर्_addr_is_समुच्चयः(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u) {
        प्रत्यागम अन्तर्_addr_is_null(family, u) == 0;
}
स्थिर अन्तःस्थ पूर्णाङ्क अन्तर्_addr_दत्त_is_null(ध्रुव संरचना अन्तर्_addr_दत्त *a) {
        प्रतिज्ञा(a);
        प्रत्यागम अन्तर्_addr_is_null(a->family, &a->स्थानाङ्कः);
}
स्थिर अन्तःस्थ bool अन्तर्_addr_दत्त_is_समुच्चयः(ध्रुव संरचना अन्तर्_addr_दत्त *a) {
        प्रत्यागम अन्तर्_addr_दत्त_is_null(a);
}

bool in4_addr_is_multicast(ध्रुव संरचना in_addr *a) विशुद्धम्;
bool in6_addr_is_multicast(ध्रुव संरचना in6_addr *a) विशुद्धम्;
पूर्णाङ्क अन्तर्_addr_is_multicast(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u) विशुद्धम्;

bool in4_addr_is_link_local(ध्रुव संरचना in_addr *a) विशुद्धम्;
bool in4_addr_is_link_local_dynamic(ध्रुव संरचना in_addr *a) विशुद्धम्;
bool in6_addr_is_link_local(ध्रुव संरचना in6_addr *a) विशुद्धम्;
पूर्णाङ्क अन्तर्_addr_is_link_local(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u) विशुद्धम्;
bool in6_addr_is_link_local_all_nodes(ध्रुव संरचना in6_addr *a) विशुद्धम्;

bool in4_addr_is_localhost(ध्रुव संरचना in_addr *a) विशुद्धम्;
पूर्णाङ्क अन्तर्_addr_is_localhost(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u) विशुद्धम्;
पूर्णाङ्क अन्तर्_addr_is_localhost_one(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u) विशुद्धम्;

bool in4_addr_is_local_multicast(ध्रुव संरचना in_addr *a) विशुद्धम्;
bool in4_addr_is_non_local(ध्रुव संरचना in_addr *a) विशुद्धम्;
bool in6_addr_is_ipv4_mapped_स्थानाङ्कः(ध्रुव संरचना in6_addr *a) विशुद्धम्;

bool in4_addr_equal(ध्रुव संरचना in_addr *a, ध्रुव संरचना in_addr *b) विशुद्धम्;
bool in6_addr_equal(ध्रुव संरचना in6_addr *a, ध्रुव संरचना in6_addr *b) विशुद्धम्;
पूर्णाङ्क अन्तर्_addr_equal(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *a, ध्रुव सङ्घ अन्तर्_addr_union *b) विशुद्धम्;
bool in4_addr_उपसर्गः_intersect(
                ध्रुव संरचना in_addr *a,
                अचिह्नित aprefixlen,
                ध्रुव संरचना in_addr *b,
                अचिह्नित bprefixlen) विशुद्धम्;
bool in6_addr_उपसर्गः_intersect(
                ध्रुव संरचना in6_addr *a,
                अचिह्नित aprefixlen,
                ध्रुव संरचना in6_addr *b,
                अचिह्नित bprefixlen) विशुद्धम्;
पूर्णाङ्क अन्तर्_addr_उपसर्गः_intersect(
                पूर्णाङ्क family,
                ध्रुव सङ्घ अन्तर्_addr_union *a,
                अचिह्नित aprefixlen,
                ध्रुव सङ्घ अन्तर्_addr_union *b,
                अचिह्नित bprefixlen) विशुद्धम्;
पूर्णाङ्क अन्तर्_addr_random_उपसर्गः(पूर्णाङ्क family, सङ्घ अन्तर्_addr_union *u, अचिह्नित prefixlen_fixed_part, अचिह्नित prefixlen);
पूर्णाङ्क अन्तर्_addr_उपसर्गः_range(
                पूर्णाङ्क family,
                ध्रुव सङ्घ अन्तर्_addr_union *अन्तर्,
                अचिह्नित prefixlen,
                सङ्घ अन्तर्_addr_union *ret_start,
                सङ्घ अन्तर्_addr_union *ret_अन्तः);

पूर्णाङ्क अन्तर्_addr_गन्तव्यम्_string(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क in6_addr_गन्तव्यम्_string(ध्रुव संरचना in6_addr *u, वर्ण **ret) {
        प्रत्यागम अन्तर्_addr_गन्तव्यम्_string(AF_INET6, (ध्रुव सङ्घ अन्तर्_addr_union*) u, ret);
}

ध्रुव वर्ण* typesafe_inet_ntop(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *a, वर्ण *buf, size_t len);
ध्रुव वर्ण* typesafe_inet_ntop4(ध्रुव संरचना in_addr *a, वर्ण *buf, size_t len);
ध्रुव वर्ण* typesafe_inet_ntop6(ध्रुव संरचना in6_addr *a, वर्ण *buf, size_t len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#परिभाषय अन्तर्_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#परिभाषय अन्तर्_ADDR_गन्तव्यम्_STRING(family, addr) typesafe_inet_ntop(family, addr, (वर्ण[अन्तर्_ADDR_MAX]){}, अन्तर्_ADDR_MAX)
#परिभाषय IN4_ADDR_गन्तव्यम्_STRING(addr) typesafe_inet_ntop4(addr, (वर्ण[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#परिभाषय IN6_ADDR_गन्तव्यम्_STRING(addr) typesafe_inet_ntop6(addr, (वर्ण[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

पूर्णाङ्क अन्तर्_addr_उपसर्गः_गन्तव्यम्_string(
                पूर्णाङ्क family,
                ध्रुव सङ्घ अन्तर्_addr_union *u,
                अचिह्नित prefixlen,
                वर्ण *buf,
                size_t buf_len);

स्थिर अन्तःस्थ ध्रुव वर्ण* अन्तर्_addr_उपसर्गः_गन्तव्यम्_string_2(
                पूर्णाङ्क family,
                ध्रुव सङ्घ अन्तर्_addr_union *u,
                अचिह्नित prefixlen,
                वर्ण *buf,
                size_t buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(अन्तर्_addr_उपसर्गः_गन्तव्यम्_string(family, u, prefixlen, buf, buf_len) == 0);
        प्रत्यागम buf;
}
स्थिर अन्तःस्थ ध्रुव वर्ण* in4_addr_उपसर्गः_गन्तव्यम्_string(ध्रुव संरचना in_addr *a, अचिह्नित prefixlen, वर्ण *buf, size_t buf_len) {
        प्रत्यागम अन्तर्_addr_उपसर्गः_गन्तव्यम्_string_2(AF_INET, (ध्रुव सङ्घ अन्तर्_addr_union *) a, prefixlen, buf, buf_len);
}
स्थिर अन्तःस्थ ध्रुव वर्ण* in6_addr_उपसर्गः_गन्तव्यम्_string(ध्रुव संरचना in6_addr *a, अचिह्नित prefixlen, वर्ण *buf, size_t buf_len) {
        प्रत्यागम अन्तर्_addr_उपसर्गः_गन्तव्यम्_string_2(AF_INET6, (ध्रुव सङ्घ अन्तर्_addr_union *) a, prefixlen, buf, buf_len);
}

#परिभाषय उपसर्गः_SUFFIX_MAX (1 + DECIMAL_STR_MAX(अचिह्नित))
#परिभाषय अन्तर्_ADDR_उपसर्गः_गन्तव्यम्_STRING(family, addr, prefixlen) \
        अन्तर्_addr_उपसर्गः_गन्तव्यम्_string_2(family, addr, prefixlen, (वर्ण[अन्तर्_ADDR_MAX + उपसर्गः_SUFFIX_MAX]){}, अन्तर्_ADDR_MAX + उपसर्गः_SUFFIX_MAX)
#परिभाषय IN4_ADDR_उपसर्गः_गन्तव्यम्_STRING(addr, prefixlen) \
        in4_addr_उपसर्गः_गन्तव्यम्_string(addr, prefixlen, (वर्ण[INET_ADDRSTRLEN + उपसर्गः_SUFFIX_MAX]){}, INET_ADDRSTRLEN + उपसर्गः_SUFFIX_MAX)
#परिभाषय IN6_ADDR_उपसर्गः_गन्तव्यम्_STRING(addr, prefixlen) \
        in6_addr_उपसर्गः_गन्तव्यम्_string(addr, prefixlen, (वर्ण[INET6_ADDRSTRLEN + उपसर्गः_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + उपसर्गः_SUFFIX_MAX)

पूर्णाङ्क अन्तर्_addr_पत्तनम्_ifindex_name_गन्तव्यम्_string(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u, uint16_t port, पूर्णाङ्क ifindex, ध्रुव वर्ण *server_name, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क अन्तर्_addr_ifindex_गन्तव्यम्_string(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u, पूर्णाङ्क ifindex, वर्ण **ret) {
        प्रत्यागम अन्तर्_addr_पत्तनम्_ifindex_name_गन्तव्यम्_string(family, u, 0, ifindex, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क अन्तर्_addr_पत्तनम्_गन्तव्यम्_string(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *u, uint16_t port, वर्ण **ret) {
        प्रत्यागम अन्तर्_addr_पत्तनम्_ifindex_name_गन्तव्यम्_string(family, u, port, 0, रिक्त, ret);
}
पूर्णाङ्क अन्तर्_addr_from_string(पूर्णाङ्क family, ध्रुव वर्ण *s, सङ्घ अन्तर्_addr_union *ret);
पूर्णाङ्क अन्तर्_addr_from_string_auto(ध्रुव वर्ण *s, पूर्णाङ्क *ret_family, सङ्घ अन्तर्_addr_union *ret);

अचिह्नित वर्ण in4_addr_netmask_गन्तव्यम्_prefixlen(ध्रुव संरचना in_addr *addr);
संरचना in_addr* in4_addr_prefixlen_गन्तव्यम्_netmask(संरचना in_addr *addr, अचिह्नित वर्ण prefixlen);
संरचना in6_addr* in6_addr_prefixlen_गन्तव्यम्_netmask(संरचना in6_addr *addr, अचिह्नित वर्ण prefixlen);
पूर्णाङ्क अन्तर्_addr_prefixlen_गन्तव्यम्_netmask(पूर्णाङ्क family, सङ्घ अन्तर्_addr_union *addr, अचिह्नित वर्ण prefixlen);
पूर्णाङ्क in4_addr_default_prefixlen(ध्रुव संरचना in_addr *addr, अचिह्नित वर्ण *prefixlen);
पूर्णाङ्क in4_addr_default_subnet_आवरणम्(ध्रुव संरचना in_addr *addr, संरचना in_addr *आवरणम्_3);
पूर्णाङ्क in4_addr_आवरणम्(संरचना in_addr *addr, अचिह्नित वर्ण prefixlen);
पूर्णाङ्क in6_addr_आवरणम्(संरचना in6_addr *addr, अचिह्नित वर्ण prefixlen);
पूर्णाङ्क अन्तर्_addr_आवरणम्(पूर्णाङ्क family, सङ्घ अन्तर्_addr_union *addr, अचिह्नित वर्ण prefixlen);
पूर्णाङ्क in4_addr_उपसर्गः_covers_पूर्णम्(ध्रुव संरचना in_addr *उपसर्गः, अचिह्नित वर्ण prefixlen, ध्रुव संरचना in_addr *स्थानाङ्कः, अचिह्नित वर्ण स्थानाङ्कः_prefixlen);
पूर्णाङ्क in6_addr_उपसर्गः_covers_पूर्णम्(ध्रुव संरचना in6_addr *उपसर्गः, अचिह्नित वर्ण prefixlen, ध्रुव संरचना in6_addr *स्थानाङ्कः, अचिह्नित वर्ण स्थानाङ्कः_prefixlen);
पूर्णाङ्क अन्तर्_addr_उपसर्गः_covers_पूर्णम्(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *उपसर्गः, अचिह्नित वर्ण prefixlen, ध्रुव सङ्घ अन्तर्_addr_union *स्थानाङ्कः, अचिह्नित वर्ण स्थानाङ्कः_prefixlen);
स्थिर अन्तःस्थ पूर्णाङ्क in4_addr_उपसर्गः_covers(ध्रुव संरचना in_addr *उपसर्गः, अचिह्नित वर्ण prefixlen, ध्रुव संरचना in_addr *स्थानाङ्कः) {
        प्रत्यागम in4_addr_उपसर्गः_covers_पूर्णम्(उपसर्गः, prefixlen, स्थानाङ्कः, 32);
}
स्थिर अन्तःस्थ पूर्णाङ्क in6_addr_उपसर्गः_covers(ध्रुव संरचना in6_addr *उपसर्गः, अचिह्नित वर्ण prefixlen, ध्रुव संरचना in6_addr *स्थानाङ्कः) {
        प्रत्यागम in6_addr_उपसर्गः_covers_पूर्णम्(उपसर्गः, prefixlen, स्थानाङ्कः, 128);
}
स्थिर अन्तःस्थ पूर्णाङ्क अन्तर्_addr_उपसर्गः_covers(पूर्णाङ्क family, ध्रुव सङ्घ अन्तर्_addr_union *उपसर्गः, अचिह्नित वर्ण prefixlen, ध्रुव सङ्घ अन्तर्_addr_union *स्थानाङ्कः) {
        प्रत्यागम अन्तर्_addr_उपसर्गः_covers_पूर्णम्(family, उपसर्गः, prefixlen, स्थानाङ्कः, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
पूर्णाङ्क अन्तर्_addr_parse_prefixlen(पूर्णाङ्क family, ध्रुव वर्ण *p, अचिह्नित वर्ण *ret);
पूर्णाङ्क अन्तर्_addr_उपसर्गः_from_string(ध्रुव वर्ण *p, पूर्णाङ्क family, सङ्घ अन्तर्_addr_union *ret_उपसर्गः, अचिह्नित वर्ण *ret_prefixlen);

प्रकारनिर्देश गणना अन्तर्_Addr_उपसर्गः_Len_बहुलकम् {
        PREFIXLEN_पूर्णम्,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} अन्तर्_Addr_उपसर्गः_Len_बहुलकम्;

पूर्णाङ्क अन्तर्_addr_उपसर्गः_from_string_auto_पूर्णम्(ध्रुव वर्ण *p, अन्तर्_Addr_उपसर्गः_Len_बहुलकम् बहुलकम्, पूर्णाङ्क *ret_family, सङ्घ अन्तर्_addr_union *ret_उपसर्गः, अचिह्नित वर्ण *ret_prefixlen);
स्थिर अन्तःस्थ पूर्णाङ्क अन्तर्_addr_उपसर्गः_from_string_auto(ध्रुव वर्ण *p, पूर्णाङ्क *ret_family, सङ्घ अन्तर्_addr_union *ret_उपसर्गः, अचिह्नित वर्ण *ret_prefixlen) {
        प्रत्यागम अन्तर्_addr_उपसर्गः_from_string_auto_पूर्णम्(p, PREFIXLEN_पूर्णम्, ret_family, ret_उपसर्गः, ret_prefixlen);
}

स्थिर अन्तःस्थ size_t FAMILY_स्थानाङ्कः_आकार(पूर्णाङ्क family) {
        विकल्प (family) {
                पक्ष AF_INET:
                        प्रत्यागम 4;
                पक्ष AF_INET6:
                        प्रत्यागम 16;
                स्वभाव:
                        assert_not_reached();
        }
}

#परिभाषय FAMILY_स्थानाङ्कः_आकार_SAFE(f)                                     \
        ({                                                              \
                पूर्णाङ्क _f = (f);                                           \
                _f == AF_INET ? परिमाण(संरचना in_addr) :                \
                _f == AF_INET6 ? परिमाण(संरचना in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#परिभाषय अन्तर्_ADDR_NULL ((सङ्घ अन्तर्_addr_union) { .in6 = {} })

शून्य अन्तर्_addr_hash_func(ध्रुव सङ्घ अन्तर्_addr_union *u, पूर्णाङ्क family, संरचना siphash *state);
शून्य अन्तर्_addr_दत्त_hash_func(ध्रुव संरचना अन्तर्_addr_दत्त *a, संरचना siphash *state);
पूर्णाङ्क अन्तर्_addr_दत्त_compare_func(ध्रुव संरचना अन्तर्_addr_दत्त *x, ध्रुव संरचना अन्तर्_addr_दत्त *y);
शून्य in6_addr_hash_func(ध्रुव संरचना in6_addr *addr, संरचना siphash *state);
पूर्णाङ्क in6_addr_compare_func(ध्रुव संरचना in6_addr *a, ध्रुव संरचना in6_addr *b);

बाह्य ध्रुव संरचना hash_ops अन्तर्_addr_दत्त_hash_ops;
बाह्य ध्रुव संरचना hash_ops अन्तर्_addr_दत्त_hash_ops_free;
बाह्य ध्रुव संरचना hash_ops in6_addr_hash_ops;
बाह्य ध्रुव संरचना hash_ops in6_addr_hash_ops_free;

स्थिर अन्तःस्थ शून्य PTR_गन्तव्यम्_IN4_ADDR(ध्रुव शून्य *p, संरचना in_addr *ret) {
        प्रतिज्ञा(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

स्थिर अन्तःस्थ शून्य* IN4_ADDR_गन्तव्यम्_PTR(ध्रुव संरचना in_addr *a) {
        प्रतिज्ञा(a);
        प्रत्यागम (शून्य*) ((uintptr_t) a->s_addr);
}

#परिभाषय IPV4_स्थानाङ्कः_FMT_STR     "%u.%u.%u.%u"
#परिभाषय IPV4_स्थानाङ्कः_FMT_VAL(स्थानाङ्कः)              \
        be32toh((स्थानाङ्कः).s_addr) >> 24,           \
        (be32toh((स्थानाङ्कः).s_addr) >> 16) & 0xFFu, \
        (be32toh((स्थानाङ्कः).s_addr) >> 8) & 0xFFu,  \
        be32toh((स्थानाङ्कः).s_addr) & 0xFFu
