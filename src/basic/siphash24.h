/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#अन्तर्भाव "forward.h"

संरचना siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        माप_प्रकार inlen;
};

शून्य siphash24_init(संरचना siphash *state, स्थिरांक uint8_t k[स्थिर 16]);
शून्य siphash24_compress(स्थिरांक शून्य *in, माप_प्रकार inlen, संरचना siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((स्थिरांक uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), आकार(typeof(in)), (state))

स्थिर ओळीत शून्य siphash24_compress_boolean(bool in, संरचना siphash *state) {
        siphash24_compress_byte(in, state);
}

स्थिर ओळीत शून्य siphash24_compress_usec_t(usec_t in, संरचना siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

स्थिर ओळीत शून्य siphash24_compress_safe(स्थिरांक शून्य *in, माप_प्रकार inlen, संरचना siphash *state) {
        जर (inlen == 0)
                परत;

        siphash24_compress(in, inlen, state);
}

शून्य siphash24_compress_string(स्थिरांक अक्षर *in, संरचना siphash *state);
शून्य siphash24_compress_iovec(स्थिरांक संरचना iovec *iov, संरचना siphash *state);

uint64_t siphash24_finalize(संरचना siphash *state);

uint64_t siphash24(स्थिरांक शून्य *in, माप_प्रकार inlen, स्थिरांक uint8_t k[स्थिर 16]);

uint64_t siphash24_uint64(uint64_t in, स्थिरांक uint8_t k[स्थिर 16]);

uint64_t siphash24_string(स्थिरांक अक्षर *s, स्थिरांक uint8_t k[स्थिर 16]);
