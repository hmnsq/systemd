/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#અન્તર્ભાવ "forward.h"

સંરચના siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

શૂન્ય siphash24_init(સંરચના siphash *state, અચલ uint8_t k[સ્થિર 16]);
શૂન્ય siphash24_compress(અચલ શૂન્ય *in, size_t inlen, સંરચના siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((અચલ uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), કદ(typeof(in)), (state))

સ્થિર અંતઃરેખ શૂન્ય siphash24_compress_boolean(bool in, સંરચના siphash *state) {
        siphash24_compress_byte(in, state);
}

સ્થિર અંતઃરેખ શૂન્ય siphash24_compress_usec_t(usec_t in, સંરચના siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

સ્થિર અંતઃરેખ શૂન્ય siphash24_compress_safe(અચલ શૂન્ય *in, size_t inlen, સંરચના siphash *state) {
        જો (inlen == 0)
                પાછા;

        siphash24_compress(in, inlen, state);
}

શૂન્ય siphash24_compress_string(અચલ અક્ષર *in, સંરચના siphash *state);
શૂન્ય siphash24_compress_iovec(અચલ સંરચના iovec *iov, સંરચના siphash *state);

uint64_t siphash24_finalize(સંરચના siphash *state);

uint64_t siphash24(અચલ શૂન્ય *in, size_t inlen, અચલ uint8_t k[સ્થિર 16]);

uint64_t siphash24_uint64(uint64_t in, અચલ uint8_t k[સ્થિર 16]);

uint64_t siphash24_string(અચલ અક્ષર *s, અચલ uint8_t k[સ્થિર 16]);
