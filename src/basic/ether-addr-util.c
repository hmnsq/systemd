/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <net/ethernet.h>
#अन्तर्भाव <net/if_arp.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "ether-addr-util.h"
#अन्तर्भाव "hash-funcs.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "in-addr-util.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "string-util.h"

अक्षर* hw_addr_to_string_full(
                स्थिरांक संरचना hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                अक्षर buffer[स्थिर HW_ADDR_TO_STRING_MAX]) {

        प्रतिज्ञा(addr);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(addr->length <= HW_ADDR_MAX_SIZE);

        साठी (माप_प्रकार i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                जर (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        परत buffer;
}

संरचना hw_addr_data *hw_addr_set(संरचना hw_addr_data *addr, स्थिरांक uint8_t *bytes, माप_प्रकार length) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        परत addr;
}

पूर्णांक hw_addr_compare(स्थिरांक संरचना hw_addr_data *a, स्थिरांक संरचना hw_addr_data *b) {
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(a->length, b->length);
        जर (r != 0)
                परत r;

        परत स_भेद(a->bytes, b->bytes, a->length);
}

शून्य hw_addr_hash_func(स्थिरांक संरचना hw_addr_data *p, संरचना siphash *state) {
        प्रतिज्ञा(p);
        प्रतिज्ञा(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(स्थिरांक संरचना hw_addr_data *addr) {
        प्रतिज्ञा(addr);
        परत addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(स्थिरांक संरचना hw_addr_data *addr, uint16_t iftype) {
        प्रतिज्ञा(addr);

        निवड (iftype) {
        स्थिती ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                जर (addr->length != ETH_ALEN)
                        परत false;

                परत !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        स्थिती ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                जर (addr->length != INFINIBAND_ALEN)
                        परत false;

                परत !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        पूर्वनिर्धारित:
                परत false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, संरचना hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(hw_addr_hash_ops_free, संरचना hw_addr_data, hw_addr_hash_func, hw_addr_compare, मुक्त);

अक्षर* ether_addr_to_string(स्थिरांक संरचना ether_addr *addr, अक्षर buffer[ETHER_ADDR_TO_STRING_MAX]) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(buffer);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        प्र_लिखो(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        परत buffer;
}

पूर्णांक ether_addr_compare(स्थिरांक संरचना ether_addr *a, स्थिरांक संरचना ether_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        परत स_भेद(a, b, ETH_ALEN);
}

स्थिर शून्य ether_addr_hash_func(स्थिरांक संरचना ether_addr *p, संरचना siphash *state) {
        प्रतिज्ञा(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(स्थिरांक संरचना ether_addr *addr) {
        प्रतिज्ञा(addr);
        परत memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, संरचना ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(ether_addr_hash_ops_free, संरचना ether_addr, ether_addr_hash_func, ether_addr_compare, मुक्त);

स्थिर पूर्णांक parse_hw_addr_one_field(स्थिरांक अक्षर **s, अक्षर sep, माप_प्रकार len, uint8_t *buf) {
        स्थिरांक अक्षर *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        प्रतिज्ञा(s);
        प्रतिज्ञा(*s);
        प्रतिज्ञा(IN_SET(len, 1, 2));
        प्रतिज्ञा(buf);

        p = *s;

        साठी (माप_प्रकार i = 0; i < len * 2; i++) {
                स्थिरांक अक्षर *hexoff;
                माप_प्रकार x;

                जर (*p == '\0' || *p == sep) {
                        जर (i == 0)
                                परत -EINVAL;
                        थांबा;
                }

                hexoff = म_अक्षर(hex, *p);
                जर (!hexoff)
                        परत -EINVAL;

                प्रतिज्ञा(hexoff >= hex);
                x = hexoff - hex;
                जर (x >= 16)
                        x -= 6; /* A-F */

                प्रतिज्ञा(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        जर (*p != '\0' && *p != sep)
                परत -EINVAL;

        निवड (len) {
        स्थिती 1:
                buf[0] = data;
                थांबा;
        स्थिती 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                थांबा;
        पूर्वनिर्धारित:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        परत cont;
}

पूर्णांक parse_hw_addr_full(स्थिरांक अक्षर *s, माप_प्रकार expected_len, संरचना hw_addr_data *ret) {
        माप_प्रकार field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        अक्षर sep;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        प्रतिज्ञा(ret);

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

        जर (IN_SET(expected_len, 0, आकार(संरचना in_addr), आकार(संरचना in6_addr))) {
                संघ in_addr_union a;
                पूर्णांक family;

                जर (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                अन्यथा {
                        family = expected_len == आकार(संरचना in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                जर (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        स_नकल(ret->bytes, a.bytes, ret->length);
                        परत 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[म_अखोज(s, HEXDIGITS)];

        जर (sep == '.')
                field_size = 2;
        अन्यथा जर (IN_SET(sep, ':', '-'))
                field_size = 1;
        अन्यथा
                परत -EINVAL;

        जर (max_len % field_size != 0)
                परत -EINVAL;

        साठी (माप_प्रकार i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                जर (r < 0)
                        परत r;
                जर (r == 0) {
                        len = (i + 1) * field_size;
                        थांबा;
                }
        }

        जर (len == 0)
                परत -EINVAL;

        जर (expected_len == 0) {
                जर (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        परत -EINVAL;
        } अन्यथा जर (expected_len != SIZE_MAX) {
                जर (len != expected_len)
                        परत -EINVAL;
        }

        ret->length = len;
        स_नकल(ret->bytes, bytes, ret->length);
        परत 0;
}

पूर्णांक parse_ether_addr(स्थिरांक अक्षर *s, संरचना ether_addr *ret) {
        संरचना hw_addr_data a;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        जर (r < 0)
                परत r;

        *ret = a.ether;
        परत 0;
}

शून्य ether_addr_mark_random(संरचना ether_addr *addr) {
        प्रतिज्ञा(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

पूर्णांक hw_addr_ensure_broadcast(संरचना hw_addr_data *bcast_addr, uint16_t arp_type) {
        प्रतिज्ञा(bcast_addr);

        जर (!hw_addr_is_null(bcast_addr))
                परत 0;

        निवड (arp_type) {
        स्थिती ARPHRD_ETHER:
                *bcast_addr = (संरचना hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                परत 0;
        स्थिती ARPHRD_INFINIBAND:
                *bcast_addr = (संरचना hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                परत 0;
        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}
