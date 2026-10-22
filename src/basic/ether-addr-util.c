/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <net/ethernet.h>
#شامل <net/if_arp.h>
#شامل <stdio.h>

#شامل "ether-addr-util.h"
#شامل "hash-funcs.h"
#شامل "hexdecoct.h"
#شامل "in-addr-util.h"
#شامل "memory-util.h"
#شامل "siphash24.h"
#شامل "string-util.h"

نویسه* hw_addr_to_string_full(
                ثابت ساختار hw_addr_data *addr,
                HardwareAddressToStringFlags flags,
                نویسه buffer[ایستا HW_ADDR_TO_STRING_MAX]) {

        أكد(addr);
        أكد(buffer);
        أكد(addr->length <= HW_ADDR_MAX_SIZE);

        برای (size_t i = 0, j = 0; i < addr->length; i++) {
                buffer[j++] = hexchar(addr->bytes[i] >> 4);
                buffer[j++] = hexchar(addr->bytes[i] & 0x0f);
                اگر (!FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON))
                        buffer[j++] = ':';
        }

        buffer[addr->length == 0 || FLAGS_SET(flags, HW_ADDR_TO_STRING_NO_COLON) ?
               addr->length * 2 :
               addr->length * 3 - 1] = '\0';
        بازگشت buffer;
}

ساختار hw_addr_data *hw_addr_set(ساختار hw_addr_data *addr, ثابت uint8_t *bytes, size_t length) {
        أكد(addr);
        أكد(length <= HW_ADDR_MAX_SIZE);

        addr->length = length;
        memcpy_safe(addr->bytes, bytes, length);
        بازگشت addr;
}

صحیح hw_addr_compare(ثابت ساختار hw_addr_data *a, ثابت ساختار hw_addr_data *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(a->length, b->length);
        اگر (r != 0)
                بازگشت r;

        بازگشت memcmp(a->bytes, b->bytes, a->length);
}

تهی hw_addr_hash_func(ثابت ساختار hw_addr_data *p, ساختار siphash *state) {
        أكد(p);
        أكد(state);

        siphash24_compress_typesafe(p->length, state);
        siphash24_compress_safe(p->bytes, p->length, state);
}

bool hw_addr_is_null(ثابت ساختار hw_addr_data *addr) {
        أكد(addr);
        بازگشت addr->length == 0 || memeqzero(addr->bytes, addr->length);
}

bool hw_addr_is_valid(ثابت ساختار hw_addr_data *addr, uint16_t iftype) {
        أكد(addr);

        انتخاب (iftype) {
        حالت ARPHRD_ETHER:
                /* Refuse all zero and all 0xFF. */
                اگر (addr->length != ETH_ALEN)
                        بازگشت false;

                بازگشت !ether_addr_is_null(&addr->ether) && !ether_addr_is_broadcast(&addr->ether);

        حالت ARPHRD_INFINIBAND:
                /* The last 8 bytes cannot be zero. */
                اگر (addr->length != INFINIBAND_ALEN)
                        بازگشت false;

                بازگشت !memeqzero(addr->bytes + INFINIBAND_ALEN - 8, 8);

        پیشفرض:
                بازگشت false;
        }
}

DEFINE_HASH_OPS(hw_addr_hash_ops, ساختار hw_addr_data, hw_addr_hash_func, hw_addr_compare);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(hw_addr_hash_ops_free, ساختار hw_addr_data, hw_addr_hash_func, hw_addr_compare, آزاد);

نویسه* ether_addr_to_string(ثابت ساختار ether_addr *addr, نویسه buffer[ETHER_ADDR_TO_STRING_MAX]) {
        أكد(addr);
        أكد(buffer);

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

        بازگشت buffer;
}

صحیح ether_addr_compare(ثابت ساختار ether_addr *a, ثابت ساختار ether_addr *b) {
        أكد(a);
        أكد(b);

        بازگشت memcmp(a, b, ETH_ALEN);
}

ایستا تهی ether_addr_hash_func(ثابت ساختار ether_addr *p, ساختار siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}

bool ether_addr_is_broadcast(ثابت ساختار ether_addr *addr) {
        أكد(addr);
        بازگشت memeqbyte(0xff, addr->ether_addr_octet, ETH_ALEN);
}

DEFINE_HASH_OPS(ether_addr_hash_ops, ساختار ether_addr, ether_addr_hash_func, ether_addr_compare);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(ether_addr_hash_ops_free, ساختار ether_addr, ether_addr_hash_func, ether_addr_compare, آزاد);

