/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <netinet/in.h>

#समावेश "forward.h"

जोड़ in_addr_union {
        काष्ठा in_addr in;
        काष्ठा in6_addr in6;
        uint8_t bytes[CONST_MAX(माप(काष्ठा in_addr), माप(काष्ठा in6_addr))];
};

काष्ठा in_addr_तथ्य {
        पूर्णांक family;
        जोड़ in_addr_union address;
};

bool in4_addr_is_null(स्थिर काष्ठा in_addr *a) _pure_;
अटल अंतरभूत bool in4_addr_is_set(स्थिर काष्ठा in_addr *a) {
        वापस !in4_addr_is_null(a);
}
bool in6_addr_is_null(स्थिर काष्ठा in6_addr *a) _pure_;
अटल अंतरभूत bool in6_addr_is_set(स्थिर काष्ठा in6_addr *a) {
        वापस !in6_addr_is_null(a);
}
पूर्णांक in_addr_is_null(पूर्णांक family, स्थिर जोड़ in_addr_union *u) _pure_;
अटल अंतरभूत bool in_addr_is_set(पूर्णांक family, स्थिर जोड़ in_addr_union *u) {
        वापस in_addr_is_null(family, u) == 0;
}
अटल अंतरभूत पूर्णांक in_addr_तथ्य_is_null(स्थिर काष्ठा in_addr_तथ्य *a) {
        निश्चित(a);
        वापस in_addr_is_null(a->family, &a->address);
}
अटल अंतरभूत bool in_addr_तथ्य_is_set(स्थिर काष्ठा in_addr_तथ्य *a) {
        वापस in_addr_तथ्य_is_null(a);
}

bool in4_addr_is_multicast(स्थिर काष्ठा in_addr *a) _pure_;
bool in6_addr_is_multicast(स्थिर काष्ठा in6_addr *a) _pure_;
पूर्णांक in_addr_is_multicast(पूर्णांक family, स्थिर जोड़ in_addr_union *u) _pure_;

bool in4_addr_is_link_local(स्थिर काष्ठा in_addr *a) _pure_;
bool in4_addr_is_link_local_dynamic(स्थिर काष्ठा in_addr *a) _pure_;
bool in6_addr_is_link_local(स्थिर काष्ठा in6_addr *a) _pure_;
पूर्णांक in_addr_is_link_local(पूर्णांक family, स्थिर जोड़ in_addr_union *u) _pure_;
bool in6_addr_is_link_local_all_nodes(स्थिर काष्ठा in6_addr *a) _pure_;

bool in4_addr_is_localhost(स्थिर काष्ठा in_addr *a) _pure_;
पूर्णांक in_addr_is_localhost(पूर्णांक family, स्थिर जोड़ in_addr_union *u) _pure_;
पूर्णांक in_addr_is_localhost_one(पूर्णांक family, स्थिर जोड़ in_addr_union *u) _pure_;

bool in4_addr_is_local_multicast(स्थिर काष्ठा in_addr *a) _pure_;
bool in4_addr_is_non_local(स्थिर काष्ठा in_addr *a) _pure_;
bool in6_addr_is_ipv4_mapped_address(स्थिर काष्ठा in6_addr *a) _pure_;

bool in4_addr_equal(स्थिर काष्ठा in_addr *a, स्थिर काष्ठा in_addr *b) _pure_;
bool in6_addr_equal(स्थिर काष्ठा in6_addr *a, स्थिर काष्ठा in6_addr *b) _pure_;
पूर्णांक in_addr_equal(पूर्णांक family, स्थिर जोड़ in_addr_union *a, स्थिर जोड़ in_addr_union *b) _pure_;
bool in4_addr_prefix_intersect(
                स्थिर काष्ठा in_addr *a,
                अचिन्हित aprefixlen,
                स्थिर काष्ठा in_addr *b,
                अचिन्हित bprefixlen) _pure_;
bool in6_addr_prefix_intersect(
                स्थिर काष्ठा in6_addr *a,
                अचिन्हित aprefixlen,
                स्थिर काष्ठा in6_addr *b,
                अचिन्हित bprefixlen) _pure_;
