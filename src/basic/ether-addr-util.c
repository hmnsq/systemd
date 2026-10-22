/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <net/ethernet.h>
#అన్తర్భావ <net/if_arp.h>
#అన్తర్భావ <stdio.h>

#అన్తర్భావ "ether-addr-util.h"
#అన్తర్భావ "hash-funcs.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "in-addr-util.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "string-util.h"

అక్షరం* hw_addr_గమ్యం_string_నిండిన(
                స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *addr,
                Hardware_చిరునామా_గమ్యం_String_Flags flags,
                అక్షరం బఫరు[స్థిర HW_ADDR_గమ్యం_STRING_MAX]) {

        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(addr->length <= HW_ADDR_MAX_కొలత);

        కొరకు (size_t i = 0, j = 0; i < addr->length; i++) {
                బఫరు[j++] = hexchar(addr->బైట్లు[i] >> 4);
                బఫరు[j++] = hexchar(addr->బైట్లు[i] & 0x0f);
                ఒకవేళ (!FLAGS_సమితి(flags, HW_ADDR_గమ్యం_STRING_NO_COLON))
                        బఫరు[j++] = ':';
        }

        బఫరు[addr->length == 0 || FLAGS_సమితి(flags, HW_ADDR_గమ్యం_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        తిరిగి బఫరు;
}

నిర్మాణం hw_addr_దత్తాంశం *hw_addr_సమితి(నిర్మాణం hw_addr_దత్తాంశం *addr, స్థిరరాశి uint8_t *బైట్లు, size_t length) {
        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(length <= HW_ADDR_MAX_కొలత);

        addr->length = length;
        memcpy_సురక్షిత(addr->బైట్లు, బైట్లు, length);
        తిరిగి addr;
}

పూర్ణాంకం hw_addr_compare(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *a, స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *b) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        r = CMP(a->length, b->length);
        ఒకవేళ (r != 0)
                తిరిగి r;

        తిరిగి memcmp(a->బైట్లు, b->బైట్లు, a->length);
}

శూన్య hw_addr_hash_func(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *p, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_సురక్షిత(p->బైట్లు, p->length, state);
}

bool hw_addr_is_null(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *addr) {
        ప్రతిజ్ఞా(addr);
        తిరిగి addr->length == 0 || memeqzero(addr->బైట్లు, addr->length);
}

bool hw_addr_is_valid(స్థిరరాశి నిర్మాణం hw_addr_దత్తాంశం *addr, uint16_t iftype) {
        ప్రతిజ్ఞా(addr);

        ఎంపిక (iftype) {
        స్థితి ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                ఒకవేళ (addr->length != ETH_ALEN)
                        తిరిగి false;

                తిరిగి !ether_addr_is_null(&addr->ether) && !ether_addr_is_ప్రసారం(&addr->ether);

        స్థితి ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                ఒకవేళ (addr->length != INFINIBAND_ALEN)
                        తిరిగి false;

                తిరిగి !memeqzero(addr->బైట్లు + INFINIBAND_ALEN - 8, 8);

        అప్రమేయ:
                తిరిగి false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, నిర్మాణం hw_addr_దత్తాంశం, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(hw_addr_hash_ops_free, నిర్మాణం hw_addr_దత్తాంశం, hw_addr_hash_func, hw_addr_compare, విడుదల);

అక్షరం* ether_addr_గమ్యం_string(స్థిరరాశి నిర్మాణం ether_addr *addr, అక్షరం బఫరు[ETHER_ADDR_గమ్యం_STRING_MAX]) {
        ప్రతిజ్ఞా(addr);
        ప్రతిజ్ఞా(బఫరు);

        /* Like ether_ntoa() but uses %02x instead of %x to print
         * ethernet addresses, which makes them look less funny. Also,
         * doesn't use a static buffer. */

        sprintf(బఫరు, "%02x:%02x:%02x:%02x:%02x:%02x",
                addr->ether_addr_octet[0],
                addr->ether_addr_octet[1],
                addr->ether_addr_octet[2],
                addr->ether_addr_octet[3],
                addr->ether_addr_octet[4],
                addr->ether_addr_octet[5]);

        తిరిగి బఫరు;
}

పూర్ణాంకం ether_addr_compare(స్థిరరాశి నిర్మాణం ether_addr *a, స్థిరరాశి నిర్మాణం ether_addr *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి memcmp(a, b, ETH_ALEN);
}

స్థిర శూన్య ether_addr_hash_func(స్థిరరాశి నిర్మాణం ether_addr *p, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_ప్రసారం(స్థిరరాశి నిర్మాణం ether_addr *addr) {
        ప్రతిజ్ఞా(addr);
        తిరిగి memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, నిర్మాణం ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(ether_addr_hash_ops_free, నిర్మాణం ether_addr, ether_addr_hash_func, ether_addr_compare, విడుదల);

స్థిర పూర్ణాంకం parse_hw_addr_one_క్షేత్రం(స్థిరరాశి అక్షరం **s, అక్షరం sep, size_t len, uint8_t *buf) {
        స్థిరరాశి అక్షరం *hex = HEXDIGITS, *p;
        uint16_t దత్తాంశం = 0;
        bool cont;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(*s);
        ప్రతిజ్ఞా(లోపల_సమితి(len, 1, 2));
        ప్రతిజ్ఞా(buf);

        p = *s;

        కొరకు (size_t i = 0; i < len * 2; i++) {
                స్థిరరాశి అక్షరం *hexoff;
                size_t x;

                ఒకవేళ (*p == '\0' || *p == sep) {
                        ఒకవేళ (i == 0)
                                తిరిగి -EINVAL;
                        ఆపు;
                }

                hexoff = strchr(hex, *p);
                ఒకవేళ (!hexoff)
                        తిరిగి -EINVAL;

                ప్రతిజ్ఞా(hexoff >= hex);
                x = hexoff - hex;
                ఒకవేళ (x >= 16)
                        x -= 6; /* A-F */

                ప్రతిజ్ఞా(x < 16);
                దత్తాంశం <<= 4;
                దత్తాంశం += x;

                p++;
        }

        ఒకవేళ (*p != '\0' && *p != sep)
                తిరిగి -EINVAL;

        ఎంపిక (len) {
        స్థితి 1:
                buf[0] = దత్తాంశం;
                ఆపు;
        స్థితి 2:
                buf[0] = (దత్తాంశం & 0xff00) >> 8;
                buf[1] = దత్తాంశం & 0xff;
                ఆపు;
        అప్రమేయ:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        తిరిగి cont;
}

పూర్ణాంకం parse_hw_addr_నిండిన(స్థిరరాశి అక్షరం *s, size_t expected_len, నిర్మాణం hw_addr_దత్తాంశం *ret) {
        size_t క్షేత్రం_కొలత, max_len, len = 0;
        uint8_t బైట్లు[HW_ADDR_MAX_కొలత];
        అక్షరం sep;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(expected_len <= HW_ADDR_MAX_కొలత || expected_len == SIZE_MAX);
        ప్రతిజ్ఞా(ret);

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

        ఒకవేళ (లోపల_సమితి(expected_len, 0, పరిమాణం(నిర్మాణం in_addr), పరిమాణం(నిర్మాణం in6_addr))) {
                సంఘం లోపల_addr_union a;
                పూర్ణాంకం family;

                ఒకవేళ (expected_len == 0)
                        r = లోపల_addr_from_string_auto(s, &family, &a);
                లేకపోతే {
                        family = expected_len == పరిమాణం(నిర్మాణం in_addr) ? AF_INET : AF_INET6;
                        r = లోపల_addr_from_string(family, s, &a);
                }
                ఒకవేళ (r >= 0) {
                        ret->length = FAMILY_చిరునామా_కొలత(family);
                        జ్ఞాపకం_నకలు(ret->బైట్లు, a.బైట్లు, ret->length);
                        తిరిగి 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_కొలత : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        ఒకవేళ (sep == '.')
                క్షేత్రం_కొలత = 2;
        లేకపోతే ఒకవేళ (లోపల_సమితి(sep, ':', '-'))
                క్షేత్రం_కొలత = 1;
        లేకపోతే
                తిరిగి -EINVAL;

        ఒకవేళ (max_len % క్షేత్రం_కొలత != 0)
                తిరిగి -EINVAL;

        కొరకు (size_t i = 0; i < max_len / క్షేత్రం_కొలత; i++) {
                r = parse_hw_addr_one_క్షేత్రం(&s, sep, క్షేత్రం_కొలత, బైట్లు + i * క్షేత్రం_కొలత);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0) {
                        len = (i + 1) * క్షేత్రం_కొలత;
                        ఆపు;
                }
        }

        ఒకవేళ (len == 0)
                తిరిగి -EINVAL;

        ఒకవేళ (expected_len == 0) {
                ఒకవేళ (!లోపల_సమితి(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        తిరిగి -EINVAL;
        } లేకపోతే ఒకవేళ (expected_len != SIZE_MAX) {
                ఒకవేళ (len != expected_len)
                        తిరిగి -EINVAL;
        }

        ret->length = len;
        జ్ఞాపకం_నకలు(ret->బైట్లు, బైట్లు, ret->length);
        తిరిగి 0;
}

పూర్ణాంకం parse_ether_addr(స్థిరరాశి అక్షరం *s, నిర్మాణం ether_addr *ret) {
        నిర్మాణం hw_addr_దత్తాంశం a;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret);

        r = parse_hw_addr_నిండిన(s, ETH_ALEN, &a);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = a.ether;
        తిరిగి 0;
}

శూన్య ether_addr_mark_random(నిర్మాణం ether_addr *addr) {
        ప్రతిజ్ఞా(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

పూర్ణాంకం hw_addr_ensure_ప్రసారం(నిర్మాణం hw_addr_దత్తాంశం *bcast_addr, uint16_t arp_type) {
        ప్రతిజ్ఞా(bcast_addr);

        ఒకవేళ (!hw_addr_is_null(bcast_addr))
                తిరిగి 0;

        ఎంపిక (arp_type) {
        స్థితి ARPHRD_ETHER:
                *bcast_addr = (నిర్మాణం hw_addr_దత్తాంశం) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                తిరిగి 0;
        స్థితి ARPHRD_INFINIBAND:
                *bcast_addr = (నిర్మాణం hw_addr_దత్తాంశం) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                తిరిగి 0;
        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}
