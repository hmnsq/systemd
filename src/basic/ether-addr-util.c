/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <net/ethernet.h>
#شامل <net/if_arp.h>
#شامل <stdio.h>

#شامل "ether-addr-util.h"
#شامل "hash-funcs.h"
#شامل "hexdecoct.h"
#شامل "in-addr-util.h"
#شامل "memory-util.h"
#شامل "siphash24.h"
#شامل "string-util.h"

حرف* hw_addr_مقصد_string_بھرا(
                ثابت ساخت hw_addr_ڈیٹا *addr,
                Hardware_پتہ_مقصد_String_Flags flags,
                حرف بفر[ساکن HW_ADDR_مقصد_STRING_MAX]) {

        أكد(addr);
        أكد(بفر);
        أكد(addr->length <= HW_ADDR_MAX_سائز);

        برائے (size_t i = 0, j = 0; i < addr->length; i++) {
                بفر[j++] = hexchar(addr->بائٹیں[i] >> 4);
                بفر[j++] = hexchar(addr->بائٹیں[i] & 0x0f);
                اگر (!FLAGS_سیٹ(flags, HW_ADDR_مقصد_STRING_NO_COLON))
                        بفر[j++] = ':';
        }

        بفر[addr->length == 0 || FLAGS_سیٹ(flags, HW_ADDR_مقصد_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        واپس بفر;
}

ساخت hw_addr_ڈیٹا *hw_addr_سیٹ(ساخت hw_addr_ڈیٹا *addr, ثابت uint8_t *بائٹیں, size_t length) {
        أكد(addr);
        أكد(length <= HW_ADDR_MAX_سائز);

        addr->length = length;
        memcpy_محفوظ(addr->بائٹیں, بائٹیں, length);
        واپس addr;
}

صحیح hw_addr_compare(ثابت ساخت hw_addr_ڈیٹا *a, ثابت ساخت hw_addr_ڈیٹا *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(a->length, b->length);
        اگر (r != 0)
                واپس r;

        واپس memcmp(a->بائٹیں, b->بائٹیں, a->length);
}

خالی hw_addr_hash_func(ثابت ساخت hw_addr_ڈیٹا *p, ساخت siphash *state) {
        أكد(p);
        أكد(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_محفوظ(p->بائٹیں, p->length, state);
}

bool hw_addr_is_null(ثابت ساخت hw_addr_ڈیٹا *addr) {
        أكد(addr);
        واپس addr->length == 0 || memeqzero(addr->بائٹیں, addr->length);
}

bool hw_addr_is_valid(ثابت ساخت hw_addr_ڈیٹا *addr, uint16_t iftype) {
        أكد(addr);

        انتخاب (iftype) {
        حالت ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                اگر (addr->length != ETH_ALEN)
                        واپس false;

                واپس !ether_addr_is_null(&addr->ether) && !ether_addr_is_نشر(&addr->ether);

        حالت ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                اگر (addr->length != INFINIBAND_ALEN)
                        واپس false;

                واپس !memeqzero(addr->بائٹیں + INFINIBAND_ALEN - 8, 8);

        طےشدہ:
                واپس false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, ساخت hw_addr_ڈیٹا, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(hw_addr_hash_ops_free, ساخت hw_addr_ڈیٹا, hw_addr_hash_func, hw_addr_compare, آزاد);

حرف* ether_addr_مقصد_string(ثابت ساخت ether_addr *addr, حرف بفر[ETHER_ADDR_مقصد_STRING_MAX]) {
        أكد(addr);
        أكد(بفر);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        sprintf(بفر, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        واپس بفر;
}

صحیح ether_addr_compare(ثابت ساخت ether_addr *a, ثابت ساخت ether_addr *b) {
        أكد(a);
        أكد(b);

        واپس memcmp(a, b, ETH_ALEN);
}

ساکن خالی ether_addr_hash_func(ثابت ساخت ether_addr *p, ساخت siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_نشر(ثابت ساخت ether_addr *addr) {
        أكد(addr);
        واپس memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, ساخت ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(ether_addr_hash_ops_free, ساخت ether_addr, ether_addr_hash_func, ether_addr_compare, آزاد);

ساکن صحیح parse_hw_addr_one_میدان(ثابت حرف **s, حرف sep, size_t len, uint8_t *buf) {
        ثابت حرف *hex = HEXDIGITS, *p;
        uint16_t ڈیٹا = 0;
        bool cont;

        أكد(s);
        أكد(*s);
        أكد(اندر_سیٹ(len, 1, 2));
        أكد(buf);

        p = *s;

        برائے (size_t i = 0; i < len * 2; i++) {
                ثابت حرف *hexoff;
                size_t x;

                اگر (*p == '\0' || *p == sep) {
                        اگر (i == 0)
                                واپس -EINVAL;
                        توڑو;
                }

                hexoff = strchr(hex, *p);
                اگر (!hexoff)
                        واپس -EINVAL;

                أكد(hexoff >= hex);
                x = hexoff - hex;
                اگر (x >= 16)
                        x -= 6; /* A-F */

                أكد(x < 16);
                ڈیٹا <<= 4;
                ڈیٹا += x;

                p++;
        }

        اگر (*p != '\0' && *p != sep)
                واپس -EINVAL;

        انتخاب (len) {
        حالت 1:
                buf[0] = ڈیٹا;
                توڑو;
        حالت 2:
                buf[0] = (ڈیٹا & 0xff00) >> 8;
                buf[1] = ڈیٹا & 0xff;
                توڑو;
        طےشدہ:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        واپس cont;
}

صحیح parse_hw_addr_بھرا(ثابت حرف *s, size_t expected_len, ساخت hw_addr_ڈیٹا *ret) {
        size_t میدان_سائز, max_len, len = 0;
        uint8_t بائٹیں[HW_ADDR_MAX_سائز];
        حرف sep;
        صحیح r;

        أكد(s);
        أكد(expected_len <= HW_ADDR_MAX_سائز || expected_len == SIZE_MAX);
        أكد(ret);

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

        اگر (اندر_سیٹ(expected_len, 0, حجم(ساخت in_addr), حجم(ساخت in6_addr))) {
                اتحاد اندر_addr_union a;
                صحیح family;

                اگر (expected_len == 0)
                        r = اندر_addr_from_string_auto(s, &family, &a);
                ورنہ {
                        family = expected_len == حجم(ساخت in_addr) ? AF_INET : AF_INET6;
                        r = اندر_addr_from_string(family, s, &a);
                }
                اگر (r >= 0) {
                        ret->length = FAMILY_پتہ_سائز(family);
                        حافظہ_نقل(ret->بائٹیں, a.بائٹیں, ret->length);
                        واپس 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_سائز : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        اگر (sep == '.')
                میدان_سائز = 2;
        ورنہ اگر (اندر_سیٹ(sep, ':', '-'))
                میدان_سائز = 1;
        ورنہ
                واپس -EINVAL;

        اگر (max_len % میدان_سائز != 0)
                واپس -EINVAL;

        برائے (size_t i = 0; i < max_len / میدان_سائز; i++) {
                r = parse_hw_addr_one_میدان(&s, sep, میدان_سائز, بائٹیں + i * میدان_سائز);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0) {
                        len = (i + 1) * میدان_سائز;
                        توڑو;
                }
        }

        اگر (len == 0)
                واپس -EINVAL;

        اگر (expected_len == 0) {
                اگر (!اندر_سیٹ(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        واپس -EINVAL;
        } ورنہ اگر (expected_len != SIZE_MAX) {
                اگر (len != expected_len)
                        واپس -EINVAL;
        }

        ret->length = len;
        حافظہ_نقل(ret->بائٹیں, بائٹیں, ret->length);
        واپس 0;
}

صحیح parse_ether_addr(ثابت حرف *s, ساخت ether_addr *ret) {
        ساخت hw_addr_ڈیٹا a;
        صحیح r;

        أكد(s);
        أكد(ret);

        r = parse_hw_addr_بھرا(s, ETH_ALEN, &a);
        اگر (r < 0)
                واپس r;

        *ret = a.ether;
        واپس 0;
}

خالی ether_addr_mark_random(ساخت ether_addr *addr) {
        أكد(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

صحیح hw_addr_ensure_نشر(ساخت hw_addr_ڈیٹا *bcast_addr, uint16_t arp_type) {
        أكد(bcast_addr);

        اگر (!hw_addr_is_null(bcast_addr))
                واپس 0;

        انتخاب (arp_type) {
        حالت ARPHRD_ETHER:
                *bcast_addr = (ساخت hw_addr_ڈیٹا) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                واپس 0;
        حالت ARPHRD_INFINIBAND:
                *bcast_addr = (ساخت hw_addr_ڈیٹا) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                واپس 0;
        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}
