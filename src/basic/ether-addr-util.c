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

char* hw_addr_to_string_full(
                const struct hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                char buffer[static HW_ADDR_TO_STRING_MAX]) {

        assert(addr);
        assert(buffer);
        assert(addr->length <= HW_ADDR_MAX_SIZE);

        为 (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                如果 (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        返回 buffer;
}

struct hw_addr_data *hw_addr_set(struct hw_addr_data *addr, const uint8_t *bytes, size_t length) {
        assert(addr);
        assert(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        返回 addr;
}

整数 hw_addr_compare(const struct hw_addr_data *a, const struct hw_addr_data *b) {
        整数 r;

        assert(a);
        assert(b);

        r = CMP(a->length, b->length);
        如果 (r != 0)
                返回 r;

        返回 memcmp(a->bytes, b->bytes, a->length);
}

void hw_addr_hash_func(const struct hw_addr_data *p, struct siphash *state) {
        assert(p);
        assert(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(const struct hw_addr_data *addr) {
        assert(addr);
        返回 addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(const struct hw_addr_data *addr, uint16_t iftype) {
        assert(addr);

        switch (iftype) {
        case ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                如果 (addr->length != ETH_ALEN)
                        返回 false;

                返回 !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        case ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                如果 (addr->length != INFINIBAND_ALEN)
                        返回 false;

                返回 !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        default:
                返回 false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, struct hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(hw_addr_hash_ops_free, struct hw_addr_data, hw_addr_hash_func, hw_addr_compare, free);

char* ether_addr_to_string(const struct ether_addr *addr, char buffer[ETHER_ADDR_TO_STRING_MAX]) {
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

        返回 buffer;
}

整数 ether_addr_compare(const struct ether_addr *a, const struct ether_addr *b) {
        assert(a);
        assert(b);

        返回 memcmp(a, b, ETH_ALEN);
}

static void ether_addr_hash_func(const struct ether_addr *p, struct siphash *state) {
        assert(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(const struct ether_addr *addr) {
        assert(addr);
        返回 memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, struct ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(ether_addr_hash_ops_free, struct ether_addr, ether_addr_hash_func, ether_addr_compare, free);

static 整数 parse_hw_addr_one_field(const char **s, char sep, size_t len, uint8_t *buf) {
        const char *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        assert(s);
        assert(*s);
        assert(IN_SET(len, 1, 2));
        assert(buf);

        p = *s;

        为 (size_t i = 0; i < len * 2; i++) {
                const char *hexoff;
                size_t x;

                如果 (*p == '\0' || *p == sep) {
                        如果 (i == 0)
                                返回 -EINVAL;
                        break;
                }

                hexoff = strchr(hex, *p);
                如果 (!hexoff)
                        返回 -EINVAL;

                assert(hexoff >= hex);
                x = hexoff - hex;
                如果 (x >= 16)
                        x -= 6; /* A-F */

                assert(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        如果 (*p != '\0' && *p != sep)
                返回 -EINVAL;

        switch (len) {
        case 1:
                buf[0] = data;
                break;
        case 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                break;
        default:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        返回 cont;
}

整数 parse_hw_addr_full(const char *s, size_t expected_len, struct hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        char sep;
        整数 r;

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

        如果 (IN_SET(expected_len, 0, sizeof(struct in_addr), sizeof(struct in6_addr))) {
                union in_addr_union a;
                整数 family;

                如果 (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                否则 {
                        family = expected_len == sizeof(struct in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                如果 (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        返回 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        如果 (sep == '.')
                field_size = 2;
        否则 如果 (IN_SET(sep, ':', '-'))
                field_size = 1;
        否则
                返回 -EINVAL;

        如果 (max_len % field_size != 0)
                返回 -EINVAL;

        为 (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0) {
                        len = (i + 1) * field_size;
                        break;
                }
        }

        如果 (len == 0)
                返回 -EINVAL;

        如果 (expected_len == 0) {
                如果 (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        返回 -EINVAL;
        } 否则 如果 (expected_len != SIZE_MAX) {
                如果 (len != expected_len)
                        返回 -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        返回 0;
}

整数 parse_ether_addr(const char *s, struct ether_addr *ret) {
        struct hw_addr_data a;
        整数 r;

        assert(s);
        assert(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        如果 (r < 0)
                返回 r;

        *ret = a.ether;
        返回 0;
}

void ether_addr_mark_random(struct ether_addr *addr) {
        assert(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

整数 hw_addr_ensure_broadcast(struct hw_addr_data *bcast_addr, uint16_t arp_type) {
        assert(bcast_addr);

        如果 (!hw_addr_is_null(bcast_addr))
                返回 0;

        switch (arp_type) {
        case ARPHRD_ETHER:
                *bcast_addr = (struct hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                返回 0;
        case ARPHRD_INFINIBAND:
                *bcast_addr = (struct hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                返回 0;
        default:
                返回 -EAFNOSUPPORT;
        }
}
