/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <endian.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

#ଅନ୍ତର୍ଭାଵ "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint16_t unaligned_read_be16(ଅଚଳ ଶୂନ୍ୟ *_u) {
        ଅଚଳ ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ଫେରନ୍ତୁ be16toh(u->x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t unaligned_read_be32(ଅଚଳ ଶୂନ୍ୟ *_u) {
        ଅଚଳ ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ଫେରନ୍ତୁ be32toh(u->x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t unaligned_read_be64(ଅଚଳ ଶୂନ୍ୟ *_u) {
        ଅଚଳ ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ଫେରନ୍ତୁ be64toh(u->x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ unaligned_write_be16(ଶୂନ୍ୟ *_u, uint16_t a) {
        ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ unaligned_write_be32(ଶୂନ୍ୟ *_u, uint32_t a) {
        ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ unaligned_write_be64(ଶୂନ୍ୟ *_u, uint64_t a) {
        ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint16_t unaligned_read_le16(ଅଚଳ ଶୂନ୍ୟ *_u) {
        ଅଚଳ ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        ଫେରନ୍ତୁ le16toh(u->x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t unaligned_read_le32(ଅଚଳ ଶୂନ୍ୟ *_u) {
        ଅଚଳ ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        ଫେରନ୍ତୁ le32toh(u->x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t unaligned_read_le64(ଅଚଳ ଶୂନ୍ୟ *_u) {
        ଅଚଳ ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        ଫେରନ୍ତୁ le64toh(u->x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ unaligned_write_le16(ଶୂନ୍ୟ *_u, uint16_t a) {
        ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ unaligned_write_le32(ଶୂନ୍ୟ *_u, uint32_t a) {
        ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ unaligned_write_le64(ଶୂନ୍ୟ *_u, uint64_t a) {
        ସଂରଚନା __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
