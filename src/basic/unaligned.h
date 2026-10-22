/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <endian.h>

#شامل "forward.h"

#شامل "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

ساکن اندرونی uint16_t unaligned_read_be16(ثابت خالی *_u) {
        ثابت ساخت __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        واپس be16toh(u->x);
}

ساکن اندرونی uint32_t unaligned_read_be32(ثابت خالی *_u) {
        ثابت ساخت __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        واپس be32toh(u->x);
}

ساکن اندرونی uint64_t unaligned_read_be64(ثابت خالی *_u) {
        ثابت ساخت __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        واپس be64toh(u->x);
}

ساکن اندرونی خالی unaligned_write_be16(خالی *_u, uint16_t a) {
        ساخت __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

ساکن اندرونی خالی unaligned_write_be32(خالی *_u, uint32_t a) {
        ساخت __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

ساکن اندرونی خالی unaligned_write_be64(خالی *_u, uint64_t a) {
        ساخت __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

ساکن اندرونی uint16_t unaligned_read_le16(ثابت خالی *_u) {
        ثابت ساخت __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        واپس le16toh(u->x);
}

ساکن اندرونی uint32_t unaligned_read_le32(ثابت خالی *_u) {
        ثابت ساخت __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        واپس le32toh(u->x);
}

ساکن اندرونی uint64_t unaligned_read_le64(ثابت خالی *_u) {
        ثابت ساخت __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        واپس le64toh(u->x);
}

ساکن اندرونی خالی unaligned_write_le16(خالی *_u, uint16_t a) {
        ساخت __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

ساکن اندرونی خالی unaligned_write_le32(خالی *_u, uint32_t a) {
        ساخت __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

ساکن اندرونی خالی unaligned_write_le64(خالی *_u, uint64_t a) {
        ساخت __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
