/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <endian.h>

#അന്തര്ഭാവ "forward.h"

#അന്തര്ഭാവ "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

സ്ഥിതം വരിയിൽ uint16_t unaligned_read_be16(സ്ഥിരം ശൂന്യം *_u) {
        സ്ഥിരം ഘടന __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        തിരികെ be16toh(u->x);
}

സ്ഥിതം വരിയിൽ uint32_t unaligned_read_be32(സ്ഥിരം ശൂന്യം *_u) {
        സ്ഥിരം ഘടന __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        തിരികെ be32toh(u->x);
}

സ്ഥിതം വരിയിൽ uint64_t unaligned_read_be64(സ്ഥിരം ശൂന്യം *_u) {
        സ്ഥിരം ഘടന __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        തിരികെ be64toh(u->x);
}

സ്ഥിതം വരിയിൽ ശൂന്യം unaligned_write_be16(ശൂന്യം *_u, uint16_t a) {
        ഘടന __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

സ്ഥിതം വരിയിൽ ശൂന്യം unaligned_write_be32(ശൂന്യം *_u, uint32_t a) {
        ഘടന __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

സ്ഥിതം വരിയിൽ ശൂന്യം unaligned_write_be64(ശൂന്യം *_u, uint64_t a) {
        ഘടന __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

സ്ഥിതം വരിയിൽ uint16_t unaligned_read_le16(സ്ഥിരം ശൂന്യം *_u) {
        സ്ഥിരം ഘടന __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        തിരികെ le16toh(u->x);
}

സ്ഥിതം വരിയിൽ uint32_t unaligned_read_le32(സ്ഥിരം ശൂന്യം *_u) {
        സ്ഥിരം ഘടന __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        തിരികെ le32toh(u->x);
}

സ്ഥിതം വരിയിൽ uint64_t unaligned_read_le64(സ്ഥിരം ശൂന്യം *_u) {
        സ്ഥിരം ഘടന __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        തിരികെ le64toh(u->x);
}

സ്ഥിതം വരിയിൽ ശൂന്യം unaligned_write_le16(ശൂന്യം *_u, uint16_t a) {
        ഘടന __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

സ്ഥിതം വരിയിൽ ശൂന്യം unaligned_write_le32(ശൂന്യം *_u, uint32_t a) {
        ഘടന __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

സ്ഥിതം വരിയിൽ ശൂന്യം unaligned_write_le64(ശൂന്യം *_u, uint64_t a) {
        ഘടന __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
