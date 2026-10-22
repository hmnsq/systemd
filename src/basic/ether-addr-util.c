/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <net/ethernet.h>
#include <net/if_arp.h>
#include <stdio.h>

#include "ether-addr-util.h"
#include "hash-funcs.h"
#include "hexdecoct.h"
#include "in-addr-util.h"
#include "memory-util.h"
#include "siphash24.h"
#include "string-util.h"

var_na* hw_addr_to_string_full(
                const sa_nracanaa hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                var_na buffer[static HW_ADDR_TO_STRING_MAX]) {

        assert(addr);
        assert(buffer);
        assert(addr->length <= HW_ADDR_MAX_SIZE);

        အတွက် (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                အကယ်၍ (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        ပြန်ပေး buffer;
}

sa_nracanaa hw_addr_data *hw_addr_set(sa_nracanaa hw_addr_data *addr, const uint8_t *bytes, size_t length) {
        assert(addr);
        assert(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        ပြန်ပေး addr;
}

ကိန်းပြည့် hw_addr_compare(const sa_nracanaa hw_addr_data *a, const sa_nracanaa hw_addr_data *b) {
        ကိန်းပြည့် r;

        assert(a);
        assert(b);

        r = CMP(a->length, b->length);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        ပြန်ပေး memcmp(a->bytes, b->bytes, a->length);
}

void hw_addr_hash_func(const sa_nracanaa hw_addr_data *p, sa_nracanaa siphash *state) {
        assert(p);
        assert(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(const sa_nracanaa hw_addr_data *addr) {
        assert(addr);
        ပြန်ပေး addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(const sa_nracanaa hw_addr_data *addr, uint16_t iftype) {
        assert(addr);

        switch (iftype) {
        pak_sa ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                အကယ်၍ (addr->length != ETH_ALEN)
                        ပြန်ပေး false;

                ပြန်ပေး !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        pak_sa ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                အကယ်၍ (addr->length != INFINIBAND_ALEN)
                        ပြန်ပေး false;

                ပြန်ပေး !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        default:
                ပြန်ပေး false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, sa_nracanaa hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(hw_addr_hash_ops_free, sa_nracanaa hw_addr_data, hw_addr_hash_func, hw_addr_compare, free);

var_na* ether_addr_to_string(const sa_nracanaa ether_addr *addr, var_na buffer[ETHER_ADDR_TO_STRING_MAX]) {
        assert(addr);
        assert(buffer);

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

        ပြန်ပေး buffer;
}

ကိန်းပြည့် ether_addr_compare(const sa_nracanaa ether_addr *a, const sa_nracanaa ether_addr *b) {
        assert(a);
        assert(b);

        ပြန်ပေး memcmp(a, b, ETH_ALEN);
}

static void ether_addr_hash_func(const sa_nracanaa ether_addr *p, sa_nracanaa siphash *state) {
        assert(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(const sa_nracanaa ether_addr *addr) {
        assert(addr);
        ပြန်ပေး memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, sa_nracanaa ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(ether_addr_hash_ops_free, sa_nracanaa ether_addr, ether_addr_hash_func, ether_addr_compare, free);

static ကိန်းပြည့် parse_hw_addr_one_field(const var_na **s, var_na sep, size_t len, uint8_t *buf) {
        const var_na *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        assert(s);
        assert(*s);
        assert(IN_SET(len, 1, 2));
        assert(buf);

        p = *s;

        အတွက် (size_t i = 0; i < len * 2; i++) {
                const var_na *hexoff;
                size_t x;

                အကယ်၍ (*p == '\0' || *p == sep) {
                        အကယ်၍ (i == 0)
                                ပြန်ပေး -EINVAL;
                        bha_nga;
                }

                hexoff = strchr(hex, *p);
                အကယ်၍ (!hexoff)
                        ပြန်ပေး -EINVAL;

                assert(hexoff >= hex);
                x = hexoff - hex;
                အကယ်၍ (x >= 16)
                        x -= 6; /* A-F */

                assert(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        အကယ်၍ (*p != '\0' && *p != sep)
                ပြန်ပေး -EINVAL;

        switch (len) {
        pak_sa 1:
                buf[0] = data;
                bha_nga;
        pak_sa 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                bha_nga;
        default:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        ပြန်ပေး cont;
}

ကိန်းပြည့် parse_hw_addr_full(const var_na *s, size_t expected_len, sa_nracanaa hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        var_na sep;
        ကိန်းပြည့် r;

        assert(s);
        assert(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
        assert(ret);

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

        အကယ်၍ (IN_SET(expected_len, 0, parimaa_na(sa_nracanaa in_addr), parimaa_na(sa_nracanaa in6_addr))) {
                sa_ngha in_addr_union a;
                ကိန်းပြည့် family;

                အကယ်၍ (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                မဟုတ်ပါက {
                        family = expected_len == parimaa_na(sa_nracanaa in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                အကယ်၍ (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        ပြန်ပေး 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        အကယ်၍ (sep == '.')
                field_size = 2;
        မဟုတ်ပါက အကယ်၍ (IN_SET(sep, ':', '-'))
                field_size = 1;
        မဟုတ်ပါက
                ပြန်ပေး -EINVAL;

        အကယ်၍ (max_len % field_size != 0)
                ပြန်ပေး -EINVAL;

        အတွက် (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0) {
                        len = (i + 1) * field_size;
                        bha_nga;
                }
        }

        အကယ်၍ (len == 0)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (expected_len == 0) {
                အကယ်၍ (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        ပြန်ပေး -EINVAL;
        } မဟုတ်ပါက အကယ်၍ (expected_len != SIZE_MAX) {
                အကယ်၍ (len != expected_len)
                        ပြန်ပေး -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_ether_addr(const var_na *s, sa_nracanaa ether_addr *ret) {
        sa_nracanaa hw_addr_data a;
        ကိန်းပြည့် r;

        assert(s);
        assert(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = a.ether;
        ပြန်ပေး 0;
}

void ether_addr_mark_random(sa_nracanaa ether_addr *addr) {
        assert(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

ကိန်းပြည့် hw_addr_ensure_broadcast(sa_nracanaa hw_addr_data *bcast_addr, uint16_t arp_type) {
        assert(bcast_addr);

        အကယ်၍ (!hw_addr_is_null(bcast_addr))
                ပြန်ပေး 0;

        switch (arp_type) {
        pak_sa ARPHRD_ETHER:
                *bcast_addr = (sa_nracanaa hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                ပြန်ပေး 0;
        pak_sa ARPHRD_INFINIBAND:
                *bcast_addr = (sa_nracanaa hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                ပြန်ပေး 0;
        default:
                ပြန်ပေး -EAFNOSUPPORT;
        }
}
