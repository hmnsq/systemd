/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/sha256.h" /* IWYU pragma: export */

ಪೂರ್ಣಾಂಕ sha256_fd(ಪೂರ್ಣಾಂಕ fd, uint64_t max_ಗಾತ್ರ, uint8_t ret[ಸ್ಥಿರ SHA256_DIGEST_ಗಾತ್ರ]);

ಪೂರ್ಣಾಂಕ parse_sha256(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint8_t ret[ಸ್ಥಿರ SHA256_DIGEST_ಗಾತ್ರ]);

bool sha256_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) ಶುದ್ಧ;

ಅಕ್ಷರ* sha256_ನೇರ_hex(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್, size_t sz);

/* The hash of the empty string */
#ವ್ಯಾಖ್ಯಾನಿಸು SHA256_ಖಾಲಿಗಳು                                                    \
        (ಸ್ಥಿರಾಂಕ uint8_t[SHA256_DIGEST_ಗಾತ್ರ]) {                           \
                0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,         \
                0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,         \
                0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,         \
                0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,         \
        }
