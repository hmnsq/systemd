/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <net/ethernet.h>
#അന്തര്ഭാവ <net/if_arp.h>
#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "ether-addr-util.h"
#അന്തര്ഭാവ "hash-funcs.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "in-addr-util.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "siphash24.h"
#അന്തര്ഭാവ "string-util.h"

അക്ഷരം* hw_addr_to_string_full(
                സ്ഥിരം ഘടന hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                അക്ഷരം buffer[സ്ഥിതം HW_ADDR_TO_STRING_MAX]) {

        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(addr->length <= HW_ADDR_MAX_SIZE);

        വേണ്ടി (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                എങ്കിൽ (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        തിരികെ buffer;
}

ഘടന hw_addr_data *hw_addr_set(ഘടന hw_addr_data *addr, സ്ഥിരം uint8_t *bytes, size_t length) {
        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        തിരികെ addr;
}

പൂർണ്ണസംഖ്യ hw_addr_compare(സ്ഥിരം ഘടന hw_addr_data *a, സ്ഥിരം ഘടന hw_addr_data *b) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        r = CMP(a->length, b->length);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        തിരികെ memcmp(a->bytes, b->bytes, a->length);
}

ശൂന്യം hw_addr_hash_func(സ്ഥിരം ഘടന hw_addr_data *p, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(സ്ഥിരം ഘടന hw_addr_data *addr) {
        പ്രതിജ്ഞാ(addr);
        തിരികെ addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(സ്ഥിരം ഘടന hw_addr_data *addr, uint16_t iftype) {
        പ്രതിജ്ഞാ(addr);

        തിരഞ്ഞെടുപ്പ് (iftype) {
        അവസ്ഥ ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                എങ്കിൽ (addr->length != ETH_ALEN)
                        തിരികെ false;

                തിരികെ !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        അവസ്ഥ ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                എങ്കിൽ (addr->length != INFINIBAND_ALEN)
                        തിരികെ false;

                തിരികെ !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        സ്ഥിരസ്ഥിതി:
                തിരികെ false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, ഘടന hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(hw_addr_hash_ops_free, ഘടന hw_addr_data, hw_addr_hash_func, hw_addr_compare, സ്വതന്ത്രം);

അക്ഷരം* ether_addr_to_string(സ്ഥിരം ഘടന ether_addr *addr, അക്ഷരം buffer[ETHER_ADDR_TO_STRING_MAX]) {
        പ്രതിജ്ഞാ(addr);
        പ്രതിജ്ഞാ(buffer);

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

        തിരികെ buffer;
}

പൂർണ്ണസംഖ്യ ether_addr_compare(സ്ഥിരം ഘടന ether_addr *a, സ്ഥിരം ഘടന ether_addr *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ memcmp(a, b, ETH_ALEN);
}

സ്ഥിതം ശൂന്യം ether_addr_hash_func(സ്ഥിരം ഘടന ether_addr *p, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(സ്ഥിരം ഘടന ether_addr *addr) {
        പ്രതിജ്ഞാ(addr);
        തിരികെ memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, ഘടന ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(ether_addr_hash_ops_free, ഘടന ether_addr, ether_addr_hash_func, ether_addr_compare, സ്വതന്ത്രം);

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_hw_addr_one_field(സ്ഥിരം അക്ഷരം **s, അക്ഷരം sep, size_t len, uint8_t *buf) {
        സ്ഥിരം അക്ഷരം *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(*s);
        പ്രതിജ്ഞാ(IN_SET(len, 1, 2));
        പ്രതിജ്ഞാ(buf);

        p = *s;

        വേണ്ടി (size_t i = 0; i < len * 2; i++) {
                സ്ഥിരം അക്ഷരം *hexoff;
                size_t x;

                എങ്കിൽ (*p == '\0' || *p == sep) {
                        എങ്കിൽ (i == 0)
                                തിരികെ -EINVAL;
                        നിർത്തുക;
                }

                hexoff = strchr(hex, *p);
                എങ്കിൽ (!hexoff)
                        തിരികെ -EINVAL;

                പ്രതിജ്ഞാ(hexoff >= hex);
                x = hexoff - hex;
                എങ്കിൽ (x >= 16)
                        x -= 6; /* A-F */

                പ്രതിജ്ഞാ(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        എങ്കിൽ (*p != '\0' && *p != sep)
                തിരികെ -EINVAL;

        തിരഞ്ഞെടുപ്പ് (len) {
        അവസ്ഥ 1:
                buf[0] = data;
                നിർത്തുക;
        അവസ്ഥ 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        തിരികെ cont;
}

പൂർണ്ണസംഖ്യ parse_hw_addr_full(സ്ഥിരം അക്ഷരം *s, size_t expected_len, ഘടന hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        അക്ഷരം sep;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        പ്രതിജ്ഞാ(ret);

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

        എങ്കിൽ (IN_SET(expected_len, 0, വലുപ്പം(ഘടന in_addr), വലുപ്പം(ഘടന in6_addr))) {
                സംഘം in_addr_union a;
                പൂർണ്ണസംഖ്യ family;

                എങ്കിൽ (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                അല്ലെങ്കിൽ {
                        family = expected_len == വലുപ്പം(ഘടന in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                എങ്കിൽ (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        തിരികെ 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        എങ്കിൽ (sep == '.')
                field_size = 2;
        അല്ലെങ്കിൽ എങ്കിൽ (IN_SET(sep, ':', '-'))
                field_size = 1;
        അല്ലെങ്കിൽ
                തിരികെ -EINVAL;

        എങ്കിൽ (max_len % field_size != 0)
                തിരികെ -EINVAL;

        വേണ്ടി (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0) {
                        len = (i + 1) * field_size;
                        നിർത്തുക;
                }
        }

        എങ്കിൽ (len == 0)
                തിരികെ -EINVAL;

        എങ്കിൽ (expected_len == 0) {
                എങ്കിൽ (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        തിരികെ -EINVAL;
        } അല്ലെങ്കിൽ എങ്കിൽ (expected_len != SIZE_MAX) {
                എങ്കിൽ (len != expected_len)
                        തിരികെ -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_ether_addr(സ്ഥിരം അക്ഷരം *s, ഘടന ether_addr *ret) {
        ഘടന hw_addr_data a;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = a.ether;
        തിരികെ 0;
}

ശൂന്യം ether_addr_mark_random(ഘടന ether_addr *addr) {
        പ്രതിജ്ഞാ(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

പൂർണ്ണസംഖ്യ hw_addr_ensure_broadcast(ഘടന hw_addr_data *bcast_addr, uint16_t arp_type) {
        പ്രതിജ്ഞാ(bcast_addr);

        എങ്കിൽ (!hw_addr_is_null(bcast_addr))
                തിരികെ 0;

        തിരഞ്ഞെടുപ്പ് (arp_type) {
        അവസ്ഥ ARPHRD_ETHER:
                *bcast_addr = (ഘടന hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                തിരികെ 0;
        അവസ്ഥ ARPHRD_INFINIBAND:
                *bcast_addr = (ഘടന hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                തിരികെ 0;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}
