/* SPDX-License-Identifier: LGPL-2.1-or-later */

#आशय once

#समावेश "forward.h"

#समावेश "../fundamental/sha256.h" /* IWYU pragma: export */

पूर्णांक sha256_fd(पूर्णांक fd, uint64_t max_आकार, uint8_t ret[अटल SHA256_DIGEST_आकार]);

पूर्णांक parse_sha256(स्थिर अक्षर *s, uint8_t ret[अटल SHA256_DIGEST_आकार]);

bool sha256_is_valid(स्थिर अक्षर *s) _pure_;

अक्षर* sha256_direct_hex(स्थिर व्योम *बफर, माप_प्रकार sz);

/* The hash of the empty string */
#घोषणा SHA256_EMPTY                                                    \
        (स्थिर uint8_t[SHA256_DIGEST_आकार]) {                           \
                0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,         \
                0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,         \
                0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,         \
                0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,         \
        }
