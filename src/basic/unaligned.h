/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <endian.h>

#समावेश "forward.h"

#समावेश "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

अटल अंतरभूत uint16_t unaligned_read_be16(स्थिर व्योम *_u) {
        स्थिर काष्ठा __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        वापस be16toh(u->x);
}

अटल अंतरभूत uint32_t unaligned_read_be32(स्थिर व्योम *_u) {
        स्थिर काष्ठा __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        वापस be32toh(u->x);
}

अटल अंतरभूत uint64_t unaligned_read_be64(स्थिर व्योम *_u) {
        स्थिर काष्ठा __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        वापस be64toh(u->x);
}

अटल अंतरभूत व्योम unaligned_write_be16(व्योम *_u, uint16_t a) {
        काष्ठा __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

अटल अंतरभूत व्योम unaligned_write_be32(व्योम *_u, uint32_t a) {
        काष्ठा __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

अटल अंतरभूत व्योम unaligned_write_be64(व्योम *_u, uint64_t a) {
        काष्ठा __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

अटल अंतरभूत uint16_t unaligned_read_le16(स्थिर व्योम *_u) {
        स्थिर काष्ठा __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        वापस le16toh(u->x);
}

अटल अंतरभूत uint32_t unaligned_read_le32(स्थिर व्योम *_u) {
        स्थिर काष्ठा __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        वापस le32toh(u->x);
}

अटल अंतरभूत uint64_t unaligned_read_le64(स्थिर व्योम *_u) {
        स्थिर काष्ठा __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        वापस le64toh(u->x);
}

अटल अंतरभूत व्योम unaligned_write_le16(व्योम *_u, uint16_t a) {
        काष्ठा __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

अटल अंतरभूत व्योम unaligned_write_le32(व्योम *_u, uint32_t a) {
        काष्ठा __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

अटल अंतरभूत व्योम unaligned_write_le64(व्योम *_u, uint64_t a) {
        काष्ठा __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
