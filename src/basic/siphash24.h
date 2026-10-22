/* SPDX-License-Identifier: CC0-1.0 */

#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସଂରଚନା siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

ଶୂନ୍ୟ siphash24_init(ସଂରଚନା siphash *state, ଅଚଳ uint8_t k[ସ୍ଥିର 16]);
ଶୂନ୍ୟ siphash24_compress(ଅଚଳ ଶୂନ୍ୟ *in, size_t inlen, ସଂରଚନା siphash *state);
#define siphash24_compress_byte(byte, state) siphash24_compress((ଅଚଳ uint8_t[]) { (byte) }, 1, (state))
#define siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), ଆକାର(typeof(in)), (state))

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ siphash24_compress_boolean(bool in, ସଂରଚନା siphash *state) {
        siphash24_compress_byte(in, state);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ siphash24_compress_usec_t(usec_t in, ସଂରଚନା siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ siphash24_compress_safe(ଅଚଳ ଶୂନ୍ୟ *in, size_t inlen, ସଂରଚନା siphash *state) {
        ଯଦି (inlen == 0)
                ଫେରନ୍ତୁ;

        siphash24_compress(in, inlen, state);
}

ଶୂନ୍ୟ siphash24_compress_string(ଅଚଳ ଅକ୍ଷର *in, ସଂରଚନା siphash *state);
ଶୂନ୍ୟ siphash24_compress_iovec(ଅଚଳ ସଂରଚନା iovec *iov, ସଂରଚନା siphash *state);

uint64_t siphash24_finalize(ସଂରଚନା siphash *state);

uint64_t siphash24(ଅଚଳ ଶୂନ୍ୟ *in, size_t inlen, ଅଚଳ uint8_t k[ସ୍ଥିର 16]);

uint64_t siphash24_uint64(uint64_t in, ଅଚଳ uint8_t k[ସ୍ଥିର 16]);

uint64_t siphash24_string(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ uint8_t k[ସ୍ଥିର 16]);
