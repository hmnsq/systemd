/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <endian.h>

#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

স্থির অন্তঃরেখ uint16_t unaligned_read_be16(ধ্রুবক শূন্য *_u) {
        ধ্রুবক কাঠামো __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ফেরত be16toh(u->x);
}

স্থির অন্তঃরেখ uint32_t unaligned_read_be32(ধ্রুবক শূন্য *_u) {
        ধ্রুবক কাঠামো __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ফেরত be32toh(u->x);
}

স্থির অন্তঃরেখ uint64_t unaligned_read_be64(ধ্রুবক শূন্য *_u) {
        ধ্রুবক কাঠামো __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ফেরত be64toh(u->x);
}

স্থির অন্তঃরেখ শূন্য unaligned_write_be16(শূন্য *_u, uint16_t a) {
        কাঠামো __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

স্থির অন্তঃরেখ শূন্য unaligned_write_be32(শূন্য *_u, uint32_t a) {
        কাঠামো __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

স্থির অন্তঃরেখ শূন্য unaligned_write_be64(শূন্য *_u, uint64_t a) {
        কাঠামো __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

স্থির অন্তঃরেখ uint16_t unaligned_read_le16(ধ্রুবক শূন্য *_u) {
        ধ্রুবক কাঠামো __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ফেরত le16toh(u->x);
}

স্থির অন্তঃরেখ uint32_t unaligned_read_le32(ধ্রুবক শূন্য *_u) {
        ধ্রুবক কাঠামো __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ফেরত le32toh(u->x);
}

স্থির অন্তঃরেখ uint64_t unaligned_read_le64(ধ্রুবক শূন্য *_u) {
        ধ্রুবক কাঠামো __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ফেরত le64toh(u->x);
}

স্থির অন্তঃরেখ শূন্য unaligned_write_le16(শূন্য *_u, uint16_t a) {
        কাঠামো __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

স্থির অন্তঃরেখ শূন্য unaligned_write_le32(শূন্য *_u, uint32_t a) {
        কাঠামো __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

স্থির অন্তঃরেখ শূন্য unaligned_write_le64(শূন্য *_u, uint64_t a) {
        কাঠামো __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
