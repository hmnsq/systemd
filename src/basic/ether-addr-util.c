/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <net/ethernet.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if_arp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "ether-addr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash-funcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "in-addr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਅੱਖਰ* hw_addr_ਮੰਜ਼ਿਲ_string_ਭਰਿਆ(
                ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr,
                Hardware_ਪਤਾ_ਮੰਜ਼ਿਲ_String_Flags flags,
                ਅੱਖਰ ਬਫਰ[ਸਥਿਰ HW_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX]) {

        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(addr->length <= HW_ADDR_MAX_ਮਾਪ);

        ਲਈ (size_t i = 0, j = 0; i < addr->length; i++) {
                ਬਫਰ[j++] = hexchar(addr->ਬਾਈਟਾਂ[i] >> 4);
                ਬਫਰ[j++] = hexchar(addr->ਬਾਈਟਾਂ[i] & 0x0f);
                ਜੇ (!FLAGS_ਸੈੱਟ(flags, HW_ADDR_ਮੰਜ਼ਿਲ_STRING_NO_COLON))
                        ਬਫਰ[j++] = ':';
        }

        ਬਫਰ[addr->length == 0 || FLAGS_ਸੈੱਟ(flags, HW_ADDR_ਮੰਜ਼ਿਲ_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        ਵਾਪਸ ਬਫਰ;
}

ਬਣਤਰ hw_addr_ਅੰਕੜੇ *hw_addr_ਸੈੱਟ(ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr, ਅਚਲ uint8_t *ਬਾਈਟਾਂ, size_t length) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(length <= HW_ADDR_MAX_ਮਾਪ);

        addr->length = length;
        memcpy_ਸੁਰੱਖਿਅਤ(addr->ਬਾਈਟਾਂ, ਬਾਈਟਾਂ, length);
        ਵਾਪਸ addr;
}

ਪੂਰਨ hw_addr_compare(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *a, ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *b) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        r = CMP(a->length, b->length);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਵਾਪਸ memcmp(a->ਬਾਈਟਾਂ, b->ਬਾਈਟਾਂ, a->length);
}

ਖਾਲੀ hw_addr_hash_func(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *p, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_ਸੁਰੱਖਿਅਤ(p->ਬਾਈਟਾਂ, p->length, state);
}

bool hw_addr_is_null(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਵਾਪਸ addr->length == 0 || memeqzero(addr->ਬਾਈਟਾਂ, addr->length);
}

bool hw_addr_is_valid(ਅਚਲ ਬਣਤਰ hw_addr_ਅੰਕੜੇ *addr, uint16_t iftype) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        ਚੋਣ (iftype) {
        ਹਾਲਤ ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                ਜੇ (addr->length != ETH_ALEN)
                        ਵਾਪਸ false;

                ਵਾਪਸ !ether_addr_is_null(&addr->ether) && !ether_addr_is_ਪ੍ਰਸਾਰਣ(&addr->ether);

        ਹਾਲਤ ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                ਜੇ (addr->length != INFINIBAND_ALEN)
                        ਵਾਪਸ false;

                ਵਾਪਸ !memeqzero(addr->ਬਾਈਟਾਂ + INFINIBAND_ALEN - 8, 8);

        ਮੂਲ:
                ਵਾਪਸ false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, ਬਣਤਰ hw_addr_ਅੰਕੜੇ, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(hw_addr_hash_ops_free, ਬਣਤਰ hw_addr_ਅੰਕੜੇ, hw_addr_hash_func, hw_addr_compare, ਮੁਕਤ);

ਅੱਖਰ* ether_addr_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਬਣਤਰ ether_addr *addr, ਅੱਖਰ ਬਫਰ[ETHER_ADDR_ਮੰਜ਼ਿਲ_STRING_MAX]) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        sprintf(ਬਫਰ, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        ਵਾਪਸ ਬਫਰ;
}

ਪੂਰਨ ether_addr_compare(ਅਚਲ ਬਣਤਰ ether_addr *a, ਅਚਲ ਬਣਤਰ ether_addr *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ memcmp(a, b, ETH_ALEN);
}

ਸਥਿਰ ਖਾਲੀ ether_addr_hash_func(ਅਚਲ ਬਣਤਰ ether_addr *p, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_ਪ੍ਰਸਾਰਣ(ਅਚਲ ਬਣਤਰ ether_addr *addr) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);
        ਵਾਪਸ memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, ਬਣਤਰ ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(ether_addr_hash_ops_free, ਬਣਤਰ ether_addr, ether_addr_hash_func, ether_addr_compare, ਮੁਕਤ);

