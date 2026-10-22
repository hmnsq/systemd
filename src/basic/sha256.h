/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/sha256.h" /* IWYU pragma: export */

পূর্ণসংখ্যা sha256_fd(পূর্ণসংখ্যা fd, uint64_t max_আকার, uint8_t ret[স্থির SHA256_DIGEST_আকার]);

পূর্ণসংখ্যা parse_sha256(ধ্রুবক অক্ষর *s, uint8_t ret[স্থির SHA256_DIGEST_আকার]);

bool sha256_is_valid(ধ্রুবক অক্ষর *s) _pure_;

অক্ষর* sha256_direct_hex(ধ্রুবক শূন্য *বাফার, size_t sz);

/* The hash of the empty string */
#define SHA256_EMPTY                                                    \
        (ধ্রুবক uint8_t[SHA256_DIGEST_আকার]) {                           \
                0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,         \
                0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,         \
                0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,         \
                0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,         \
        }
