/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <endian.h>

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

स्थिर ओळीत uint16_t unaligned_read_be16(स्थिरांक शून्य *_u) {
        स्थिरांक संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        परत be16toh(u->x);
}

स्थिर ओळीत uint32_t unaligned_read_be32(स्थिरांक शून्य *_u) {
        स्थिरांक संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        परत be32toh(u->x);
}

स्थिर ओळीत uint64_t unaligned_read_be64(स्थिरांक शून्य *_u) {
        स्थिरांक संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        परत be64toh(u->x);
}

स्थिर ओळीत शून्य unaligned_write_be16(शून्य *_u, uint16_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

स्थिर ओळीत शून्य unaligned_write_be32(शून्य *_u, uint32_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

स्थिर ओळीत शून्य unaligned_write_be64(शून्य *_u, uint64_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

स्थिर ओळीत uint16_t unaligned_read_le16(स्थिरांक शून्य *_u) {
        स्थिरांक संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        परत le16toh(u->x);
}

स्थिर ओळीत uint32_t unaligned_read_le32(स्थिरांक शून्य *_u) {
        स्थिरांक संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        परत le32toh(u->x);
}

स्थिर ओळीत uint64_t unaligned_read_le64(स्थिरांक शून्य *_u) {
        स्थिरांक संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        परत le64toh(u->x);
}

स्थिर ओळीत शून्य unaligned_write_le16(शून्य *_u, uint16_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

स्थिर ओळीत शून्य unaligned_write_le32(शून्य *_u, uint32_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

स्थिर ओळीत शून्य unaligned_write_le64(शून्य *_u, uint64_t a) {
        संरचना __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
