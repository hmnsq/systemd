/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <endian.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

ਸਥਿਰ ਸਤਰਅੰਦਰ uint16_t unaligned_read_be16(ਅਚਲ ਖਾਲੀ *_u) {
        ਅਚਲ ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ਵਾਪਸ be16toh(u->x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t unaligned_read_be32(ਅਚਲ ਖਾਲੀ *_u) {
        ਅਚਲ ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ਵਾਪਸ be32toh(u->x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t unaligned_read_be64(ਅਚਲ ਖਾਲੀ *_u) {
        ਅਚਲ ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ਵਾਪਸ be64toh(u->x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ unaligned_write_be16(ਖਾਲੀ *_u, uint16_t a) {
        ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ unaligned_write_be32(ਖਾਲੀ *_u, uint32_t a) {
        ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ unaligned_write_be64(ਖਾਲੀ *_u, uint64_t a) {
        ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

ਸਥਿਰ ਸਤਰਅੰਦਰ uint16_t unaligned_read_le16(ਅਚਲ ਖਾਲੀ *_u) {
        ਅਚਲ ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ਵਾਪਸ le16toh(u->x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t unaligned_read_le32(ਅਚਲ ਖਾਲੀ *_u) {
        ਅਚਲ ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ਵਾਪਸ le32toh(u->x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t unaligned_read_le64(ਅਚਲ ਖਾਲੀ *_u) {
        ਅਚਲ ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ਵਾਪਸ le64toh(u->x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ unaligned_write_le16(ਖਾਲੀ *_u, uint16_t a) {
        ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ unaligned_write_le32(ਖਾਲੀ *_u, uint32_t a) {
        ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ unaligned_write_le64(ਖਾਲੀ *_u, uint64_t a) {
        ਬਣਤਰ __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
