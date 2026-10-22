/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#شامل "forward.h"

ساختار siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

تهی siphash24_init(ساختار siphash *state, ثابت uint8_t k[ایستا 16]);
تهی siphash24_compress(ثابت تهی *in, size_t inlen, ساختار siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((ثابت uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), اندازه(typeof(in)), (state))

ایستا درونخطی تهی siphash24_compress_boolean(bool in, ساختار siphash *state) {
        siphash24_compress_byte(in, state);
}

ایستا درونخطی تهی siphash24_compress_usec_t(usec_t in, ساختار siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

ایستا درونخطی تهی siphash24_compress_safe(ثابت تهی *in, size_t inlen, ساختار siphash *state) {
        اگر (inlen == 0)
                بازگشت;

        siphash24_compress(in, inlen, state);
}

تهی siphash24_compress_string(ثابت نویسه *in, ساختار siphash *state);
تهی siphash24_compress_iovec(ثابت ساختار iovec *iov, ساختار siphash *state);

uint64_t siphash24_finalize(ساختار siphash *state);

uint64_t siphash24(ثابت تهی *in, size_t inlen, ثابت uint8_t k[ایستا 16]);

uint64_t siphash24_uint64(uint64_t in, ثابت uint8_t k[ایستا 16]);

uint64_t siphash24_string(ثابت نویسه *s, ثابت uint8_t k[ایستا 16]);
