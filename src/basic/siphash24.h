/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#অন্তর্ভাব "forward.h"

কাঠামো siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

শূন্য siphash24_init(কাঠামো siphash *state, ধ্রুবক uint8_t k[স্থির 16]);
শূন্য siphash24_compress(ধ্রুবক শূন্য *in, size_t inlen, কাঠামো siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((ধ্রুবক uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), আকার(typeof(in)), (state))

স্থির অন্তঃরেখ শূন্য siphash24_compress_boolean(bool in, কাঠামো siphash *state) {
        siphash24_compress_byte(in, state);
}

স্থির অন্তঃরেখ শূন্য siphash24_compress_usec_t(usec_t in, কাঠামো siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

স্থির অন্তঃরেখ শূন্য siphash24_compress_safe(ধ্রুবক শূন্য *in, size_t inlen, কাঠামো siphash *state) {
        যদি (inlen == 0)
                ফেরত;

        siphash24_compress(in, inlen, state);
}

শূন্য siphash24_compress_string(ধ্রুবক অক্ষর *in, কাঠামো siphash *state);
শূন্য siphash24_compress_iovec(ধ্রুবক কাঠামো iovec *iov, কাঠামো siphash *state);

uint64_t siphash24_finalize(কাঠামো siphash *state);

uint64_t siphash24(ধ্রুবক শূন্য *in, size_t inlen, ধ্রুবক uint8_t k[স্থির 16]);

uint64_t siphash24_uint64(uint64_t in, ধ্রুবক uint8_t k[স্থির 16]);

uint64_t siphash24_string(ধ্রুবক অক্ষর *s, ধ্রুবক uint8_t k[স্থির 16]);
