/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/sha256.h" /* IWYU pragma: export */

ਪੂਰਨ sha256_fd(ਪੂਰਨ fd, uint64_t max_ਮਾਪ, uint8_t ret[ਸਥਿਰ SHA256_DIGEST_ਮਾਪ]);

ਪੂਰਨ parse_sha256(ਅਚਲ ਅੱਖਰ *s, uint8_t ret[ਸਥਿਰ SHA256_DIGEST_ਮਾਪ]);

bool sha256_is_valid(ਅਚਲ ਅੱਖਰ *s) ਸ਼ੁੱਧ;

ਅੱਖਰ* sha256_ਸਿੱਧਾ_hex(ਅਚਲ ਖਾਲੀ *ਬਫਰ, size_t sz);

/* The hash of the empty string */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHA256_ਖਾਲੀ_ਥਾਵਾਂ                                                    \
        (ਅਚਲ uint8_t[SHA256_DIGEST_ਮਾਪ]) {                           \
                0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,         \
                0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,         \
                0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,         \
                0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,         \
        }
