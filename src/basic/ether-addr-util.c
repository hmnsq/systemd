/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <net/ethernet.h>
#অন্তর্ভাৱ <net/if_arp.h>
#অন্তর্ভাৱ <stdio.h>

#অন্তর্ভাৱ "ether-addr-util.h"
#অন্তর্ভাৱ "hash-funcs.h"
#অন্তর্ভাৱ "hexdecoct.h"
#অন্তর্ভাৱ "in-addr-util.h"
#অন্তর্ভাৱ "memory-util.h"
#অন্তর্ভাৱ "siphash24.h"
#অন্তর্ভাৱ "string-util.h"

ৱর্ণ* hw_addr_to_string_full(
                ধ্রুৱ সংরচনা hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                ৱর্ণ buffer[স্থির HW_ADDR_TO_STRING_MAX]) {

        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(addr->length <= HW_ADDR_MAX_SIZE);

        বাবে (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                যদি (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        ঘূৰাই buffer;
}

সংরচনা hw_addr_data *hw_addr_set(সংরচনা hw_addr_data *addr, ধ্রুৱ uint8_t *bytes, size_t length) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        ঘূৰাই addr;
}

পূর্ণাঙ্ক hw_addr_compare(ধ্রুৱ সংরচনা hw_addr_data *a, ধ্রুৱ সংরচনা hw_addr_data *b) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = CMP(a->length, b->length);
        যদি (r != 0)
                ঘূৰাই r;

        ঘূৰাই memcmp(a->bytes, b->bytes, a->length);
}

শূন্য hw_addr_hash_func(ধ্রুৱ সংরচনা hw_addr_data *p, সংরচনা siphash *state) {
        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(ধ্রুৱ সংরচনা hw_addr_data *addr) {
        প্রতিজ্ঞা(addr);
        ঘূৰাই addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(ধ্রুৱ সংরচনা hw_addr_data *addr, uint16_t iftype) {
        প্রতিজ্ঞা(addr);

        ৱিকল্প (iftype) {
        পক্ষ ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                যদি (addr->length != ETH_ALEN)
                        ঘূৰাই false;

                ঘূৰাই !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        পক্ষ ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                যদি (addr->length != INFINIBAND_ALEN)
                        ঘূৰাই false;

                ঘূৰাই !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        স্ৱভাৱ:
                ঘূৰাই false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, সংরচনা hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(hw_addr_hash_ops_free, সংরচনা hw_addr_data, hw_addr_hash_func, hw_addr_compare, মুক্ত);

ৱর্ণ* ether_addr_to_string(ধ্রুৱ সংরচনা ether_addr *addr, ৱর্ণ buffer[ETHER_ADDR_TO_STRING_MAX]) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(buffer);

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

        ঘূৰাই buffer;
}

পূর্ণাঙ্ক ether_addr_compare(ধ্রুৱ সংরচনা ether_addr *a, ধ্রুৱ সংরচনা ether_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ঘূৰাই memcmp(a, b, ETH_ALEN);
}

স্থির শূন্য ether_addr_hash_func(ধ্রুৱ সংরচনা ether_addr *p, সংরচনা siphash *state) {
        প্রতিজ্ঞা(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(ধ্রুৱ সংরচনা ether_addr *addr) {
        প্রতিজ্ঞা(addr);
        ঘূৰাই memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, সংরচনা ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(ether_addr_hash_ops_free, সংরচনা ether_addr, ether_addr_hash_func, ether_addr_compare, মুক্ত);

স্থির পূর্ণাঙ্ক parse_hw_addr_one_field(ধ্রুৱ ৱর্ণ **s, ৱর্ণ sep, size_t len, uint8_t *buf) {
        ধ্রুৱ ৱর্ণ *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(*s);
        প্রতিজ্ঞা(IN_SET(len, 1, 2));
        প্রতিজ্ঞা(buf);

        p = *s;

        বাবে (size_t i = 0; i < len * 2; i++) {
                ধ্রুৱ ৱর্ণ *hexoff;
                size_t x;

                যদি (*p == '\0' || *p == sep) {
                        যদি (i == 0)
                                ঘূৰাই -EINVAL;
                        ভঙ্গ;
                }

                hexoff = strchr(hex, *p);
                যদি (!hexoff)
                        ঘূৰাই -EINVAL;

                প্রতিজ্ঞা(hexoff >= hex);
                x = hexoff - hex;
                যদি (x >= 16)
                        x -= 6; /* A-F */

                প্রতিজ্ঞা(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        যদি (*p != '\0' && *p != sep)
                ঘূৰাই -EINVAL;

        ৱিকল্প (len) {
        পক্ষ 1:
                buf[0] = data;
                ভঙ্গ;
        পক্ষ 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                ভঙ্গ;
        স্ৱভাৱ:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        ঘূৰাই cont;
}

পূর্ণাঙ্ক parse_hw_addr_full(ধ্রুৱ ৱর্ণ *s, size_t expected_len, সংরচনা hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        ৱর্ণ sep;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        প্রতিজ্ঞা(ret);

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

        যদি (IN_SET(expected_len, 0, পরিমাণ(সংরচনা in_addr), পরিমাণ(সংরচনা in6_addr))) {
                সঙ্ঘ in_addr_union a;
                পূর্ণাঙ্ক family;

                যদি (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                অন্যথা {
                        family = expected_len == পরিমাণ(সংরচনা in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                যদি (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        ঘূৰাই 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        যদি (sep == '.')
                field_size = 2;
        অন্যথা যদি (IN_SET(sep, ':', '-'))
                field_size = 1;
        অন্যথা
                ঘূৰাই -EINVAL;

        যদি (max_len % field_size != 0)
                ঘূৰাই -EINVAL;

        বাবে (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0) {
                        len = (i + 1) * field_size;
                        ভঙ্গ;
                }
        }

        যদি (len == 0)
                ঘূৰাই -EINVAL;

        যদি (expected_len == 0) {
                যদি (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        ঘূৰাই -EINVAL;
        } অন্যথা যদি (expected_len != SIZE_MAX) {
                যদি (len != expected_len)
                        ঘূৰাই -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_ether_addr(ধ্রুৱ ৱর্ণ *s, সংরচনা ether_addr *ret) {
        সংরচনা hw_addr_data a;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = a.ether;
        ঘূৰাই 0;
}

শূন্য ether_addr_mark_random(সংরচনা ether_addr *addr) {
        প্রতিজ্ঞা(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

পূর্ণাঙ্ক hw_addr_ensure_broadcast(সংরচনা hw_addr_data *bcast_addr, uint16_t arp_type) {
        প্রতিজ্ঞা(bcast_addr);

        যদি (!hw_addr_is_null(bcast_addr))
                ঘূৰাই 0;

        ৱিকল্প (arp_type) {
        পক্ষ ARPHRD_ETHER:
                *bcast_addr = (সংরচনা hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                ঘূৰাই 0;
        পক্ষ ARPHRD_INFINIBAND:
                *bcast_addr = (সংরচনা hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                ঘূৰাই 0;
        স্ৱভাৱ:
                ঘূৰাই -EAFNOSUPPORT;
        }
}
