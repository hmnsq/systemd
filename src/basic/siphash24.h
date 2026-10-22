/* SPDX-License-Identifier: CC0-1.0 */

#ہدایت once

#شامل "forward.h"

ساخت siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

خالی siphash24_init(ساخت siphash *state, ثابت uint8_t k[ساکن 16]);
خالی siphash24_compress(ثابت خالی *اندر, size_t inlen, ساخت siphash *state);
#تعریف_کرو siphash24_compress_byte(byte, state) siphash24_compress((ثابت uint8_t[]) { (byte) }, 1, (state))
#تعریف_کرو siphash24_compress_typesafe(اندر, state)                  \
        siphash24_compress(&(اندر), حجم(typeof(اندر)), (state))

ساکن اندرونی خالی siphash24_compress_boolean(bool اندر, ساخت siphash *state) {
        siphash24_compress_byte(اندر, state);
}

ساکن اندرونی خالی siphash24_compress_usec_t(usec_t اندر, ساخت siphash *state) {
        uint64_t u = htole64(اندر);
        siphash24_compress_typesafe(u, state);
}

ساکن اندرونی خالی siphash24_compress_محفوظ(ثابت خالی *اندر, size_t inlen, ساخت siphash *state) {
        اگر (inlen == 0)
                واپس;

        siphash24_compress(اندر, inlen, state);
}

خالی siphash24_compress_string(ثابت حرف *اندر, ساخت siphash *state);
خالی siphash24_compress_iovec(ثابت ساخت iovec *iov, ساخت siphash *state);

uint64_t siphash24_finalize(ساخت siphash *state);

uint64_t siphash24(ثابت خالی *اندر, size_t inlen, ثابت uint8_t k[ساکن 16]);

uint64_t siphash24_uint64(uint64_t اندر, ثابت uint8_t k[ساکن 16]);

uint64_t siphash24_string(ثابت حرف *s, ثابت uint8_t k[ساکن 16]);
