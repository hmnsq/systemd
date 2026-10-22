/* SPDX-License-Identifier: CC0-1.0 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਬਣਤਰ siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

ਖਾਲੀ siphash24_init(ਬਣਤਰ siphash *state, ਅਚਲ uint8_t k[ਸਥਿਰ 16]);
ਖਾਲੀ siphash24_compress(ਅਚਲ ਖਾਲੀ *ਵਿੱਚ, size_t inlen, ਬਣਤਰ siphash *state);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ siphash24_compress_byte(byte, state) siphash24_compress((ਅਚਲ uint8_t[]) { (byte) }, 1, (state))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ siphash24_compress_typesafe(ਵਿੱਚ, state)                  \
        siphash24_compress(&(ਵਿੱਚ), ਆਕਾਰ(typeof(ਵਿੱਚ)), (state))

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ siphash24_compress_boolean(bool ਵਿੱਚ, ਬਣਤਰ siphash *state) {
        siphash24_compress_byte(ਵਿੱਚ, state);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ siphash24_compress_usec_t(usec_t ਵਿੱਚ, ਬਣਤਰ siphash *state) {
        uint64_t u = htole64(ਵਿੱਚ);
        siphash24_compress_typesafe(u, state);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ siphash24_compress_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਖਾਲੀ *ਵਿੱਚ, size_t inlen, ਬਣਤਰ siphash *state) {
        ਜੇ (inlen == 0)
                ਵਾਪਸ;

        siphash24_compress(ਵਿੱਚ, inlen, state);
}

ਖਾਲੀ siphash24_compress_string(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ, ਬਣਤਰ siphash *state);
ਖਾਲੀ siphash24_compress_iovec(ਅਚਲ ਬਣਤਰ iovec *iov, ਬਣਤਰ siphash *state);

uint64_t siphash24_finalize(ਬਣਤਰ siphash *state);

uint64_t siphash24(ਅਚਲ ਖਾਲੀ *ਵਿੱਚ, size_t inlen, ਅਚਲ uint8_t k[ਸਥਿਰ 16]);

uint64_t siphash24_uint64(uint64_t ਵਿੱਚ, ਅਚਲ uint8_t k[ਸਥਿਰ 16]);

uint64_t siphash24_string(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ uint8_t k[ਸਥਿਰ 16]);
