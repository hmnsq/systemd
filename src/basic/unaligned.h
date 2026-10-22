/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <endian.h>

#అన్తర్భావ "forward.h"

#అన్తర్భావ "../fundamental/unaligned.h" /* IWYU pragma: export */

/* BE */

స్థిర అంతఃవరుస uint16_t unaligned_read_be16(స్థిరరాశి శూన్య *_u) {
        స్థిరరాశి నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        తిరిగి be16toh(u->x);
}

స్థిర అంతఃవరుస uint32_t unaligned_read_be32(స్థిరరాశి శూన్య *_u) {
        స్థిరరాశి నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        తిరిగి be32toh(u->x);
}

స్థిర అంతఃవరుస uint64_t unaligned_read_be64(స్థిరరాశి శూన్య *_u) {
        స్థిరరాశి నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        తిరిగి be64toh(u->x);
}

స్థిర అంతఃవరుస శూన్య unaligned_write_be16(శూన్య *_u, uint16_t a) {
        నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = be16toh(a);
}

స్థిర అంతఃవరుస శూన్య unaligned_write_be32(శూన్య *_u, uint32_t a) {
        నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = be32toh(a);
}

స్థిర అంతఃవరుస శూన్య unaligned_write_be64(శూన్య *_u, uint64_t a) {
        నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = be64toh(a);
}

/* LE */

స్థిర అంతఃవరుస uint16_t unaligned_read_le16(స్థిరరాశి శూన్య *_u) {
        స్థిరరాశి నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        తిరిగి le16toh(u->x);
}

స్థిర అంతఃవరుస uint32_t unaligned_read_le32(స్థిరరాశి శూన్య *_u) {
        స్థిరరాశి నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        తిరిగి le32toh(u->x);
}

స్థిర అంతఃవరుస uint64_t unaligned_read_le64(స్థిరరాశి శూన్య *_u) {
        స్థిరరాశి నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        తిరిగి le64toh(u->x);
}

స్థిర అంతఃవరుస శూన్య unaligned_write_le16(శూన్య *_u, uint16_t a) {
        నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint16_t x; } *u = _u;

        u->x = le16toh(a);
}

స్థిర అంతఃవరుస శూన్య unaligned_write_le32(శూన్య *_u, uint32_t a) {
        నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint32_t x; } *u = _u;

        u->x = le32toh(a);
}

స్థిర అంతఃవరుస శూన్య unaligned_write_le64(శూన్య *_u, uint64_t a) {
        నిర్మాణం __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        u->x = le64toh(a);
}
