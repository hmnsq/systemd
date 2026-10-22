/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#अन्तर्भाव "forward.h"

संरचना siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

शून्य siphash24_init(संरचना siphash *state, ध्रुव uint8_t k[स्थिर 16]);
शून्य siphash24_compress(ध्रुव शून्य *in, size_t inlen, संरचना siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((ध्रुव uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), परिमाण(typeof(in)), (state))

स्थिर अन्तःस्थ शून्य siphash24_compress_boolean(bool in, संरचना siphash *state) {
        siphash24_compress_byte(in, state);
}

स्थिर अन्तःस्थ शून्य siphash24_compress_usec_t(usec_t in, संरचना siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

स्थिर अन्तःस्थ शून्य siphash24_compress_safe(ध्रुव शून्य *in, size_t inlen, संरचना siphash *state) {
        जइ (inlen == 0)
                प्रत्यागम;

        siphash24_compress(in, inlen, state);
}

शून्य siphash24_compress_string(ध्रुव वर्ण *in, संरचना siphash *state);
शून्य siphash24_compress_iovec(ध्रुव संरचना iovec *iov, संरचना siphash *state);

uint64_t siphash24_finalize(संरचना siphash *state);

uint64_t siphash24(ध्रुव शून्य *in, size_t inlen, ध्रुव uint8_t k[स्थिर 16]);

uint64_t siphash24_uint64(uint64_t in, ध्रुव uint8_t k[स्थिर 16]);

uint64_t siphash24_string(ध्रुव वर्ण *s, ध्रुव uint8_t k[स्थिर 16]);
