/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <net/ethernet.h>
#অন্তর্ভাব <net/if_arp.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "ether-addr-util.h"
#অন্তর্ভাব "hash-funcs.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "in-addr-util.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "siphash24.h"
#অন্তর্ভাব "string-util.h"

অক্ষর* hw_addr_to_string_full(
                ধ্রুবক কাঠামো hw_addr_তথ্য *addr,
                HardwareAddressToStringFlags flags,
                অক্ষর বাফার[স্থির HW_ADDR_TO_STRING_MAX]) {

        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(addr->length <= HW_ADDR_MAX_আকার);

        জন্য (size_t i = 0, j = 0; i < addr->length; i++) {
                বাফার[j++] = hexchar(addr->bytes[i] >> 4);
                বাফার[j++] = hexchar(addr->bytes[i] & 0x0f);
                যদি (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        বাফার[j++] = ':';
        }

        বাফার[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        ফেরত বাফার;
}

কাঠামো hw_addr_তথ্য *hw_addr_set(কাঠামো hw_addr_তথ্য *addr, ধ্রুবক uint8_t *bytes, size_t length) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(length <= HW_ADDR_MAX_আকার);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        ফেরত addr;
}

পূর্ণসংখ্যা hw_addr_compare(ধ্রুবক কাঠামো hw_addr_তথ্য *a, ধ্রুবক কাঠামো hw_addr_তথ্য *b) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = CMP(a->length, b->length);
        যদি (r != 0)
                ফেরত r;

        ফেরত memcmp(a->bytes, b->bytes, a->length);
}