पूर्णांक in_addr_prefix_intersect(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *a,
                अचिन्हित aprefixlen,
                स्थिर जोड़ in_addr_union *b,
                अचिन्हित bprefixlen) _pure_;
पूर्णांक in_addr_random_prefix(पूर्णांक family, जोड़ in_addr_union *u, अचिन्हित prefixlen_fixed_part, अचिन्हित prefixlen);
पूर्णांक in_addr_prefix_range(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *in,
                अचिन्हित prefixlen,
                जोड़ in_addr_union *ret_start,
                जोड़ in_addr_union *ret_end);

पूर्णांक in_addr_to_string(पूर्णांक family, स्थिर जोड़ in_addr_union *u, अक्षर **ret);
अटल अंतरभूत पूर्णांक in6_addr_to_string(स्थिर काष्ठा in6_addr *u, अक्षर **ret) {
        वापस in_addr_to_string(AF_INET6, (स्थिर जोड़ in_addr_union*) u, ret);
}

स्थिर अक्षर* typesafe_inet_ntop(पूर्णांक family, स्थिर जोड़ in_addr_union *a, अक्षर *buf, माप_प्रकार len);
स्थिर अक्षर* typesafe_inet_ntop4(स्थिर काष्ठा in_addr *a, अक्षर *buf, माप_प्रकार len);
स्थिर अक्षर* typesafe_inet_ntop6(स्थिर काष्ठा in6_addr *a, अक्षर *buf, माप_प्रकार len);

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#घोषणा IN_ADDR_MAX CONST_MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)
#घोषणा IN_ADDR_TO_STRING(family, addr) typesafe_inet_ntop(family, addr, (अक्षर[IN_ADDR_MAX]){}, IN_ADDR_MAX)
#घोषणा IN4_ADDR_TO_STRING(addr) typesafe_inet_ntop4(addr, (अक्षर[INET_ADDRSTRLEN]){}, INET_ADDRSTRLEN)
#घोषणा IN6_ADDR_TO_STRING(addr) typesafe_inet_ntop6(addr, (अक्षर[INET6_ADDRSTRLEN]){}, INET6_ADDRSTRLEN)

पूर्णांक in_addr_prefix_to_string(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *u,
                अचिन्हित prefixlen,
                अक्षर *buf,
                माप_प्रकार buf_len);

अटल अंतरभूत स्थिर अक्षर* _in_addr_prefix_to_string(
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *u,
                अचिन्हित prefixlen,
                अक्षर *buf,
                माप_प्रकार buf_len) {
        /* We assume that this is called with an appropriately sized buffer and can never fail. */
        assert_se(in_addr_prefix_to_string(family, u, prefixlen, buf, buf_len) == 0);
        वापस buf;
}
अटल अंतरभूत स्थिर अक्षर* _in4_addr_prefix_to_string(स्थिर काष्ठा in_addr *a, अचिन्हित prefixlen, अक्षर *buf, माप_प्रकार buf_len) {
        वापस _in_addr_prefix_to_string(AF_INET, (स्थिर जोड़ in_addr_union *) a, prefixlen, buf, buf_len);
}
अटल अंतरभूत स्थिर अक्षर* _in6_addr_prefix_to_string(स्थिर काष्ठा in6_addr *a, अचिन्हित prefixlen, अक्षर *buf, माप_प्रकार buf_len) {
        वापस _in_addr_prefix_to_string(AF_INET6, (स्थिर जोड़ in_addr_union *) a, prefixlen, buf, buf_len);
}

#घोषणा PREFIX_SUFFIX_MAX (1 + DECIMAL_STR_MAX(अचिन्हित))
#घोषणा IN_ADDR_PREFIX_TO_STRING(family, addr, prefixlen) \
        _in_addr_prefix_to_string(family, addr, prefixlen, (अक्षर[IN_ADDR_MAX + PREFIX_SUFFIX_MAX]){}, IN_ADDR_MAX + PREFIX_SUFFIX_MAX)
