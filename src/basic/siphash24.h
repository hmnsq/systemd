/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#அந்தர்பாவ "forward.h"

கட்டமைப்பு siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

வெறுமை siphash24_init(கட்டமைப்பு siphash *state, மாறிலி uint8_t k[நிலையான 16]);
வெறுமை siphash24_compress(மாறிலி வெறுமை *in, size_t inlen, கட்டமைப்பு siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((மாறிலி uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), அளவு(typeof(in)), (state))

நிலையான உள்வரி வெறுமை siphash24_compress_boolean(bool in, கட்டமைப்பு siphash *state) {
        siphash24_compress_byte(in, state);
}

நிலையான உள்வரி வெறுமை siphash24_compress_usec_t(usec_t in, கட்டமைப்பு siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

நிலையான உள்வரி வெறுமை siphash24_compress_safe(மாறிலி வெறுமை *in, size_t inlen, கட்டமைப்பு siphash *state) {
        எனில் (inlen == 0)
                திரும்பு;

        siphash24_compress(in, inlen, state);
}

வெறுமை siphash24_compress_string(மாறிலி எழுத்து *in, கட்டமைப்பு siphash *state);
வெறுமை siphash24_compress_iovec(மாறிலி கட்டமைப்பு iovec *iov, கட்டமைப்பு siphash *state);

uint64_t siphash24_finalize(கட்டமைப்பு siphash *state);

uint64_t siphash24(மாறிலி வெறுமை *in, size_t inlen, மாறிலி uint8_t k[நிலையான 16]);

uint64_t siphash24_uint64(uint64_t in, மாறிலி uint8_t k[நிலையான 16]);

uint64_t siphash24_string(மாறிலி எழுத்து *s, மாறிலி uint8_t k[நிலையான 16]);