শূন্য hw_addr_hash_func(ধ্রুবক কাঠামো hw_addr_তথ্য *p, কাঠামো siphash *state) {
        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(ধ্রুবক কাঠামো hw_addr_তথ্য *addr) {
        প্রতিজ্ঞা(addr);
        ফেরত addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(ধ্রুবক কাঠামো hw_addr_তথ্য *addr, uint16_t iftype) {
        প্রতিজ্ঞা(addr);

        নির্বাচন (iftype) {
        ক্ষেত্র ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                যদি (addr->length != ETH_ALEN)
                        ফেরত false;

                ফেরত !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        ক্ষেত্র ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                যদি (addr->length != INFINIBAND_ALEN)
                        ফেরত false;

                ফেরত !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        পূর্বনির্ধারিত:
                ফেরত false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, কাঠামো hw_addr_তথ্য, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(hw_addr_hash_ops_free, কাঠামো hw_addr_তথ্য, hw_addr_hash_func, hw_addr_compare, মুক্ত);

অক্ষর* ether_addr_to_string(ধ্রুবক কাঠামো ether_addr *addr, অক্ষর বাফার[ETHER_ADDR_TO_STRING_MAX]) {
        প্রতিজ্ঞা(addr);
        প্রতিজ্ঞা(বাফার);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        sprintf(বাফার, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        ফেরত বাফার;
}

পূর্ণসংখ্যা ether_addr_compare(ধ্রুবক কাঠামো ether_addr *a, ধ্রুবক কাঠামো ether_addr *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ফেরত memcmp(a, b, ETH_ALEN);
}

স্থির শূন্য ether_addr_hash_func(ধ্রুবক কাঠামো ether_addr *p, কাঠামো siphash *state) {
        প্রতিজ্ঞা(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(ধ্রুবক কাঠামো ether_addr *addr) {
        প্রতিজ্ঞা(addr);
        ফেরত memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, কাঠামো ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(ether_addr_hash_ops_free, কাঠামো ether_addr, ether_addr_hash_func, ether_addr_compare, মুক্ত);

স্থির পূর্ণসংখ্যা parse_hw_addr_one_field(ধ্রুবক অক্ষর **s, অক্ষর sep, size_t len, uint8_t *buf) {
        ধ্রুবক অক্ষর *hex = HEXDIGITS, *p;
        uint16_t তথ্য = 0;
        bool cont;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(*s);
        প্রতিজ্ঞা(IN_SET(len, 1, 2));
        প্রতিজ্ঞা(buf);

        p = *s;

        জন্য (size_t i = 0; i < len * 2; i++) {
                ধ্রুবক অক্ষর *hexoff;
                size_t x;

                যদি (*p == '\0' || *p == sep) {
                        যদি (i == 0)
                                ফেরত -EINVAL;
                        বিরতি;
                }

                hexoff = strchr(hex, *p);
                যদি (!hexoff)
                        ফেরত -EINVAL;

                প্রতিজ্ঞা(hexoff >= hex);
                x = hexoff - hex;
                যদি (x >= 16)
                        x -= 6; /* A-F */

                প্রতিজ্ঞা(x < 16);
                তথ্য <<= 4;
                তথ্য += x;

                p++;
        }

        যদি (*p != '\0' && *p != sep)
                ফেরত -EINVAL;

        নির্বাচন (len) {
        ক্ষেত্র 1:
                buf[0] = তথ্য;
                বিরতি;
        ক্ষেত্র 2:
                buf[0] = (তথ্য & 0xff00) >> 8;
                buf[1] = তথ্য & 0xff;
                বিরতি;
        পূর্বনির্ধারিত:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        ফেরত cont;
}

পূর্ণসংখ্যা parse_hw_addr_full(ধ্রুবক অক্ষর *s, size_t expected_len, কাঠামো hw_addr_তথ্য *ret) {
        size_t field_আকার, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_আকার];
        অক্ষর sep;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(expected_len <= HW_ADDR_MAX_আকার || expected_len == SIZE_MAX);
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

        যদি (IN_SET(expected_len, 0, আকার(কাঠামো in_addr), আকার(কাঠামো in6_addr))) {
                সংঘ in_addr_union a;
                পূর্ণসংখ্যা family;

                যদি (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                অন্যথা {
                        family = expected_len == আকার(কাঠামো in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                যদি (r >= 0) {
                        ret->length = FAMILY_ADDRESS_আকার(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        ফেরত 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_আকার : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        যদি (sep == '.')
                field_আকার = 2;
        অন্যথা যদি (IN_SET(sep, ':', '-'))
                field_আকার = 1;
        অন্যথা
                ফেরত -EINVAL;

        যদি (max_len % field_আকার != 0)
                ফেরত -EINVAL;

        জন্য (size_t i = 0; i < max_len / field_আকার; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_আকার, bytes + i * field_আকার);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0) {
                        len = (i + 1) * field_আকার;
                        বিরতি;
                }
        }

        যদি (len == 0)
                ফেরত -EINVAL;

        যদি (expected_len == 0) {
                যদি (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        ফেরত -EINVAL;
        } অন্যথা যদি (expected_len != SIZE_MAX) {
                যদি (len != expected_len)
                        ফেরত -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        ফেরত 0;
}

পূর্ণসংখ্যা parse_ether_addr(ধ্রুবক অক্ষর *s, কাঠামো ether_addr *ret) {
        কাঠামো hw_addr_তথ্য a;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        যদি (r < 0)
                ফেরত r;

        *ret = a.ether;
        ফেরত 0;
}

শূন্য ether_addr_mark_random(কাঠামো ether_addr *addr) {
        প্রতিজ্ঞা(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

পূর্ণসংখ্যা hw_addr_ensure_broadcast(কাঠামো hw_addr_তথ্য *bcast_addr, uint16_t arp_type) {
        প্রতিজ্ঞা(bcast_addr);

        যদি (!hw_addr_is_null(bcast_addr))
                ফেরত 0;

        নির্বাচন (arp_type) {
        ক্ষেত্র ARPHRD_ETHER:
                *bcast_addr = (কাঠামো hw_addr_তথ্য) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                ফেরত 0;
        ক্ষেত্র ARPHRD_INFINIBAND:
                *bcast_addr = (কাঠামো hw_addr_তথ্য) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                ফেরত 0;
        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}
