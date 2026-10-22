/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <net/ethernet.h>
#அந்தர்பாவ <net/if_arp.h>
#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "ether-addr-util.h"
#அந்தர்பாவ "hash-funcs.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "in-addr-util.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "siphash24.h"
#அந்தர்பாவ "string-util.h"

எழுத்து* hw_addr_to_string_full(
                மாறிலி கட்டமைப்பு hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                எழுத்து buffer[நிலையான HW_ADDR_TO_STRING_MAX]) {

        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(addr->length <= HW_ADDR_MAX_SIZE);

        க்கு (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                எனில் (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        திரும்பு buffer;
}

கட்டமைப்பு hw_addr_data *hw_addr_set(கட்டமைப்பு hw_addr_data *addr, மாறிலி uint8_t *bytes, size_t length) {
        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        திரும்பு addr;
}

முழு hw_addr_compare(மாறிலி கட்டமைப்பு hw_addr_data *a, மாறிலி கட்டமைப்பு hw_addr_data *b) {
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        r = CMP(a->length, b->length);
        எனில் (r != 0)
                திரும்பு r;

        திரும்பு memcmp(a->bytes, b->bytes, a->length);
}

வெறுமை hw_addr_hash_func(மாறிலி கட்டமைப்பு hw_addr_data *p, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(மாறிலி கட்டமைப்பு hw_addr_data *addr) {
        ப்ரதிஜ்ஞா(addr);
        திரும்பு addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(மாறிலி கட்டமைப்பு hw_addr_data *addr, uint16_t iftype) {
        ப்ரதிஜ்ஞா(addr);

        தேர்வு (iftype) {
        நிலை ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                எனில் (addr->length != ETH_ALEN)
                        திரும்பு false;

                திரும்பு !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        நிலை ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                எனில் (addr->length != INFINIBAND_ALEN)
                        திரும்பு false;

                திரும்பு !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        இயல்பு:
                திரும்பு false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, கட்டமைப்பு hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(hw_addr_hash_ops_free, கட்டமைப்பு hw_addr_data, hw_addr_hash_func, hw_addr_compare, விடுவி);

எழுத்து* ether_addr_to_string(மாறிலி கட்டமைப்பு ether_addr *addr, எழுத்து buffer[ETHER_ADDR_TO_STRING_MAX]) {
        ப்ரதிஜ்ஞா(addr);
        ப்ரதிஜ்ஞா(buffer);

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

        திரும்பு buffer;
}

முழு ether_addr_compare(மாறிலி கட்டமைப்பு ether_addr *a, மாறிலி கட்டமைப்பு ether_addr *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு memcmp(a, b, ETH_ALEN);
}

நிலையான வெறுமை ether_addr_hash_func(மாறிலி கட்டமைப்பு ether_addr *p, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(மாறிலி கட்டமைப்பு ether_addr *addr) {
        ப்ரதிஜ்ஞா(addr);
        திரும்பு memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, கட்டமைப்பு ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(ether_addr_hash_ops_free, கட்டமைப்பு ether_addr, ether_addr_hash_func, ether_addr_compare, விடுவி);

நிலையான முழு parse_hw_addr_one_field(மாறிலி எழுத்து **s, எழுத்து sep, size_t len, uint8_t *buf) {
        மாறிலி எழுத்து *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(*s);
        ப்ரதிஜ்ஞா(IN_SET(len, 1, 2));
        ப்ரதிஜ்ஞா(buf);

        p = *s;

        க்கு (size_t i = 0; i < len * 2; i++) {
                மாறிலி எழுத்து *hexoff;
                size_t x;

                எனில் (*p == '\0' || *p == sep) {
                        எனில் (i == 0)
                                திரும்பு -EINVAL;
                        நிறுத்து;
                }

                hexoff = strchr(hex, *p);
                எனில் (!hexoff)
                        திரும்பு -EINVAL;

                ப்ரதிஜ்ஞா(hexoff >= hex);
                x = hexoff - hex;
                எனில் (x >= 16)
                        x -= 6; /* A-F */

                ப்ரதிஜ்ஞா(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        எனில் (*p != '\0' && *p != sep)
                திரும்பு -EINVAL;

        தேர்வு (len) {
        நிலை 1:
                buf[0] = data;
                நிறுத்து;
        நிலை 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                நிறுத்து;
        இயல்பு:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        திரும்பு cont;
}

முழு parse_hw_addr_full(மாறிலி எழுத்து *s, size_t expected_len, கட்டமைப்பு hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        எழுத்து sep;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        ப்ரதிஜ்ஞா(ret);

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

        எனில் (IN_SET(expected_len, 0, அளவு(கட்டமைப்பு in_addr), அளவு(கட்டமைப்பு in6_addr))) {
                ஒன்றிப்பு in_addr_union a;
                முழு family;

                எனில் (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                இல்லையெனில் {
                        family = expected_len == அளவு(கட்டமைப்பு in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                எனில் (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        திரும்பு 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        எனில் (sep == '.')
                field_size = 2;
        இல்லையெனில் எனில் (IN_SET(sep, ':', '-'))
                field_size = 1;
        இல்லையெனில்
                திரும்பு -EINVAL;

        எனில் (max_len % field_size != 0)
                திரும்பு -EINVAL;

        க்கு (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0) {
                        len = (i + 1) * field_size;
                        நிறுத்து;
                }
        }

        எனில் (len == 0)
                திரும்பு -EINVAL;

        எனில் (expected_len == 0) {
                எனில் (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        திரும்பு -EINVAL;
        } இல்லையெனில் எனில் (expected_len != SIZE_MAX) {
                எனில் (len != expected_len)
                        திரும்பு -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        திரும்பு 0;
}

முழு parse_ether_addr(மாறிலி எழுத்து *s, கட்டமைப்பு ether_addr *ret) {
        கட்டமைப்பு hw_addr_data a;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        எனில் (r < 0)
                திரும்பு r;

        *ret = a.ether;
        திரும்பு 0;
}

வெறுமை ether_addr_mark_random(கட்டமைப்பு ether_addr *addr) {
        ப்ரதிஜ்ஞா(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

முழு hw_addr_ensure_broadcast(கட்டமைப்பு hw_addr_data *bcast_addr, uint16_t arp_type) {
        ப்ரதிஜ்ஞா(bcast_addr);

        எனில் (!hw_addr_is_null(bcast_addr))
                திரும்பு 0;

        தேர்வு (arp_type) {
        நிலை ARPHRD_ETHER:
                *bcast_addr = (கட்டமைப்பு hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                திரும்பு 0;
        நிலை ARPHRD_INFINIBAND:
                *bcast_addr = (கட்டமைப்பு hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                திரும்பு 0;
        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}
