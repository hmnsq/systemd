/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <net/ethernet.h>
#અન્તર્ભાવ <net/if_arp.h>
#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "ether-addr-util.h"
#અન્તર્ભાવ "hash-funcs.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "in-addr-util.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "siphash24.h"
#અન્તર્ભાવ "string-util.h"

અક્ષર* hw_addr_to_string_full(
                અચલ સંરચના hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                અક્ષર buffer[સ્થિર HW_ADDR_TO_STRING_MAX]) {

        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(addr->length <= HW_ADDR_MAX_SIZE);

        માટે (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                જો (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        પાછા buffer;
}

સંરચના hw_addr_data *hw_addr_set(સંરચના hw_addr_data *addr, અચલ uint8_t *bytes, size_t length) {
        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        પાછા addr;
}

પૂર્ણાંક hw_addr_compare(અચલ સંરચના hw_addr_data *a, અચલ સંરચના hw_addr_data *b) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        r = CMP(a->length, b->length);
        જો (r != 0)
                પાછા r;

        પાછા memcmp(a->bytes, b->bytes, a->length);
}

શૂન્ય hw_addr_hash_func(અચલ સંરચના hw_addr_data *p, સંરચના siphash *state) {
        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(અચલ સંરચના hw_addr_data *addr) {
        પ્રતિજ્ઞા(addr);
        પાછા addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(અચલ સંરચના hw_addr_data *addr, uint16_t iftype) {
        પ્રતિજ્ઞા(addr);

        પસંદગી (iftype) {
        સ્થિતિ ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                જો (addr->length != ETH_ALEN)
                        પાછા false;

                પાછા !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        સ્થિતિ ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                જો (addr->length != INFINIBAND_ALEN)
                        પાછા false;

                પાછા !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        મૂળભૂત:
                પાછા false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, સંરચના hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(hw_addr_hash_ops_free, સંરચના hw_addr_data, hw_addr_hash_func, hw_addr_compare, મુક્ત);

અક્ષર* ether_addr_to_string(અચલ સંરચના ether_addr *addr, અક્ષર buffer[ETHER_ADDR_TO_STRING_MAX]) {
        પ્રતિજ્ઞા(addr);
        પ્રતિજ્ઞા(buffer);

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

        પાછા buffer;
}

પૂર્ણાંક ether_addr_compare(અચલ સંરચના ether_addr *a, અચલ સંરચના ether_addr *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા memcmp(a, b, ETH_ALEN);
}

સ્થિર શૂન્ય ether_addr_hash_func(અચલ સંરચના ether_addr *p, સંરચના siphash *state) {
        પ્રતિજ્ઞા(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(અચલ સંરચના ether_addr *addr) {
        પ્રતિજ્ઞા(addr);
        પાછા memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, સંરચના ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(ether_addr_hash_ops_free, સંરચના ether_addr, ether_addr_hash_func, ether_addr_compare, મુક્ત);

સ્થિર પૂર્ણાંક parse_hw_addr_one_field(અચલ અક્ષર **s, અક્ષર sep, size_t len, uint8_t *buf) {
        અચલ અક્ષર *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(*s);
        પ્રતિજ્ઞા(IN_SET(len, 1, 2));
        પ્રતિજ્ઞા(buf);

        p = *s;

        માટે (size_t i = 0; i < len * 2; i++) {
                અચલ અક્ષર *hexoff;
                size_t x;

                જો (*p == '\0' || *p == sep) {
                        જો (i == 0)
                                પાછા -EINVAL;
                        તોડો;
                }

                hexoff = strchr(hex, *p);
                જો (!hexoff)
                        પાછા -EINVAL;

                પ્રતિજ્ઞા(hexoff >= hex);
                x = hexoff - hex;
                જો (x >= 16)
                        x -= 6; /* A-F */

                પ્રતિજ્ઞા(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        જો (*p != '\0' && *p != sep)
                પાછા -EINVAL;

        પસંદગી (len) {
        સ્થિતિ 1:
                buf[0] = data;
                તોડો;
        સ્થિતિ 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                તોડો;
        મૂળભૂત:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        પાછા cont;
}

પૂર્ણાંક parse_hw_addr_full(અચલ અક્ષર *s, size_t expected_len, સંરચના hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        અક્ષર sep;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        પ્રતિજ્ઞા(ret);

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

        જો (IN_SET(expected_len, 0, કદ(સંરચના in_addr), કદ(સંરચના in6_addr))) {
                સંઘ in_addr_union a;
                પૂર્ણાંક family;

                જો (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                નહીંતર {
                        family = expected_len == કદ(સંરચના in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                જો (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        પાછા 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        જો (sep == '.')
                field_size = 2;
        નહીંતર જો (IN_SET(sep, ':', '-'))
                field_size = 1;
        નહીંતર
                પાછા -EINVAL;

        જો (max_len % field_size != 0)
                પાછા -EINVAL;

        માટે (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                જો (r < 0)
                        પાછા r;
                જો (r == 0) {
                        len = (i + 1) * field_size;
                        તોડો;
                }
        }

        જો (len == 0)
                પાછા -EINVAL;

        જો (expected_len == 0) {
                જો (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        પાછા -EINVAL;
        } નહીંતર જો (expected_len != SIZE_MAX) {
                જો (len != expected_len)
                        પાછા -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        પાછા 0;
}

પૂર્ણાંક parse_ether_addr(અચલ અક્ષર *s, સંરચના ether_addr *ret) {
        સંરચના hw_addr_data a;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        જો (r < 0)
                પાછા r;

        *ret = a.ether;
        પાછા 0;
}

શૂન્ય ether_addr_mark_random(સંરચના ether_addr *addr) {
        પ્રતિજ્ઞા(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

પૂર્ણાંક hw_addr_ensure_broadcast(સંરચના hw_addr_data *bcast_addr, uint16_t arp_type) {
        પ્રતિજ્ઞા(bcast_addr);

        જો (!hw_addr_is_null(bcast_addr))
                પાછા 0;

        પસંદગી (arp_type) {
        સ્થિતિ ARPHRD_ETHER:
                *bcast_addr = (સંરચના hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                પાછા 0;
        સ્થિતિ ARPHRD_INFINIBAND:
                *bcast_addr = (સંરચના hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                પાછા 0;
        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}
