/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <net/ethernet.h>
#समावेश <net/if_arp.h>
#समावेश <stdio.h>

#समावेश "ether-addr-util.h"
#समावेश "hash-funcs.h"
#समावेश "hexdecoct.h"
#समावेश "in-addr-util.h"
#समावेश "memory-util.h"
#समावेश "siphash24.h"
#समावेश "string-util.h"

अक्षर* hw_addr_to_string_full(
                स्थिर काष्ठा hw_addr_तथ्य *addr,
                HardwareAddressToStringFlags flags,
                अक्षर बफर[अटल HW_ADDR_TO_STRING_MAX]) {

        निश्चित(addr);
        निश्चित(बफर);
        निश्चित(addr->length <= HW_ADDR_MAX_आकार);

        क्रम (माप_प्रकार i = 0, j = 0; i < addr->length; i++) {
                बफर[j++] = hexchar(addr->bytes[i] >> 4);
                बफर[j++] = hexchar(addr->bytes[i] & 0x0f);
                अगर (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        बफर[j++] = ':';
        }

        बफर[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        वापस बफर;
}

काष्ठा hw_addr_तथ्य *hw_addr_set(काष्ठा hw_addr_तथ्य *addr, स्थिर uint8_t *bytes, माप_प्रकार length) {
        निश्चित(addr);
        निश्चित(length <= HW_ADDR_MAX_आकार);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        वापस addr;
}

पूर्णांक hw_addr_compare(स्थिर काष्ठा hw_addr_तथ्य *a, स्थिर काष्ठा hw_addr_तथ्य *b) {
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        r = CMP(a->length, b->length);
        अगर (r != 0)
                वापस r;

        वापस स_भेद(a->bytes, b->bytes, a->length);
}

व्योम hw_addr_hash_func(स्थिर काष्ठा hw_addr_तथ्य *p, काष्ठा siphash *state) {
        निश्चित(p);
        निश्चित(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(स्थिर काष्ठा hw_addr_तथ्य *addr) {
        निश्चित(addr);
        वापस addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(स्थिर काष्ठा hw_addr_तथ्य *addr, uint16_t iftype) {
        निश्चित(addr);

        चयन (iftype) {
        हाल ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                अगर (addr->length != ETH_ALEN)
                        वापस false;

                वापस !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        हाल ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                अगर (addr->length != INFINIBAND_ALEN)
                        वापस false;

                वापस !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        शेष:
                वापस false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, काष्ठा hw_addr_तथ्य, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(hw_addr_hash_ops_free, काष्ठा hw_addr_तथ्य, hw_addr_hash_func, hw_addr_compare, मुक्त);

अक्षर* ether_addr_to_string(स्थिर काष्ठा ether_addr *addr, अक्षर बफर[ETHER_ADDR_TO_STRING_MAX]) {
        निश्चित(addr);
        निश्चित(बफर);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        प्र_लिखो(बफर, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        वापस बफर;
}

पूर्णांक ether_addr_compare(स्थिर काष्ठा ether_addr *a, स्थिर काष्ठा ether_addr *b) {
        निश्चित(a);
        निश्चित(b);

        वापस स_भेद(a, b, ETH_ALEN);
}

अटल व्योम ether_addr_hash_func(स्थिर काष्ठा ether_addr *p, काष्ठा siphash *state) {
        निश्चित(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(स्थिर काष्ठा ether_addr *addr) {
        निश्चित(addr);
        वापस memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, काष्ठा ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(ether_addr_hash_ops_free, काष्ठा ether_addr, ether_addr_hash_func, ether_addr_compare, मुक्त);

अटल पूर्णांक parse_hw_addr_one_field(स्थिर अक्षर **s, अक्षर sep, माप_प्रकार len, uint8_t *buf) {
        स्थिर अक्षर *hex = HEXDIGITS, *p;
        uint16_t तथ्य = 0;
        bool cont;

        निश्चित(s);
        निश्चित(*s);
        निश्चित(IN_SET(len, 1, 2));
        निश्चित(buf);

        p = *s;

        क्रम (माप_प्रकार i = 0; i < len * 2; i++) {
                स्थिर अक्षर *hexoff;
                माप_प्रकार x;

                अगर (*p == '\0' || *p == sep) {
                        अगर (i == 0)
                                वापस -EINVAL;
                        अवरोध;
                }

                hexoff = म_अक्षर(hex, *p);
                अगर (!hexoff)
                        वापस -EINVAL;

                निश्चित(hexoff >= hex);
                x = hexoff - hex;
                अगर (x >= 16)
                        x -= 6; /* A-F */

                निश्चित(x < 16);
                तथ्य <<= 4;
                तथ्य += x;

                p++;
        }

        अगर (*p != '\0' && *p != sep)
                वापस -EINVAL;

        चयन (len) {
        हाल 1:
                buf[0] = तथ्य;
                अवरोध;
        हाल 2:
                buf[0] = (तथ्य & 0xff00) >> 8;
                buf[1] = तथ्य & 0xff;
                अवरोध;
        शेष:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        वापस cont;
}

पूर्णांक parse_hw_addr_full(स्थिर अक्षर *s, माप_प्रकार expected_len, काष्ठा hw_addr_तथ्य *ret) {
        माप_प्रकार field_आकार, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_आकार];
        अक्षर sep;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(expected_len <= HW_ADDR_MAX_आकार || expected_len == SIZE_MAX);
        निश्चित(ret);

        /* This accepts the following formats:
         *
         * Dot separated 2 bytes format: xxyy.zzaa.bbcc
         * Colon separated 1 bytes format: xx:yy:zz:aa:bb:cc
         * Hyphen separated 1 bytes format: xx-yy-zz-aa-bb-cc
         *
         * Moreover, if expected_len == 0, 4, or 16, this also accepts:
         *
         * IPv4 format: used by IPv4 tunnel, e.g. ipgre
         * IPv6 format: used by IPv6 tunnel, e.g. ip6gre
         *
         * The expected_len argument controls the length of acceptable addresses:
         *
         * 0: accepts 4 (AF_INET), 16 (AF_INET6), 6 (ETH_ALEN), or 20 (INFINIBAND_ALEN).
         * SIZE_MAX: accepts arbitrary length, but at least one separator must be included.
         * Otherwise: accepts addresses with matching length.
         */

        अगर (IN_SET(expected_len, 0, माप(काष्ठा in_addr), माप(काष्ठा in6_addr))) {
                जोड़ in_addr_union a;
                पूर्णांक family;

                अगर (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                अन्यथा {
                        family = expected_len == माप(काष्ठा in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                अगर (r >= 0) {
                        ret->length = FAMILY_ADDRESS_आकार(family);
                        स_नकल(ret->bytes, a.bytes, ret->length);
                        वापस 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_आकार : expected_len;
        sep = s[म_अखोज(s, HEXDIGITS)];

        अगर (sep == '.')
                field_आकार = 2;
        अन्यथा अगर (IN_SET(sep, ':', '-'))
                field_आकार = 1;
        अन्यथा
                वापस -EINVAL;

        अगर (max_len % field_आकार != 0)
                वापस -EINVAL;

        क्रम (माप_प्रकार i = 0; i < max_len / field_आकार; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_आकार, bytes + i * field_आकार);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0) {
                        len = (i + 1) * field_आकार;
                        अवरोध;
                }
        }

        अगर (len == 0)
                वापस -EINVAL;

        अगर (expected_len == 0) {
                अगर (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        वापस -EINVAL;
        } अन्यथा अगर (expected_len != SIZE_MAX) {
                अगर (len != expected_len)
                        वापस -EINVAL;
        }

        ret->length = len;
        स_नकल(ret->bytes, bytes, ret->length);
        वापस 0;
}

पूर्णांक parse_ether_addr(स्थिर अक्षर *s, काष्ठा ether_addr *ret) {
        काष्ठा hw_addr_तथ्य a;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        अगर (r < 0)
                वापस r;

        *ret = a.ether;
        वापस 0;
}

व्योम ether_addr_mark_random(काष्ठा ether_addr *addr) {
        निश्चित(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

पूर्णांक hw_addr_ensure_broadcast(काष्ठा hw_addr_तथ्य *bcast_addr, uint16_t arp_type) {
        निश्चित(bcast_addr);

        अगर (!hw_addr_is_null(bcast_addr))
                वापस 0;

        चयन (arp_type) {
        हाल ARPHRD_ETHER:
                *bcast_addr = (काष्ठा hw_addr_तथ्य) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                वापस 0;
        हाल ARPHRD_INFINIBAND:
                *bcast_addr = (काष्ठा hw_addr_तथ्य) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                वापस 0;
        शेष:
                वापस -EAFNOSUPPORT;
        }
}
