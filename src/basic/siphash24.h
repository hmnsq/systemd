/* SPDX-License-Identifier: CC0-1.0 */

#ہدایت once

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
فراغ siphash24_compress(ثابت فراغ *وچ, size_t inlen, بنية siphash *state);
#تعریف_کرو siphash24_compress_byte(byte, state) siphash24_compress((ثابت uint8_t[]) { (byte) }, 1, (state))
#تعریف_کرو siphash24_compress_typesafe(وچ, state)                  \
        siphash24_compress(&(وچ), حجم(typeof(وچ)), (state))

ساكن مضمن فراغ siphash24_compress_boolean(bool وچ, بنية siphash *state) {
        siphash24_compress_byte(وچ, state);
}

ساكن مضمن فراغ siphash24_compress_usec_t(usec_t وچ, بنية siphash *state) {
        uint64_t u = htole64(وچ);
        siphash24_compress_typesafe(u, state);
}

ساكن مضمن فراغ siphash24_compress_محفوظ(ثابت فراغ *وچ, size_t inlen, بنية siphash *state) {
        إذا (inlen == 0)
                أرجع;

        siphash24_compress(وچ, inlen, state);
}

فراغ siphash24_compress_string(ثابت حرف *وچ, بنية siphash *state);
فراغ siphash24_compress_iovec(ثابت بنية iovec *iov, بنية siphash *state);

uint64_t siphash24_finalize(بنية siphash *state);

uint64_t siphash24(ثابت فراغ *وچ, size_t inlen, ثابت uint8_t k[ساكن 16]);

uint64_t siphash24_uint64(uint64_t وچ, ثابت uint8_t k[ساكن 16]);

uint64_t siphash24_string(ثابت حرف *s, ثابت uint8_t k[ساكن 16]);
