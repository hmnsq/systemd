/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#അന്തര്ഭാവ "forward.h"

#അന്തര്ഭാവ "../fundamental/sha256.h" /* IWYU pragma: export */

പൂർണ്ണസംഖ്യ sha256_fd(പൂർണ്ണസംഖ്യ fd, uint64_t max_size, uint8_t ret[സ്ഥിതം SHA256_DIGEST_SIZE]);

പൂർണ്ണസംഖ്യ parse_sha256(സ്ഥിരം അക്ഷരം *s, uint8_t ret[സ്ഥിതം SHA256_DIGEST_SIZE]);

bool sha256_is_valid(സ്ഥിരം അക്ഷരം *s) _pure_;

അക്ഷരം* sha256_direct_hex(സ്ഥിരം ശൂന്യം *buffer, size_t sz);

/* The hash of the empty string */
#define SHA256_EMPTY                                                    \
        (സ്ഥിരം uint8_t[SHA256_DIGEST_SIZE]) {                           \
                0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,         \
                0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,         \
                0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,         \
                0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,         \
        }