ایستا صحیح parse_hw_addr_one_field(ثابت نویسه **s, نویسه sep, size_t len, uint8_t *buf) {
        ثابت نویسه *hex = HEXDIGITS, *p;
        uint16_t data = 0;
        bool cont;

        أكد(s);
        أكد(*s);
        أكد(IN_SET(len, 1, 2));
        أكد(buf);

        p = *s;

        برای (size_t i = 0; i < len * 2; i++) {
                ثابت نویسه *hexoff;
                size_t x;

                اگر (*p == '\0' || *p == sep) {
                        اگر (i == 0)
                                بازگشت -EINVAL;
                        بشکن;
                }

                hexoff = strchr(hex, *p);
                اگر (!hexoff)
                        بازگشت -EINVAL;

                أكد(hexoff >= hex);
                x = hexoff - hex;
                اگر (x >= 16)
                        x -= 6; /* A-F */

                أكد(x < 16);
                data <<= 4;
                data += x;

                p++;
        }

        اگر (*p != '\0' && *p != sep)
                بازگشت -EINVAL;

        انتخاب (len) {
        حالت 1:
                buf[0] = data;
                بشکن;
        حالت 2:
                buf[0] = (data & 0xff00) >> 8;
                buf[1] = data & 0xff;
                بشکن;
        پیشفرض:
                assert_not_reached();
        }

        cont = *p == sep;
        *s = p + cont;
        بازگشت cont;
}

صحیح parse_hw_addr_full(ثابت نویسه *s, size_t expected_len, ساختار hw_addr_data *ret) {
        size_t field_size, max_len, len = 0;
        uint8_t bytes[HW_ADDR_MAX_SIZE];
        نویسه sep;
        صحیح r;

        أكد(s);
        أكد(expected_len <= HW_ADDR_MAX_SIZE || expected_len == SIZE_MAX);
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

        اگر (IN_SET(expected_len, 0, اندازه(ساختار in_addr), اندازه(ساختار in6_addr))) {
                اجتماع in_addr_union a;
                صحیح family;

                اگر (expected_len == 0)
                        r = in_addr_from_string_auto(s, &family, &a);
                وگرنه {
                        family = expected_len == اندازه(ساختار in_addr) ? AF_INET : AF_INET6;
                        r = in_addr_from_string(family, s, &a);
                }
                اگر (r >= 0) {
                        ret->length = FAMILY_ADDRESS_SIZE(family);
                        memcpy(ret->bytes, a.bytes, ret->length);
                        بازگشت 0;
                }
        }

        max_len =
                expected_len == 0 ? INFINIBAND_ALEN :
                expected_len == SIZE_MAX ? HW_ADDR_MAX_SIZE : expected_len;
        sep = s[strspn(s, HEXDIGITS)];

        اگر (sep == '.')
                field_size = 2;
        وگرنه اگر (IN_SET(sep, ':', '-'))
                field_size = 1;
        وگرنه
                بازگشت -EINVAL;

        اگر (max_len % field_size != 0)
                بازگشت -EINVAL;

        برای (size_t i = 0; i < max_len / field_size; i++) {
                r = parse_hw_addr_one_field(&s, sep, field_size, bytes + i * field_size);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0) {
                        len = (i + 1) * field_size;
                        بشکن;
                }
        }

        اگر (len == 0)
                بازگشت -EINVAL;

        اگر (expected_len == 0) {
                اگر (!IN_SET(len, 4, 16, ETH_ALEN, INFINIBAND_ALEN))
                        بازگشت -EINVAL;
        } وگرنه اگر (expected_len != SIZE_MAX) {
                اگر (len != expected_len)
                        بازگشت -EINVAL;
        }

        ret->length = len;
        memcpy(ret->bytes, bytes, ret->length);
        بازگشت 0;
}

صحیح parse_ether_addr(ثابت نویسه *s, ساختار ether_addr *ret) {
        ساختار hw_addr_data a;
        صحیح r;

        أكد(s);
        أكد(ret);

        r = parse_hw_addr_full(s, ETH_ALEN, &a);
        اگر (r < 0)
                بازگشت r;

        *ret = a.ether;
        بازگشت 0;
}

تهی ether_addr_mark_random(ساختار ether_addr *addr) {
        أكد(addr);

        /* see eth_random_addr in the kernel */
        addr->ether_addr_octet[0] &= 0xfe;        /* clear multicast bit */
        addr->ether_addr_octet[0] |= 0x02;        /* set local assignment bit (IEEE802) */
}

صحیح hw_addr_ensure_broadcast(ساختار hw_addr_data *bcast_addr, uint16_t arp_type) {
        أكد(bcast_addr);

        اگر (!hw_addr_is_null(bcast_addr))
                بازگشت 0;

        انتخاب (arp_type) {
        حالت ARPHRD_ETHER:
                *bcast_addr = (ساختار hw_addr_data) {
                        .length = ETH_ALEN,
                        .ether = {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }},
                };
                بازگشت 0;
        حالت ARPHRD_INFINIBAND:
                *bcast_addr = (ساختار hw_addr_data) {
                        .length = INFINIBAND_ALEN,
                        .infiniband = {
                                0x00, 0xff, 0xff, 0xff, 0xff, 0x12, 0x40, 0x1b,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0xff, 0xff, 0xff, 0xff,
                        },
                };
                بازگشت 0;
        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}
