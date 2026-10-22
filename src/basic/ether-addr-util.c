/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <net/ethernet.h>
#ଅନ୍ତର୍ଭାଵ <net/if_arp.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "ether-addr-util.h"
#ଅନ୍ତର୍ଭାଵ "hash-funcs.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "in-addr-util.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "siphash24.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ଅକ୍ଷର* hw_addr_to_string_full(
                ଅଚଳ ସଂରଚନା hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                ଅକ୍ଷର buffer[ସ୍ଥିର HW_ADDR_TO_STRING_MAX]) {

        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(addr->length <= HW_ADDR_MAX_SIZE);

        ପାଇଁ (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                ଯଦି (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        ଫେରନ୍ତୁ buffer;
}

ସଂରଚନା hw_addr_data *hw_addr_set(ସଂରଚନା hw_addr_data *addr, ଅଚଳ uint8_t *bytes, size_t length) {
        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        ଫେରନ୍ତୁ addr;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hw_addr_compare(ଅଚଳ ସଂରଚନା hw_addr_data *a, ଅଚଳ ସଂରଚନା hw_addr_data *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        r = CMP(a->length, b->length);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ memcmp(a->bytes, b->bytes, a->length);
}

ଶୂନ୍ୟ hw_addr_hash_func(ଅଚଳ ସଂରଚନା hw_addr_data *p, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(ଅଚଳ ସଂରଚନା hw_addr_data *addr) {
        ପ୍ରତିଜ୍ଞା(addr);
        ଫେରନ୍ତୁ addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(ଅଚଳ ସଂରଚନା hw_addr_data *addr, uint16_t iftype) {
        ପ୍ରତିଜ୍ଞା(addr);

        ବାଛ (iftype) {
        ସ୍ଥିତି ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                ଯଦି (addr->length != ETH_ALEN)
                        ଫେରନ୍ତୁ false;

                ଫେରନ୍ତୁ !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        ସ୍ଥିତି ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                ଯଦି (addr->length != INFINIBAND_ALEN)
                        ଫେରନ୍ତୁ false;

                ଫେରନ୍ତୁ !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, ସଂରଚନା hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(hw_addr_hash_ops_free, ସଂରଚନା hw_addr_data, hw_addr_hash_func, hw_addr_compare, ମୁକ୍ତ);

ଅକ୍ଷର* ether_addr_to_string(ଅଚଳ ସଂରଚନା ether_addr *addr, ଅକ୍ଷର buffer[ETHER_ADDR_TO_STRING_MAX]) {
        ପ୍ରତିଜ୍ଞା(addr);
        ପ୍ରତିଜ୍ଞା(buffer);

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

        ଫେରନ୍ତୁ buffer;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ether_addr_compare(ଅଚଳ ସଂରଚନା ether_addr *a, ଅଚଳ ସଂରଚନା ether_addr *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ memcmp(a, b, ETH_ALEN);
}

ସ୍ଥିର ଶୂନ୍ୟ ether_addr_hash_func(ଅଚଳ ସଂରଚନା ether_addr *p, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(ଅଚଳ ସଂରଚନା ether_addr *addr) {
        ପ୍ରତିଜ୍ଞା(addr);
        ଫେରନ୍ତୁ memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, ସଂରଚନା ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(ether_addr_hash_ops_free, ସଂରଚନା ether_addr, ether_addr_hash_func, ether_addr_compare, ମୁକ୍ତ);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_hw_addr_one_field(ଅଚଳ ଅକ୍ଷର **s, ଅକ୍ଷର sep, size_t len, uint8_t *buf) {
        ଅଚଳ ଅକ୍ଷର *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(*s);
        ପ୍ରତିଜ୍ଞା(IN_SET(len, 1, 2));
        ପ୍ରତିଜ୍ଞା(buf);

        p = *s;

        ପାଇଁ (size_t i = 0; i < len * 2; i++) {
                ଅଚଳ ଅକ୍ଷର *hexoff;
                size_t x;

                ଯଦି (*p == '\0' || *p == sep) {
                        ଯଦି (i == 0)
                                ଫେରନ୍ତୁ -EINVAL;
                        ଭାଙ୍ଗ;
                }

                hexoff = strchr(hex, *p);
                ଯଦି (!hexoff)
                        ଫେରନ୍ତୁ -EINVAL;

                ପ୍ରତିଜ୍ଞା(hexoff >= hex);
                x = hexoff - hex;
                ଯଦି (x >= 16)
                        x -= 6; /* A-F */

                ପ୍ରତିଜ୍ଞା(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        ଯଦି (*p != '\0' && *p != sep)
                ଫେରନ୍ତୁ -EINVAL;

        ବାଛ (len) {
        ସ୍ଥିତି 1:
                buf[0] = data;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        ଫେରନ୍ତୁ cont;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_hw_addr_full(ଅଚଳ ଅକ୍ଷର *s, size_t expected_len, ସଂରଚନା hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        ଅକ୍ଷର sep;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        ପ୍ରତିଜ୍ଞା(ret);

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

        ଯଦି (IN_SET(expected_len, 0, ଆକାର(ସଂରଚନା in_addr), ଆକାର(ସଂରଚନା in6_addr))) {
                ସଂଘ in_addr_union a;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family;

                ଯଦି (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                ଅନ୍ୟଥା {
                        family = expected_len == ଆକାର(ସଂରଚନା in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                ଯଦି (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        ଫେରନ୍ତୁ 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        ଯଦି (sep == '.')
                field_size = 2;
        ଅନ୍ୟଥା ଯଦି (IN_SET(sep, ':', '-'))
                field_size = 1;
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (max_len % field_size != 0)
                ଫେରନ୍ତୁ -EINVAL;

        ପାଇଁ (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0) {
                        len = (i + 1) * field_size;
                        ଭାଙ୍ଗ;
                }
        }

        ଯଦି (len == 0)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (expected_len == 0) {
                ଯଦି (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        ଫେରନ୍ତୁ -EINVAL;
        } ଅନ୍ୟଥା ଯଦି (expected_len != SIZE_MAX) {
                ଯଦି (len != expected_len)
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ether_addr(ଅଚଳ ଅକ୍ଷର *s, ସଂରଚନା ether_addr *ret) {
        ସଂରଚନା hw_addr_data a;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = a.ether;
        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ ether_addr_mark_random(ସଂରଚନା ether_addr *addr) {
        ପ୍ରତିଜ୍ଞା(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hw_addr_ensure_broadcast(ସଂରଚନା hw_addr_data *bcast_addr, uint16_t arp_type) {
        ପ୍ରତିଜ୍ଞା(bcast_addr);

        ଯଦି (!hw_addr_is_null(bcast_addr))
                ଫେରନ୍ତୁ 0;

        ବାଛ (arp_type) {
        ସ୍ଥିତି ARPHRD_ETHER:
                *bcast_addr = (ସଂରଚନା hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                ଫେରନ୍ତୁ 0;
        ସ୍ଥିତି ARPHRD_INFINIBAND:
                *bcast_addr = (ସଂରଚନା hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                ଫେରନ୍ତୁ 0;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}
