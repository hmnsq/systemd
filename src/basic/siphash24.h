/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#অন্তর্ভাব "forward.h"

সংরচনা siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

শূন্য siphash24_init(সংরচনা siphash *state, ধ্রুব uint8_t k[স্থির 16]);
শূন্য siphash24_compress(ধ্রুব শূন্য *in, size_t inlen, সংরচনা siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((ধ্রুব uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), পরিমাণ(typeof(in)), (state))

স্থির অন্তঃস্থ শূন্য siphash24_compress_boolean(bool in, সংরচনা siphash *state) {
        siphash24_compress_byte(in, state);
}

স্থির অন্তঃস্থ শূন্য siphash24_compress_usec_t(usec_t in, সংরচনা siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

স্থির অন্তঃস্থ শূন্য siphash24_compress_safe(ধ্রুব শূন্য *in, size_t inlen, সংরচনা siphash *state) {
        যদি (inlen == 0)
                প্রত্যাগম;

        siphash24_compress(in, inlen, state);
}

শূন্য siphash24_compress_string(ধ্রুব বর্ণ *in, সংরচনা siphash *state);
শূন্য siphash24_compress_iovec(ধ্রুব সংরচনা iovec *iov, সংরচনা siphash *state);

uint64_t siphash24_finalize(সংরচনা siphash *state);

uint64_t siphash24(ধ্রুব শূন্য *in, size_t inlen, ধ্রুব uint8_t k[স্থির 16]);

uint64_t siphash24_uint64(uint64_t in, ধ্রুব uint8_t k[স্থির 16]);

uint64_t siphash24_string(ধ্রুব বর্ণ *s, ধ্রুব uint8_t k[স্থির 16]);
