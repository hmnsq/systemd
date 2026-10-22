/* SPDX-License-Identifier: LGPL-2.1-or-later */

#సూచన once

#అన్తర్భావ "forward.h"

#అన్తర్భావ "../fundamental/sha256.h" /* IWYU pragma: export */

పూర్ణాంకం sha256_fd(పూర్ణాంకం fd, uint64_t max_కొలత, uint8_t ret[స్థిర SHA256_DIGEST_కొలత]);

పూర్ణాంకం parse_sha256(స్థిరరాశి అక్షరం *s, uint8_t ret[స్థిర SHA256_DIGEST_కొలత]);

bool sha256_is_valid(స్థిరరాశి అక్షరం *s) శుద్ధ;

అక్షరం* sha256_ప్రత్యక్ష_hex(స్థిరరాశి శూన్య *బఫరు, size_t sz);

/* The hash of the empty string */
#నిర్వచించు SHA256_ఖాళీలు                                                    \
        (స్థిరరాశి uint8_t[SHA256_DIGEST_కొలత]) {                           \
                0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,         \
                0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,         \
                0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,         \
                0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,         \
        }
