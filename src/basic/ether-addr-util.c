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

ಅಕ್ಷರ* hw_addr_ಗಮ್ಯ_string_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr,
                Hardware_ವಿಳಾಸ_ಗಮ್ಯ_String_Flags flags,
                ಅಕ್ಷರ ಬಫರ್[ಸ್ಥಿರ HW_ADDR_ಗಮ್ಯ_STRING_MAX]) {

        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(addr->length <= HW_ADDR_MAX_ಗಾತ್ರ);

        ಗಾಗಿ (size_t i = 0, j = 0; i < addr->length; i++) {
                ಬಫರ್[j++] = hexchar(addr->ಬೈಟುಗಳು[i] >> 4);
                ಬಫರ್[j++] = hexchar(addr->ಬೈಟುಗಳು[i] & 0x0f);
                ಆದರೆ (!FLAGS_ಗಣ(flags, HW_ADDR_ಗಮ್ಯ_STRING_NO_COLON))
                        ಬಫರ್[j++] = ':';
        }

        ಬಫರ್[addr->length == 0 || FLAGS_ಗಣ(flags, HW_ADDR_ಗಮ್ಯ_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        ಹಿಂತಿರುಗಿ ಬಫರ್;
}

ರಚನೆ hw_addr_ದತ್ತಾಂಶ *hw_addr_ಗಣ(ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr, ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು, size_t length) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(length <= HW_ADDR_MAX_ಗಾತ್ರ);

        addr->length = length;
        memcpy_ಸುರಕ್ಷಿತ(addr->ಬೈಟುಗಳು, ಬೈಟುಗಳು, length);
        ಹಿಂತಿರುಗಿ addr;
}

ಪೂರ್ಣಾಂಕ hw_addr_compare(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *a, ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(a->length, b->length);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ memcmp(a->ಬೈಟುಗಳು, b->ಬೈಟುಗಳು, a->length);
}

ಶೂನ್ಯ hw_addr_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *p, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_ಸುರಕ್ಷಿತ(p->ಬೈಟುಗಳು, p->length, state);
}

bool hw_addr_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಹಿಂತಿರುಗಿ addr->length == 0 || memeqzero(addr->ಬೈಟುಗಳು, addr->length);
}

bool hw_addr_is_valid(ಸ್ಥಿರಾಂಕ ರಚನೆ hw_addr_ದತ್ತಾಂಶ *addr, uint16_t iftype) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        ಆಯ್ಕೆ (iftype) {
        ಸ್ಥಿತಿ ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                ಆದರೆ (addr->length != ETH_ALEN)
                        ಹಿಂತಿರುಗಿ false;

                ಹಿಂತಿರುಗಿ !ether_addr_is_null(&addr->ether) && !ether_addr_is_ಪ್ರಸಾರ(&addr->ether);

        ಸ್ಥಿತಿ ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                ಆದರೆ (addr->length != INFINIBAND_ALEN)
                        ಹಿಂತಿರುಗಿ false;

                ಹಿಂತಿರುಗಿ !memeqzero(addr->ಬೈಟುಗಳು + INFINIBAND_ALEN - 8, 8);

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, ರಚನೆ hw_addr_ದತ್ತಾಂಶ, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(hw_addr_hash_ops_free, ರಚನೆ hw_addr_ದತ್ತಾಂಶ, hw_addr_hash_func, hw_addr_compare, ಬಿಡುಗಡೆ);

ಅಕ್ಷರ* ether_addr_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr, ಅಕ್ಷರ ಬಫರ್[ETHER_ADDR_ಗಮ್ಯ_STRING_MAX]) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        sprintf(ಬಫರ್, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        ಹಿಂತಿರುಗಿ ಬಫರ್;
}

ಪೂರ್ಣಾಂಕ ether_addr_compare(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *a, ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ memcmp(a, b, ETH_ALEN);
}

ಸ್ಥಿರ ಶೂನ್ಯ ether_addr_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *p, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_ಪ್ರಸಾರ(ಸ್ಥಿರಾಂಕ ರಚನೆ ether_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);
        ಹಿಂತಿರುಗಿ memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, ರಚನೆ ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(ether_addr_hash_ops_free, ರಚನೆ ether_addr, ether_addr_hash_func, ether_addr_compare, ಬಿಡುಗಡೆ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_hw_addr_one_ಕ್ಷೇತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **s, ಅಕ್ಷರ sep, size_t len, uint8_t *buf) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hex = HEXDIGITS, *p;
        uint16_t ದತ್ತಾಂಶ = 0;
        bool cont;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(*s);
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(len, 1, 2));
        ಪ್ರತಿಜ್ಞಾ(buf);

        p = *s;

        ಗಾಗಿ (size_t i = 0; i < len * 2; i++) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hexoff;
                size_t x;

                ಆದರೆ (*p == '\0' || *p == sep) {
                        ಆದರೆ (i == 0)
                                ಹಿಂತಿರುಗಿ -EINVAL;
                        ನಿಲ್ಲಿಸು;
                }

                hexoff = strchr(hex, *p);
                ಆದರೆ (!hexoff)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಪ್ರತಿಜ್ಞಾ(hexoff >= hex);
                x = hexoff - hex;
                ಆದರೆ (x >= 16)
                        x -= 6; /* A-F */

                ಪ್ರತಿಜ್ಞಾ(x < 16);
                ದತ್ತಾಂಶ <<= 4;
                ದತ್ತಾಂಶ += x;

                p++;
        }

        ಆದರೆ (*p != '\0' && *p != sep)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆಯ್ಕೆ (len) {
        ಸ್ಥಿತಿ 1:
                buf[0] = ದತ್ತಾಂಶ;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 2:
                buf[0] = (ದತ್ತಾಂಶ & 0xff00) >> 8;
                buf[1] = ದತ್ತಾಂಶ & 0xff;
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        ಹಿಂತಿರುಗಿ cont;
}