#घोषणा IN4_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in4_addr_prefix_to_string(addr, prefixlen, (अक्षर[INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET_ADDRSTRLEN + PREFIX_SUFFIX_MAX)
#घोषणा IN6_ADDR_PREFIX_TO_STRING(addr, prefixlen) \
        _in6_addr_prefix_to_string(addr, prefixlen, (अक्षर[INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX]){}, INET6_ADDRSTRLEN + PREFIX_SUFFIX_MAX)

पूर्णांक in_addr_port_ifindex_name_to_string(पूर्णांक family, स्थिर जोड़ in_addr_union *u, uint16_t port, पूर्णांक ifindex, स्थिर अक्षर *server_name, अक्षर **ret);
अटल अंतरभूत पूर्णांक in_addr_ifindex_to_string(पूर्णांक family, स्थिर जोड़ in_addr_union *u, पूर्णांक ifindex, अक्षर **ret) {
        वापस in_addr_port_ifindex_name_to_string(family, u, 0, ifindex, शून्य, ret);
}
अटल अंतरभूत पूर्णांक in_addr_port_to_string(पूर्णांक family, स्थिर जोड़ in_addr_union *u, uint16_t port, अक्षर **ret) {
        वापस in_addr_port_ifindex_name_to_string(family, u, port, 0, शून्य, ret);
}
पूर्णांक in_addr_from_string(पूर्णांक family, स्थिर अक्षर *s, जोड़ in_addr_union *ret);
पूर्णांक in_addr_from_string_auto(स्थिर अक्षर *s, पूर्णांक *ret_family, जोड़ in_addr_union *ret);

