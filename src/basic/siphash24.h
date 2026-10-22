/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#ضمن "forward.h"

بنية siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

فراغ siphash24_init(بنية siphash *state, ثابت uint8_t k[ساكن 16]);
فراغ siphash24_compress(ثابت فراغ *in, size_t inlen, بنية siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((ثابت uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), حجم(typeof(in)), (state))

ساكن مضمن فراغ siphash24_compress_boolean(bool in, بنية siphash *state) {
        siphash24_compress_byte(in, state);
}

ساكن مضمن فراغ siphash24_compress_usec_t(usec_t in, بنية siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

ساكن مضمن فراغ siphash24_compress_safe(ثابت فراغ *in, size_t inlen, بنية siphash *state) {
        𐡄𐡍 (inlen == 0)
                أرجع;

        siphash24_compress(in, inlen, state);
}

فراغ siphash24_compress_string(ثابت حرف *in, بنية siphash *state);
فراغ siphash24_compress_iovec(ثابت بنية iovec *iov, بنية siphash *state);

uint64_t siphash24_finalize(بنية siphash *state);

uint64_t siphash24(ثابت فراغ *in, size_t inlen, ثابت uint8_t k[ساكن 16]);

uint64_t siphash24_uint64(uint64_t in, ثابت uint8_t k[ساكن 16]);

uint64_t siphash24_string(ثابت حرف *s, ثابت uint8_t k[ساكن 16]);
