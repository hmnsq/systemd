/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ہدایت once

#شامل "forward.h"

#شامل "../fundamental/sha256.h" /* IWYU pragma: export */

صحیح sha256_fd(صحیح fd, uint64_t max_سائز, uint8_t ret[ساکن SHA256_DIGEST_سائز]);

صحیح parse_sha256(ثابت حرف *s, uint8_t ret[ساکن SHA256_DIGEST_سائز]);

bool sha256_is_valid(ثابت حرف *s) خالص;

حرف* sha256_براہ_راست_hex(ثابت خالی *بفر, size_t sz);

/* The hash of the empty string */
#تعریف_کرو SHA256_خالی_خانے                                                    \
        (ثابت uint8_t[SHA256_DIGEST_سائز]) {                           \
                0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,         \
                0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,         \
                0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,         \
                0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,         \
        }
