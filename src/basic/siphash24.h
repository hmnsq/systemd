/* SPDX-License-Identifier: CC0-1.0 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ರಚನೆ siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

ಶೂನ್ಯ siphash24_init(ರಚನೆ siphash *state, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]);
ಶೂನ್ಯ siphash24_compress(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ, size_t inlen, ರಚನೆ siphash *state);
#ವ್ಯಾಖ್ಯಾನಿಸು siphash24_compress_byte(byte, state) siphash24_compress((ಸ್ಥಿರಾಂಕ uint8_t[]) { (byte) }, 1, (state))
#ವ್ಯಾಖ್ಯಾನಿಸು siphash24_compress_typesafe(ಒಳ, state)                  \
        siphash24_compress(&(ಒಳ), ಗಾತ್ರ(typeof(ಒಳ)), (state))

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ siphash24_compress_boolean(bool ಒಳ, ರಚನೆ siphash *state) {
        siphash24_compress_byte(ಒಳ, state);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ siphash24_compress_usec_t(usec_t ಒಳ, ರಚನೆ siphash *state) {
        uint64_t u = htole64(ಒಳ);
        siphash24_compress_typesafe(u, state);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ siphash24_compress_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ, size_t inlen, ರಚನೆ siphash *state) {
        ಆದರೆ (inlen == 0)
                ಹಿಂತಿರುಗಿ;

        siphash24_compress(ಒಳ, inlen, state);
}

ಶೂನ್ಯ siphash24_compress_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ, ರಚನೆ siphash *state);
ಶೂನ್ಯ siphash24_compress_iovec(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov, ರಚನೆ siphash *state);

uint64_t siphash24_finalize(ರಚನೆ siphash *state);

uint64_t siphash24(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ, size_t inlen, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]);

uint64_t siphash24_uint64(uint64_t ಒಳ, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]);

uint64_t siphash24_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]);
