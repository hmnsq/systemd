/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <endian.h>

#અન્તર્ભાવ "forward.h"

#અન્તર્ભાવ "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

સ્થિર અંતઃરેખ uint16_t unaligned_read_be16(અચલ શૂન્ય *_u) {
        અચલ સંરચના __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        પાછા be16toh(u->x);
}

સ્થિર અંતઃરેખ uint32_t unaligned_read_be32(અચલ શૂન્ય *_u) {
        અચલ સંરચના __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        પાછા be32toh(u->x);
}

સ્થિર અંતઃરેખ uint64_t unaligned_read_be64(અચલ શૂન્ય *_u) {
        અચલ સંરચના __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        પાછા be64toh(u->x);
}

સ્થિર અંતઃરેખ શૂન્ય unaligned_write_be16(શૂન્ય *_u, uint16_t a) {
        સંરચના __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

સ્થિર અંતઃરેખ શૂન્ય unaligned_write_be32(શૂન્ય *_u, uint32_t a) {
        સંરચના __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

સ્થિર અંતઃરેખ શૂન્ય unaligned_write_be64(શૂન્ય *_u, uint64_t a) {
        સંરચના __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

સ્થિર અંતઃરેખ uint16_t unaligned_read_le16(અચલ શૂન્ય *_u) {
        અચલ સંરચના __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        પાછા le16toh(u->x);
}

સ્થિર અંતઃરેખ uint32_t unaligned_read_le32(અચલ શૂન્ય *_u) {
        અચલ સંરચના __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        પાછા le32toh(u->x);
}

સ્થિર અંતઃરેખ uint64_t unaligned_read_le64(અચલ શૂન્ય *_u) {
        અચલ સંરચના __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        પાછા le64toh(u->x);
}

સ્થિર અંતઃરેખ શૂન્ય unaligned_write_le16(શૂન્ય *_u, uint16_t a) {
        સંરચના __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

સ્થિર અંતઃરેખ શૂન્ય unaligned_write_le32(શૂન્ય *_u, uint32_t a) {
        સંરચના __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

સ્થિર અંતઃરેખ શૂન્ય unaligned_write_le64(શૂન્ય *_u, uint64_t a) {
        સંરચના __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
