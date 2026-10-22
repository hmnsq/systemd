/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <net/ethernet.h>
#ಅನ್ತರ್ಭಾವ <net/if_arp.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "ether-addr-util.h"
#ಅನ್ತರ್ಭಾವ "hash-funcs.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "in-addr-util.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ವರ್ಣ* hw_addr_to_string_full(
                ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                ವರ್ಣ buffer[ಸ್ಥಿರ HW_ADDR_TO_STRING_MAX]) {

        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(addr->length <= HW_ADDR_MAX_SIZE);

        ಪ್ರತಿ (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                ಯದಿ (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        ಪ್ರತ್ಯಾಗಮ buffer;
}

ಸಂರಚನಾ hw_addr_data *hw_addr_set(ಸಂರಚನಾ hw_addr_data *addr, ಧ್ರುವ uint8_t *bytes, size_t length) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        ಪ್ರತ್ಯಾಗಮ addr;
}

ಪೂರ್ಣಾಙ್ಕ hw_addr_compare(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *a, ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(a->length, b->length);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ memcmp(a->bytes, b->bytes, a->length);
}

ಶೂನ್ಯ hw_addr_hash_func(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *p, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತ್ಯಾಗಮ addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(ಧ್ರುವ ಸಂರಚನಾ hw_addr_data *addr, uint16_t iftype) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ವಿಕಲ್ಪ (iftype) {
        ಪಕ್ಷ ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                ಯದಿ (addr->length != ETH_ALEN)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಪ್ರತ್ಯಾಗಮ !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        ಪಕ್ಷ ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                ಯದಿ (addr->length != INFINIBAND_ALEN)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಪ್ರತ್ಯಾಗಮ !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, ಸಂರಚನಾ hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(hw_addr_hash_ops_free, ಸಂರಚನಾ hw_addr_data, hw_addr_hash_func, hw_addr_compare, ಮುಕ್ತ);

ವರ್ಣ* ether_addr_to_string(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr, ವರ್ಣ buffer[ETHER_ADDR_TO_STRING_MAX]) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(buffer);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        ಪ್ರತ್ಯಾಗಮ buffer;
}

ಪೂರ್ಣಾಙ್ಕ ether_addr_compare(ಧ್ರುವ ಸಂರಚನಾ ether_addr *a, ಧ್ರುವ ಸಂರಚನಾ ether_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ memcmp(a, b, ETH_ALEN);
}

ಸ್ಥಿರ ಶೂನ್ಯ ether_addr_hash_func(ಧ್ರುವ ಸಂರಚನಾ ether_addr *p, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(ಧ್ರುವ ಸಂರಚನಾ ether_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತ್ಯಾಗಮ memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, ಸಂರಚನಾ ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(ether_addr_hash_ops_free, ಸಂರಚನಾ ether_addr, ether_addr_hash_func, ether_addr_compare, ಮುಕ್ತ);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_hw_addr_one_field(ಧ್ರುವ ವರ್ಣ **s, ವರ್ಣ sep, size_t len, uint8_t *buf) {
        ಧ್ರುವ ವರ್ಣ *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(*s);
        ಪ್ರತಿಜ್ಞಾ(IN_SET(len, 1, 2));
        ಪ್ರತಿಜ್ಞಾ(buf);

        p = *s;

        ಪ್ರತಿ (size_t i = 0; i < len * 2; i++) {
                ಧ್ರುವ ವರ್ಣ *hexoff;
                size_t x;

                ಯದಿ (*p == '\0' || *p == sep) {
                        ಯದಿ (i == 0)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        ಭಙ್ಗ;
                }

                hexoff = strchr(hex, *p);
                ಯದಿ (!hexoff)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತಿಜ್ಞಾ(hexoff >= hex);
                x = hexoff - hex;
                ಯದಿ (x >= 16)
                        x -= 6; /* A-F */

                ಪ್ರತಿಜ್ಞಾ(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        ಯದಿ (*p != '\0' && *p != sep)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ವಿಕಲ್ಪ (len) {
        ಪಕ್ಷ 1:
                buf[0] = data;
                ಭಙ್ಗ;
        ಪಕ್ಷ 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                ಭಙ್ಗ;
        ಸ್ವಭಾವ:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        ಪ್ರತ್ಯಾಗಮ cont;
}

ಪೂರ್ಣಾಙ್ಕ parse_hw_addr_full(ಧ್ರುವ ವರ್ಣ *s, size_t expected_len, ಸಂರಚನಾ hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        ವರ್ಣ sep;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

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

        ಯದಿ (IN_SET(expected_len, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr), ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr))) {
                ಸಙ್ಘ in_addr_union a;
                ಪೂರ್ಣಾಙ್ಕ family;

                ಯದಿ (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                ಅನ್ಯಥಾ {
                        family = expected_len == ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                ಯದಿ (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        ಯದಿ (sep == '.')
                field_size = 2;
        ಅನ್ಯಥಾ ಯದಿ (IN_SET(sep, ':', '-'))
                field_size = 1;
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (max_len % field_size != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತಿ (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0) {
                        len = (i + 1) * field_size;
                        ಭಙ್ಗ;
                }
        }

        ಯದಿ (len == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (expected_len == 0) {
                ಯದಿ (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        } ಅನ್ಯಥಾ ಯದಿ (expected_len != SIZE_MAX) {
                ಯದಿ (len != expected_len)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_ether_addr(ಧ್ರುವ ವರ್ಣ *s, ಸಂರಚನಾ ether_addr *ret) {
        ಸಂರಚನಾ hw_addr_data a;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = a.ether;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ ether_addr_mark_random(ಸಂರಚನಾ ether_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

ಪೂರ್ಣಾಙ್ಕ hw_addr_ensure_broadcast(ಸಂರಚನಾ hw_addr_data *bcast_addr, uint16_t arp_type) {
        ಪ್ರತಿಜ್ಞಾ(bcast_addr);

        ಯದಿ (!hw_addr_is_null(bcast_addr))
                ಪ್ರತ್ಯಾಗಮ 0;

        ವಿಕಲ್ಪ (arp_type) {
        ಪಕ್ಷ ARPHRD_ETHER:
                *bcast_addr = (ಸಂರಚನಾ hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                ಪ್ರತ್ಯಾಗಮ 0;
        ಪಕ್ಷ ARPHRD_INFINIBAND:
                *bcast_addr = (ಸಂರಚನಾ hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                ಪ್ರತ್ಯಾಗಮ 0;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}
