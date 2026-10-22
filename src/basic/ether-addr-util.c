/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <net/ethernet.h>
#ضمن <net/if_arp.h>
#ضمن <stdio.h>

#ضمن "ether-addr-util.h"
#ضمن "hash-funcs.h"
#ضمن "hexdecoct.h"
#ضمن "in-addr-util.h"
#ضمن "memory-util.h"
#ضمن "siphash24.h"
#ضمن "string-util.h"

حرف* hw_addr_منزل_string_ڀريل(
                ثابت بنية hw_addr_معلومات *addr,
                Hardware_پتو_منزل_String_Flags flags,
                حرف بفر[ساكن HW_ADDR_منزل_STRING_MAX]) {

        أكد(addr);
        أكد(بفر);
        أكد(addr->length <= HW_ADDR_MAX_سائيز);

        لكل (size_t i = 0, j = 0; i < addr->length; i++) {
                بفر[j++] = hexchar(addr->بائيٽون[i] >> 4);
                بفر[j++] = hexchar(addr->بائيٽون[i] & 0x0f);
                إذا (!FLAGS_سيٽ(flags, HW_ADDR_منزل_STRING_NO_COLON))
                        بفر[j++] = ':';
        }

        بفر[addr->length == 0 || FLAGS_سيٽ(flags, HW_ADDR_منزل_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        أرجع بفر;
}

بنية hw_addr_معلومات *hw_addr_سيٽ(بنية hw_addr_معلومات *addr, ثابت uint8_t *بائيٽون, size_t length) {
        أكد(addr);
        أكد(length <= HW_ADDR_MAX_سائيز);

        addr->length = length;
        memcpy_محفوظ(addr->بائيٽون, بائيٽون, length);
        أرجع addr;
}

صحيح hw_addr_compare(ثابت بنية hw_addr_معلومات *a, ثابت بنية hw_addr_معلومات *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = CMP(a->length, b->length);
        إذا (r != 0)
                أرجع r;

        أرجع memcmp(a->بائيٽون, b->بائيٽون, a->length);
}

فراغ hw_addr_hash_func(ثابت بنية hw_addr_معلومات *p, بنية siphash *state) {
        أكد(p);
        أكد(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_محفوظ(p->بائيٽون, p->length, state);
}

bool hw_addr_is_null(ثابت بنية hw_addr_معلومات *addr) {
        أكد(addr);
        أرجع addr->length == 0 || memeqzero(addr->بائيٽون, addr->length);
}

bool hw_addr_is_valid(ثابت بنية hw_addr_معلومات *addr, uint16_t iftype) {
        أكد(addr);

        بدل (iftype) {
        حالة ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                إذا (addr->length != ETH_ALEN)
                        أرجع false;

                أرجع !ether_addr_is_null(&addr->ether) && !ether_addr_is_نشر(&addr->ether);

        حالة ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                إذا (addr->length != INFINIBAND_ALEN)
                        أرجع false;

                أرجع !memeqzero(addr->بائيٽون + INFINIBAND_ALEN - 8, 8);

        افتراضي:
                أرجع false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, بنية hw_addr_معلومات, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_ڪنجي_DESTRUCTOR(hw_addr_hash_ops_free, بنية hw_addr_معلومات, hw_addr_hash_func, hw_addr_compare, حرر);

حرف* ether_addr_منزل_string(ثابت بنية ether_addr *addr, حرف بفر[ETHER_ADDR_منزل_STRING_MAX]) {
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

        أرجع بفر;
}

صحيح ether_addr_compare(ثابت بنية ether_addr *a, ثابت بنية ether_addr *b) {
        أكد(a);
        أكد(b);

        أرجع memcmp(a, b, ETH_ALEN);
}

ساكن فراغ ether_addr_hash_func(ثابت بنية ether_addr *p, بنية siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_نشر(ثابت بنية ether_addr *addr) {
        أكد(addr);
        أرجع memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, بنية ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_ڪنجي_DESTRUCTOR(ether_addr_hash_ops_free, بنية ether_addr, ether_addr_hash_func, ether_addr_compare, حرر);

ساكن صحيح parse_hw_addr_one_ميدان(ثابت حرف **s, حرف sep, size_t len, uint8_t *buf) {
        ثابت حرف *hex = HEXDIGITS, *p;
        uint16_t معلومات = 0;
        bool cont;

        أكد(s);
        أكد(*s);
        أكد(اندريون_سيٽ(len, 1, 2));
        أكد(buf);

        p = *s;

        لكل (size_t i = 0; i < len * 2; i++) {
                ثابت حرف *hexoff;
                size_t x;

                إذا (*p == '\0' || *p == sep) {
                        إذا (i == 0)
                                أرجع -EINVAL;
                        اكسر;
                }

                hexoff = strchr(hex, *p);
                إذا (!hexoff)
                        أرجع -EINVAL;

                أكد(hexoff >= hex);
                x = hexoff - hex;
                إذا (x >= 16)
                        x -= 6; /* A-F */

                أكد(x < 16);
                معلومات <<= 4;
                معلومات += x;

                p++;
        }

        إذا (*p != '\0' && *p != sep)
                أرجع -EINVAL;

        بدل (len) {
        حالة 1:
                buf[0] = معلومات;
                اكسر;
        حالة 2:
                buf[0] = (معلومات & 0xff00) >> 8;
                buf[1] = معلومات & 0xff;
                اكسر;
        افتراضي:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        أرجع cont;
}

صحيح parse_hw_addr_ڀريل(ثابت حرف *s, size_t expected_len, بنية hw_addr_معلومات *ret) {
        size_t ميدان_سائيز, max_len, len = 0;
        uint8_t بائيٽون[HW_ADDR_MAX_سائيز];
        حرف sep;
        صحيح r;

        أكد(s);
        أكد(expected_len <= HW_ADDR_MAX_سائيز || expected_len == SIZE_MAX);
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

        إذا (اندريون_سيٽ(expected_len, 0, حجم(بنية in_addr), حجم(بنية in6_addr))) {
                اتحاد اندريون_addr_union a;
                صحيح family;

                إذا (expected_len == 0)
                        r = اندريون_addr_from_string_auto(s, &family, &a);
                وإلا {
                        family = expected_len == حجم(بنية in_addr) ? AF_INET : AF_INET6;
                        r = اندريون_addr_from_string(family, s, &a);
                }
                إذا (r >= 0) {
                        ret->length = FAMILY_پتو_سائيز(family);
                        يادگيري_نقل(ret->بائيٽون, a.بائيٽون, ret->length);
                        أرجع 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_سائيز : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        إذا (sep == '.')
                ميدان_سائيز = 2;
        وإلا إذا (اندريون_سيٽ(sep, ':', '-'))
                ميدان_سائيز = 1;
        وإلا
                أرجع -EINVAL;

        إذا (max_len % ميدان_سائيز != 0)
                أرجع -EINVAL;

        لكل (size_t i = 0; i < max_len / ميدان_سائيز; i++) {
                r = parse_hw_addr_one_ميدان(&s, sep, ميدان_سائيز, بائيٽون + i * ميدان_سائيز);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0) {
                        len = (i + 1) * ميدان_سائيز;
                        اكسر;
                }
        }

        إذا (len == 0)
                أرجع -EINVAL;

        إذا (expected_len == 0) {
                إذا (!اندريون_سيٽ(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        أرجع -EINVAL;
        } وإلا إذا (expected_len != SIZE_MAX) {
                إذا (len != expected_len)
                        أرجع -EINVAL;
        }

        ret->length = len;
        يادگيري_نقل(ret->بائيٽون, بائيٽون, ret->length);
        أرجع 0;
}

صحيح parse_ether_addr(ثابت حرف *s, بنية ether_addr *ret) {
        بنية hw_addr_معلومات a;
        صحيح r;

        أكد(s);
        أكد(ret);

        r = parse_hw_addr_ڀريل(s, ETH_ALEN, &a);
        إذا (r < 0)
                أرجع r;

        *ret = a.ether;
        أرجع 0;
}

فراغ ether_addr_mark_random(بنية ether_addr *addr) {
        أكد(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

صحيح hw_addr_ensure_نشر(بنية hw_addr_معلومات *bcast_addr, uint16_t arp_type) {
        أكد(bcast_addr);

        إذا (!hw_addr_is_null(bcast_addr))
                أرجع 0;

        بدل (arp_type) {
        حالة ARPHRD_ETHER:
                *bcast_addr = (بنية hw_addr_معلومات) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                أرجع 0;
        حالة ARPHRD_INFINIBAND:
                *bcast_addr = (بنية hw_addr_معلومات) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                أرجع 0;
        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}
