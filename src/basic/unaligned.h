/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <endian.h>

#شامل "forward.h"

#شامل "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

ایستا درونخطی uint16_t unaligned_read_be16(ثابت تهی *_u) {
        ثابت ساختار __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        بازگشت be16toh(u->x);
}

ایستا درونخطی uint32_t unaligned_read_be32(ثابت تهی *_u) {
        ثابت ساختار __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        بازگشت be32toh(u->x);
}

ایستا درونخطی uint64_t unaligned_read_be64(ثابت تهی *_u) {
        ثابت ساختار __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        بازگشت be64toh(u->x);
}

ایستا درونخطی تهی unaligned_write_be16(تهی *_u, uint16_t a) {
        ساختار __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

ایستا درونخطی تهی unaligned_write_be32(تهی *_u, uint32_t a) {
        ساختار __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

ایستا درونخطی تهی unaligned_write_be64(تهی *_u, uint64_t a) {
        ساختار __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

ایستا درونخطی uint16_t unaligned_read_le16(ثابت تهی *_u) {
        ثابت ساختار __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        بازگشت le16toh(u->x);
}

ایستا درونخطی uint32_t unaligned_read_le32(ثابت تهی *_u) {
        ثابت ساختار __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        بازگشت le32toh(u->x);
}

ایستا درونخطی uint64_t unaligned_read_le64(ثابت تهی *_u) {
        ثابت ساختار __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        بازگشت le64toh(u->x);
}

ایستا درونخطی تهی unaligned_write_le16(تهی *_u, uint16_t a) {
        ساختار __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

ایستا درونخطی تهی unaligned_write_le32(تهی *_u, uint32_t a) {
        ساختار __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

ایستا درونخطی تهی unaligned_write_le64(تهی *_u, uint64_t a) {
        ساختار __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
