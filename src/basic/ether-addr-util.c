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

वर्ण* hw_addr_to_string_full(
                अचर संरचना hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                वर्ण buffer[स्थिर HW_ADDR_TO_STRING_MAX]) {

        प्रतिज्ञा(addr);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(addr->length <= HW_ADDR_MAX_SIZE);

        लागि (माप_प्रकार i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                यदि (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        फर्काऊ buffer;
}

संरचना hw_addr_data *hw_addr_set(संरचना hw_addr_data *addr, अचर uint8_t *bytes, माप_प्रकार length) {
        प्रतिज्ञा(addr);
        प्रतिज्ञा(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        फर्काऊ addr;
}

पूर्णाङ्क hw_addr_compare(अचर संरचना hw_addr_data *a, अचर संरचना hw_addr_data *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(a->length, b->length);
        यदि (r != 0)
                फर्काऊ r;

        फर्काऊ स_भेद(a->bytes, b->bytes, a->length);
}

शून्य hw_addr_hash_func(अचर संरचना hw_addr_data *p, संरचना siphash *state) {
        प्रतिज्ञा(p);
        प्रतिज्ञा(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(अचर संरचना hw_addr_data *addr) {
        प्रतिज्ञा(addr);
        फर्काऊ addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(अचर संरचना hw_addr_data *addr, uint16_t iftype) {
        प्रतिज्ञा(addr);

        विकल्प (iftype) {
        पक्ष ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                यदि (addr->length != ETH_ALEN)
                        फर्काऊ false;

                फर्काऊ !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        पक्ष ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                यदि (addr->length != INFINIBAND_ALEN)
                        फर्काऊ false;

                फर्काऊ !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        स्वभाव:
                फर्काऊ false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, संरचना hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_साँचो_DESTRUCTOR(hw_addr_hash_ops_free, संरचना hw_addr_data, hw_addr_hash_func, hw_addr_compare, मुक्त);

वर्ण* ether_addr_to_string(अचर संरचना ether_addr *addr, वर्ण buffer[ETHER_ADDR_TO_STRING_MAX]) {
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

        फर्काऊ buffer;
}

पूर्णाङ्क ether_addr_compare(अचर संरचना ether_addr *a, अचर संरचना ether_addr *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        फर्काऊ स_भेद(a, b, ETH_ALEN);
}

स्थिर शून्य ether_addr_hash_func(अचर संरचना ether_addr *p, संरचना siphash *state) {
        प्रतिज्ञा(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(अचर संरचना ether_addr *addr) {
        प्रतिज्ञा(addr);
        फर्काऊ memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, संरचना ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_साँचो_DESTRUCTOR(ether_addr_hash_ops_free, संरचना ether_addr, ether_addr_hash_func, ether_addr_compare, मुक्त);

स्थिर पूर्णाङ्क parse_hw_addr_one_field(अचर वर्ण **s, वर्ण sep, माप_प्रकार len, uint8_t *buf) {
        अचर वर्ण *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        प्रतिज्ञा(s);
        प्रतिज्ञा(*s);
        प्रतिज्ञा(IN_SET(len, 1, 2));
        प्रतिज्ञा(buf);

        p = *s;

        लागि (माप_प्रकार i = 0; i < len * 2; i++) {
                अचर वर्ण *hexoff;
                माप_प्रकार x;

                यदि (*p == '\0' || *p == sep) {
                        यदि (i == 0)
                                फर्काऊ -EINVAL;
                        रोक;
                }

                hexoff = म_अक्षर(hex, *p);
                यदि (!hexoff)
                        फर्काऊ -EINVAL;

                प्रतिज्ञा(hexoff >= hex);
                x = hexoff - hex;
                यदि (x >= 16)
                        x -= 6; /* A-F */

                प्रतिज्ञा(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        यदि (*p != '\0' && *p != sep)
                फर्काऊ -EINVAL;

        विकल्प (len) {
        पक्ष 1:
                buf[0] = data;
                रोक;
        पक्ष 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                रोक;
        स्वभाव:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        फर्काऊ cont;
}

पूर्णाङ्क parse_hw_addr_full(अचर वर्ण *s, माप_प्रकार expected_len, संरचना hw_addr_data *ret) {
        माप_प्रकार field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        वर्ण sep;
        पूर्णाङ्क r;

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

        यदि (IN_SET(expected_len, 0, परिमाण(संरचना in_addr), परिमाण(संरचना in6_addr))) {
                सङ्घ in_addr_union a;
                पूर्णाङ्क family;

                यदि (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                अन्यथा {
                        family = expected_len == परिमाण(संरचना in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                यदि (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        स_नकल(ret->bytes, a.bytes, ret->length);
                        फर्काऊ 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[म_अखोज(s, HEXDIGITS)];

        यदि (sep == '.')
                field_size = 2;
        अन्यथा यदि (IN_SET(sep, ':', '-'))
                field_size = 1;
        अन्यथा
                फर्काऊ -EINVAL;

        यदि (max_len % field_size != 0)
                फर्काऊ -EINVAL;

        लागि (माप_प्रकार i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0) {
                        len = (i + 1) * field_size;
                        रोक;
                }
        }

        यदि (len == 0)
                फर्काऊ -EINVAL;

        यदि (expected_len == 0) {
                यदि (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        फर्काऊ -EINVAL;
        } अन्यथा यदि (expected_len != SIZE_MAX) {
                यदि (len != expected_len)
                        फर्काऊ -EINVAL;
        }

        ret->length = len;
        स_नकल(ret->bytes, bytes, ret->length);
        फर्काऊ 0;
}

पूर्णाङ्क parse_ether_addr(अचर वर्ण *s, संरचना ether_addr *ret) {
        संरचना hw_addr_data a;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        यदि (r < 0)
                फर्काऊ r;

        *ret = a.ether;
        फर्काऊ 0;
}

शून्य ether_addr_mark_random(संरचना ether_addr *addr) {
        प्रतिज्ञा(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

पूर्णाङ्क hw_addr_ensure_broadcast(संरचना hw_addr_data *bcast_addr, uint16_t arp_type) {
        प्रतिज्ञा(bcast_addr);

        यदि (!hw_addr_is_null(bcast_addr))
                फर्काऊ 0;

        विकल्प (arp_type) {
        पक्ष ARPHRD_ETHER:
                *bcast_addr = (संरचना hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                फर्काऊ 0;
        पक्ष ARPHRD_INFINIBAND:
                *bcast_addr = (संरचना hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                फर्काऊ 0;
        स्वभाव:
                फर्काऊ -EAFNOSUPPORT;
        }
}