अचिन्हित अक्षर in4_addr_netmask_to_prefixlen(स्थिर काष्ठा in_addr *addr);
काष्ठा in_addr* in4_addr_prefixlen_to_netmask(काष्ठा in_addr *addr, अचिन्हित अक्षर prefixlen);
काष्ठा in6_addr* in6_addr_prefixlen_to_netmask(काष्ठा in6_addr *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in_addr_prefixlen_to_netmask(पूर्णांक family, जोड़ in_addr_union *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in4_addr_default_prefixlen(स्थिर काष्ठा in_addr *addr, अचिन्हित अक्षर *prefixlen);
पूर्णांक in4_addr_default_subnet_mask(स्थिर काष्ठा in_addr *addr, काष्ठा in_addr *mask);
पूर्णांक in4_addr_mask(काष्ठा in_addr *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in6_addr_mask(काष्ठा in6_addr *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in_addr_mask(पूर्णांक family, जोड़ in_addr_union *addr, अचिन्हित अक्षर prefixlen);
पूर्णांक in4_addr_prefix_covers_full(स्थिर काष्ठा in_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिर काष्ठा in_addr *address, अचिन्हित अक्षर address_prefixlen);
पूर्णांक in6_addr_prefix_covers_full(स्थिर काष्ठा in6_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिर काष्ठा in6_addr *address, अचिन्हित अक्षर address_prefixlen);
पूर्णांक in_addr_prefix_covers_full(पूर्णांक family, स्थिर जोड़ in_addr_union *prefix, अचिन्हित अक्षर prefixlen, स्थिर जोड़ in_addr_union *address, अचिन्हित अक्षर address_prefixlen);
अटल अंतरभूत पूर्णांक in4_addr_prefix_covers(स्थिर काष्ठा in_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिर काष्ठा in_addr *address) {
        वापस in4_addr_prefix_covers_full(prefix, prefixlen, address, 32);
}
अटल अंतरभूत पूर्णांक in6_addr_prefix_covers(स्थिर काष्ठा in6_addr *prefix, अचिन्हित अक्षर prefixlen, स्थिर काष्ठा in6_addr *address) {
        वापस in6_addr_prefix_covers_full(prefix, prefixlen, address, 128);
}
अटल अंतरभूत पूर्णांक in_addr_prefix_covers(पूर्णांक family, स्थिर जोड़ in_addr_union *prefix, अचिन्हित अक्षर prefixlen, स्थिर जोड़ in_addr_union *address) {
        वापस in_addr_prefix_covers_full(family, prefix, prefixlen, address, family == AF_INET ? 32 : family == AF_INET6 ? 128 : 0);
}
पूर्णांक in_addr_parse_prefixlen(पूर्णांक family, स्थिर अक्षर *p, अचिन्हित अक्षर *ret);
पूर्णांक in_addr_prefix_from_string(स्थिर अक्षर *p, पूर्णांक family, जोड़ in_addr_union *ret_prefix, अचिन्हित अक्षर *ret_prefixlen);

प्रकार क्रमागत InAddrPrefixLenMode {
        PREFIXLEN_FULL,   /* Default to prefixlen of address size, 32 for IPv4 or 128 for IPv6, if not specified. */
        PREFIXLEN_REFUSE, /* Fail with -ENOANO if prefixlen is not specified. */
} InAddrPrefixLenMode;

पूर्णांक in_addr_prefix_from_string_auto_full(स्थिर अक्षर *p, InAddrPrefixLenMode mode, पूर्णांक *ret_family, जोड़ in_addr_union *ret_prefix, अचिन्हित अक्षर *ret_prefixlen);
अटल अंतरभूत पूर्णांक in_addr_prefix_from_string_auto(स्थिर अक्षर *p, पूर्णांक *ret_family, जोड़ in_addr_union *ret_prefix, अचिन्हित अक्षर *ret_prefixlen) {
        वापस in_addr_prefix_from_string_auto_full(p, PREFIXLEN_FULL, ret_family, ret_prefix, ret_prefixlen);
}

अटल अंतरभूत माप_प्रकार FAMILY_ADDRESS_आकार(पूर्णांक family) {
        चयन (family) {
                हाल AF_INET:
                        वापस 4;
                हाल AF_INET6:
                        वापस 16;
                शेष:
                        assert_not_reached();
        }
}

#घोषणा FAMILY_ADDRESS_आकार_SAFE(f)                                     \
        ({                                                              \
                पूर्णांक _f = (f);                                           \
                _f == AF_INET ? माप(काष्ठा in_addr) :                \
                _f == AF_INET6 ? माप(काष्ठा in6_addr) : 0;           \
        })

/* Workaround for clang, explicitly specify the maximum-size element here.
 * See also oss-fuzz#11344. */
#घोषणा IN_ADDR_NULL ((जोड़ in_addr_union) { .in6 = {} })

व्योम in_addr_hash_func(स्थिर जोड़ in_addr_union *u, पूर्णांक family, काष्ठा siphash *state);
व्योम in_addr_तथ्य_hash_func(स्थिर काष्ठा in_addr_तथ्य *a, काष्ठा siphash *state);
पूर्णांक in_addr_तथ्य_compare_func(स्थिर काष्ठा in_addr_तथ्य *x, स्थिर काष्ठा in_addr_तथ्य *y);
व्योम in6_addr_hash_func(स्थिर काष्ठा in6_addr *addr, काष्ठा siphash *state);
पूर्णांक in6_addr_compare_func(स्थिर काष्ठा in6_addr *a, स्थिर काष्ठा in6_addr *b);

बाह्य स्थिर काष्ठा hash_ops in_addr_तथ्य_hash_ops;
बाह्य स्थिर काष्ठा hash_ops in_addr_तथ्य_hash_ops_free;
बाह्य स्थिर काष्ठा hash_ops in6_addr_hash_ops;
बाह्य स्थिर काष्ठा hash_ops in6_addr_hash_ops_free;

अटल अंतरभूत व्योम PTR_TO_IN4_ADDR(स्थिर व्योम *p, काष्ठा in_addr *ret) {
        निश्चित(ret);
        ret->s_addr = (uint32_t) ((uintptr_t) p);
}

अटल अंतरभूत व्योम* IN4_ADDR_TO_PTR(स्थिर काष्ठा in_addr *a) {
        निश्चित(a);
        वापस (व्योम*) ((uintptr_t) a->s_addr);
}

#घोषणा IPV4_ADDRESS_FMT_STR     "%u.%u.%u.%u"
#घोषणा IPV4_ADDRESS_FMT_VAL(address)              \
        be32toh((address).s_addr) >> 24,           \
        (be32toh((address).s_addr) >> 16) & 0xFFu, \
        (be32toh((address).s_addr) >> 8) & 0xFFu,  \
        be32toh((address).s_addr) & 0xFFu
