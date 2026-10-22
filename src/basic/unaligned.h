/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <endian.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint16_t unaligned_read_be16(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *_u) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ಹಿಂತಿರುಗಿ be16toh(u->x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t unaligned_read_be32(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *_u) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ಹಿಂತಿರುಗಿ be32toh(u->x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t unaligned_read_be64(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *_u) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ಹಿಂತಿರುಗಿ be64toh(u->x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ unaligned_write_be16(ಶೂನ್ಯ *_u, uint16_t a) {
        ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ unaligned_write_be32(ಶೂನ್ಯ *_u, uint32_t a) {
        ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ unaligned_write_be64(ಶೂನ್ಯ *_u, uint64_t a) {
        ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint16_t unaligned_read_le16(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *_u) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ಹಿಂತಿರುಗಿ le16toh(u->x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t unaligned_read_le32(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *_u) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ಹಿಂತಿರುಗಿ le32toh(u->x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t unaligned_read_le64(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *_u) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ಹಿಂತಿರುಗಿ le64toh(u->x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ unaligned_write_le16(ಶೂನ್ಯ *_u, uint16_t a) {
        ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ unaligned_write_le32(ಶೂನ್ಯ *_u, uint32_t a) {
        ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ unaligned_write_le64(ಶೂನ್ಯ *_u, uint64_t a) {
        ರಚನೆ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
