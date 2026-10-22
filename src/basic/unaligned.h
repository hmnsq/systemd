/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <endian.h>

#அந்தர்பாவ "forward.h"

#அந்தர்பாவ "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

நிலையான உள்வரி uint16_t unaligned_read_be16(மாறிலி வெறுமை *_u) {
        மாறிலி கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        திரும்பு be16toh(u->x);
}

நிலையான உள்வரி uint32_t unaligned_read_be32(மாறிலி வெறுமை *_u) {
        மாறிலி கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        திரும்பு be32toh(u->x);
}

நிலையான உள்வரி uint64_t unaligned_read_be64(மாறிலி வெறுமை *_u) {
        மாறிலி கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        திரும்பு be64toh(u->x);
}

நிலையான உள்வரி வெறுமை unaligned_write_be16(வெறுமை *_u, uint16_t a) {
        கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

நிலையான உள்வரி வெறுமை unaligned_write_be32(வெறுமை *_u, uint32_t a) {
        கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

நிலையான உள்வரி வெறுமை unaligned_write_be64(வெறுமை *_u, uint64_t a) {
        கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

நிலையான உள்வரி uint16_t unaligned_read_le16(மாறிலி வெறுமை *_u) {
        மாறிலி கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        திரும்பு le16toh(u->x);
}

நிலையான உள்வரி uint32_t unaligned_read_le32(மாறிலி வெறுமை *_u) {
        மாறிலி கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        திரும்பு le32toh(u->x);
}

நிலையான உள்வரி uint64_t unaligned_read_le64(மாறிலி வெறுமை *_u) {
        மாறிலி கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        திரும்பு le64toh(u->x);
}

நிலையான உள்வரி வெறுமை unaligned_write_le16(வெறுமை *_u, uint16_t a) {
        கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

நிலையான உள்வரி வெறுமை unaligned_write_le32(வெறுமை *_u, uint32_t a) {
        கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

நிலையான உள்வரி வெறுமை unaligned_write_le64(வெறுமை *_u, uint64_t a) {
        கட்டமைப்பு __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
