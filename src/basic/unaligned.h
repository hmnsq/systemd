/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <endian.h>

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

स्थिर अन्तःस्थ uint16_t unaligned_read_be16(ध्रुव शून्य *_u) {
        ध्रुव संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        प्रत्यागम be16toh(u->x);
}

स्थिर अन्तःस्थ uint32_t unaligned_read_be32(ध्रुव शून्य *_u) {
        ध्रुव संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        प्रत्यागम be32toh(u->x);
}

स्थिर अन्तःस्थ uint64_t unaligned_read_be64(ध्रुव शून्य *_u) {
        ध्रुव संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        प्रत्यागम be64toh(u->x);
}

स्थिर अन्तःस्थ शून्य unaligned_write_be16(शून्य *_u, uint16_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

स्थिर अन्तःस्थ शून्य unaligned_write_be32(शून्य *_u, uint32_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

स्थिर अन्तःस्थ शून्य unaligned_write_be64(शून्य *_u, uint64_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

स्थिर अन्तःस्थ uint16_t unaligned_read_le16(ध्रुव शून्य *_u) {
        ध्रुव संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        प्रत्यागम le16toh(u->x);
}

स्थिर अन्तःस्थ uint32_t unaligned_read_le32(ध्रुव शून्य *_u) {
        ध्रुव संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        प्रत्यागम le32toh(u->x);
}

स्थिर अन्तःस्थ uint64_t unaligned_read_le64(ध्रुव शून्य *_u) {
        ध्रुव संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        प्रत्यागम le64toh(u->x);
}

स्थिर अन्तःस्थ शून्य unaligned_write_le16(शून्य *_u, uint16_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

स्थिर अन्तःस्थ शून्य unaligned_write_le32(शून्य *_u, uint32_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

स्थिर अन्तःस्थ शून्य unaligned_write_le64(शून्य *_u, uint64_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