ਸਥਿਰ ਪੂਰਨ parse_hw_addr_one_ਖੇਤਰ(ਅਚਲ ਅੱਖਰ **s, ਅੱਖਰ sep, size_t len, uint8_t *buf) {
        ਅਚਲ ਅੱਖਰ *hex = HEXDIGITS, *p;
        uint16_t ਅੰਕੜੇ = 0;
        bool cont;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(*s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(len, 1, 2));
        ਪ੍ਰਤਿਜ੍ਞਾ(buf);

        p = *s;

        ਲਈ (size_t i = 0; i < len * 2; i++) {
                ਅਚਲ ਅੱਖਰ *hexoff;
                size_t x;

                ਜੇ (*p == '\0' || *p == sep) {
                        ਜੇ (i == 0)
                                ਵਾਪਸ -EINVAL;
                        ਰੋਕੋ;
                }

                hexoff = strchr(hex, *p);
                ਜੇ (!hexoff)
                        ਵਾਪਸ -EINVAL;

                ਪ੍ਰਤਿਜ੍ਞਾ(hexoff >= hex);
                x = hexoff - hex;
                ਜੇ (x >= 16)
                        x -= 6; /* A-F */

                ਪ੍ਰਤਿਜ੍ਞਾ(x < 16);
                ਅੰਕੜੇ <<= 4;
                ਅੰਕੜੇ += x;

                p++;
        }

        ਜੇ (*p != '\0' && *p != sep)
                ਵਾਪਸ -EINVAL;

        ਚੋਣ (len) {
        ਹਾਲਤ 1:
                buf[0] = ਅੰਕੜੇ;
                ਰੋਕੋ;
        ਹਾਲਤ 2:
                buf[0] = (ਅੰਕੜੇ & 0xff00) >> 8;
                buf[1] = ਅੰਕੜੇ & 0xff;
                ਰੋਕੋ;
        ਮੂਲ:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        ਵਾਪਸ cont;
}

ਪੂਰਨ parse_hw_addr_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, size_t expected_len, ਬਣਤਰ hw_addr_ਅੰਕੜੇ *ret) {
        size_t ਖੇਤਰ_ਮਾਪ, max_len, len = 0;
        uint8_t ਬਾਈਟਾਂ[HW_ADDR_MAX_ਮਾਪ];
        ਅੱਖਰ sep;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(expected_len <= HW_ADDR_MAX_ਮਾਪ || expected_len == SIZE_MAX);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

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

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(expected_len, 0, ਆਕਾਰ(ਬਣਤਰ in_addr), ਆਕਾਰ(ਬਣਤਰ in6_addr))) {
                ਸੰਘ ਵਿੱਚ_addr_union a;
                ਪੂਰਨ family;

                ਜੇ (expected_len == 0)
                        r = ਵਿੱਚ_addr_from_string_auto(s, &family, &a);
                ਨਹੀਂਤਾਂ {
                        family = expected_len == ਆਕਾਰ(ਬਣਤਰ in_addr) ? AF_INET : AF_INET6;
                        r = ਵਿੱਚ_addr_from_string(family, s, &a);
                }
                ਜੇ (r >= 0) {
                        ret->length = FAMILY_ਪਤਾ_ਮਾਪ(family);
                        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ret->ਬਾਈਟਾਂ, a.ਬਾਈਟਾਂ, ret->length);
                        ਵਾਪਸ 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_ਮਾਪ : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        ਜੇ (sep == '.')
                ਖੇਤਰ_ਮਾਪ = 2;
        ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(sep, ':', '-'))
                ਖੇਤਰ_ਮਾਪ = 1;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ -EINVAL;

        ਜੇ (max_len % ਖੇਤਰ_ਮਾਪ != 0)
                ਵਾਪਸ -EINVAL;

        ਲਈ (size_t i = 0; i < max_len / ਖੇਤਰ_ਮਾਪ; i++) {
                r = parse_hw_addr_one_ਖੇਤਰ(&s, sep, ਖੇਤਰ_ਮਾਪ, ਬਾਈਟਾਂ + i * ਖੇਤਰ_ਮਾਪ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0) {
                        len = (i + 1) * ਖੇਤਰ_ਮਾਪ;
                        ਰੋਕੋ;
                }
        }

        ਜੇ (len == 0)
                ਵਾਪਸ -EINVAL;

        ਜੇ (expected_len == 0) {
                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        ਵਾਪਸ -EINVAL;
        } ਨਹੀਂਤਾਂ ਜੇ (expected_len != SIZE_MAX) {
                ਜੇ (len != expected_len)
                        ਵਾਪਸ -EINVAL;
        }

        ret->length = len;
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ret->ਬਾਈਟਾਂ, ਬਾਈਟਾਂ, ret->length);
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_ether_addr(ਅਚਲ ਅੱਖਰ *s, ਬਣਤਰ ether_addr *ret) {
        ਬਣਤਰ hw_addr_ਅੰਕੜੇ a;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_hw_addr_ਭਰਿਆ(s, ETH_ALEN, &a);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = a.ether;
        ਵਾਪਸ 0;
}

ਖਾਲੀ ether_addr_mark_random(ਬਣਤਰ ether_addr *addr) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

ਪੂਰਨ hw_addr_ensure_ਪ੍ਰਸਾਰਣ(ਬਣਤਰ hw_addr_ਅੰਕੜੇ *bcast_addr, uint16_t arp_type) {
        ਪ੍ਰਤਿਜ੍ਞਾ(bcast_addr);

        ਜੇ (!hw_addr_is_null(bcast_addr))
                ਵਾਪਸ 0;

        ਚੋਣ (arp_type) {
        ਹਾਲਤ ARPHRD_ETHER:
                *bcast_addr = (ਬਣਤਰ hw_addr_ਅੰਕੜੇ) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                ਵਾਪਸ 0;
        ਹਾਲਤ ARPHRD_INFINIBAND:
                *bcast_addr = (ਬਣਤਰ hw_addr_ਅੰਕੜੇ) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                ਵਾਪਸ 0;
        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}