ಪೂರ್ಣಾಂಕ parse_hw_addr_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t expected_len, ರಚನೆ hw_addr_ದತ್ತಾಂಶ *ret) {
        size_t ಕ್ಷೇತ್ರ_ಗಾತ್ರ, max_len, len = 0;
        uint8_t ಬೈಟುಗಳು[HW_ADDR_MAX_ಗಾತ್ರ];
        ಅಕ್ಷರ sep;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(expected_len <= HW_ADDR_MAX_ಗಾತ್ರ || expected_len == SIZE_MAX);
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

        ಆದರೆ (ಒಳ_ಗಣ(expected_len, 0, ಗಾತ್ರ(ರಚನೆ in_addr), ಗಾತ್ರ(ರಚನೆ in6_addr))) {
                ಒಕ್ಕೂಟ ಒಳ_addr_union a;
                ಪೂರ್ಣಾಂಕ family;

                ಆದರೆ (expected_len == 0)
                        r = ಒಳ_addr_from_string_auto(s, &family, &a);
                ಇಲ್ಲದಿದ್ದರೆ {
                        family = expected_len == ಗಾತ್ರ(ರಚನೆ in_addr) ? AF_INET : AF_INET6;
                        r = ಒಳ_addr_from_string(family, s, &a);
                }
                ಆದರೆ (r >= 0) {
                        ret->length = FAMILY_ವಿಳಾಸ_ಗಾತ್ರ(family);
                        ಸ್ಮರಣೆ_ನಕಲು(ret->ಬೈಟುಗಳು, a.ಬೈಟುಗಳು, ret->length);
                        ಹಿಂತಿರುಗಿ 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_ಗಾತ್ರ : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        ಆದರೆ (sep == '.')
                ಕ್ಷೇತ್ರ_ಗಾತ್ರ = 2;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(sep, ':', '-'))
                ಕ್ಷೇತ್ರ_ಗಾತ್ರ = 1;
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (max_len % ಕ್ಷೇತ್ರ_ಗಾತ್ರ != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಗಾಗಿ (size_t i = 0; i < max_len / ಕ್ಷೇತ್ರ_ಗಾತ್ರ; i++) {
                r = parse_hw_addr_one_ಕ್ಷೇತ್ರ(&s, sep, ಕ್ಷೇತ್ರ_ಗಾತ್ರ, ಬೈಟುಗಳು + i * ಕ್ಷೇತ್ರ_ಗಾತ್ರ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0) {
                        len = (i + 1) * ಕ್ಷೇತ್ರ_ಗಾತ್ರ;
                        ನಿಲ್ಲಿಸು;
                }
        }

        ಆದರೆ (len == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (expected_len == 0) {
                ಆದರೆ (!ಒಳ_ಗಣ(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        ಹಿಂತಿರುಗಿ -EINVAL;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (expected_len != SIZE_MAX) {
                ಆದರೆ (len != expected_len)
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ret->length = len;
        ಸ್ಮರಣೆ_ನಕಲು(ret->ಬೈಟುಗಳು, ಬೈಟುಗಳು, ret->length);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_ether_addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ರಚನೆ ether_addr *ret) {
        ರಚನೆ hw_addr_ದತ್ತಾಂಶ a;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_hw_addr_ತುಂಬಿದ(s, ETH_ALEN, &a);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = a.ether;
        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ ether_addr_mark_random(ರಚನೆ ether_addr *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

ಪೂರ್ಣಾಂಕ hw_addr_ensure_ಪ್ರಸಾರ(ರಚನೆ hw_addr_ದತ್ತಾಂಶ *bcast_addr, uint16_t arp_type) {
        ಪ್ರತಿಜ್ಞಾ(bcast_addr);

        ಆದರೆ (!hw_addr_is_null(bcast_addr))
                ಹಿಂತಿರುಗಿ 0;

        ಆಯ್ಕೆ (arp_type) {
        ಸ್ಥಿತಿ ARPHRD_ETHER:
                *bcast_addr = (ರಚನೆ hw_addr_ದತ್ತಾಂಶ) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                ಹಿಂತಿರುಗಿ 0;
        ಸ್ಥಿತಿ ARPHRD_INFINIBAND:
                *bcast_addr = (ರಚನೆ hw_addr_ದತ್ತಾಂಶ) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                ಹಿಂತಿರುಗಿ 0;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}
