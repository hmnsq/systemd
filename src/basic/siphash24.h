/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸಂರಚನಾ siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

ಶೂನ್ಯ siphash24_init(ಸಂರಚನಾ siphash *state, ಧ್ರುವ uint8_t k[ಸ್ಥಿರ 16]);
ಶೂನ್ಯ siphash24_compress(ಧ್ರುವ ಶೂನ್ಯ *in, size_t inlen, ಸಂರಚನಾ siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((ಧ್ರುವ uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), ಪರಿಮಾಣ(typeof(in)), (state))

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ siphash24_compress_boolean(bool in, ಸಂರಚನಾ siphash *state) {
        siphash24_compress_byte(in, state);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ siphash24_compress_usec_t(usec_t in, ಸಂರಚನಾ siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ siphash24_compress_safe(ಧ್ರುವ ಶೂನ್ಯ *in, size_t inlen, ಸಂರಚನಾ siphash *state) {
        ಯದಿ (inlen == 0)
                ಪ್ರತ್ಯಾಗಮ;

        siphash24_compress(in, inlen, state);
}

ಶೂನ್ಯ siphash24_compress_string(ಧ್ರುವ ವರ್ಣ *in, ಸಂರಚನಾ siphash *state);
ಶೂನ್ಯ siphash24_compress_iovec(ಧ್ರುವ ಸಂರಚನಾ iovec *iov, ಸಂರಚನಾ siphash *state);

uint64_t siphash24_finalize(ಸಂರಚನಾ siphash *state);

uint64_t siphash24(ಧ್ರುವ ಶೂನ್ಯ *in, size_t inlen, ಧ್ರುವ uint8_t k[ಸ್ಥಿರ 16]);

uint64_t siphash24_uint64(uint64_t in, ಧ್ರುವ uint8_t k[ಸ್ಥಿರ 16]);

uint64_t siphash24_string(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ uint8_t k[ಸ್ಥಿರ 16]);
