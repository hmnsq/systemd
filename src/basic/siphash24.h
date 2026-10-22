/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#അന്തര്ഭാവ "forward.h"

ഘടന siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

ശൂന്യം siphash24_init(ഘടന siphash *state, സ്ഥിരം uint8_t k[സ്ഥിതം 16]);
ശൂന്യം siphash24_compress(സ്ഥിരം ശൂന്യം *in, size_t inlen, ഘടന siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((സ്ഥിരം uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), വലുപ്പം(typeof(in)), (state))

സ്ഥിതം വരിയിൽ ശൂന്യം siphash24_compress_boolean(bool in, ഘടന siphash *state) {
        siphash24_compress_byte(in, state);
}

സ്ഥിതം വരിയിൽ ശൂന്യം siphash24_compress_usec_t(usec_t in, ഘടന siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

സ്ഥിതം വരിയിൽ ശൂന്യം siphash24_compress_safe(സ്ഥിരം ശൂന്യം *in, size_t inlen, ഘടന siphash *state) {
        എങ്കിൽ (inlen == 0)
                തിരികെ;

        siphash24_compress(in, inlen, state);
}

ശൂന്യം siphash24_compress_string(സ്ഥിരം അക്ഷരം *in, ഘടന siphash *state);
ശൂന്യം siphash24_compress_iovec(സ്ഥിരം ഘടന iovec *iov, ഘടന siphash *state);

uint64_t siphash24_finalize(ഘടന siphash *state);

uint64_t siphash24(സ്ഥിരം ശൂന്യം *in, size_t inlen, സ്ഥിരം uint8_t k[സ്ഥിതം 16]);

uint64_t siphash24_uint64(uint64_t in, സ്ഥിരം uint8_t k[സ്ഥിതം 16]);

uint64_t siphash24_string(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം uint8_t k[സ്ഥിതം 16]);
