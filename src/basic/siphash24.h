/* SPDX-License-Identifier: CC0-1.0 */

#आशय once

#समावेश "forward.h"

काष्ठा siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        माप_प्रकार inlen;
};

व्योम siphash24_init(काष्ठा siphash *state, स्थिर uint8_t k[अटल 16]);
व्योम siphash24_compress(स्थिर व्योम *in, माप_प्रकार inlen, काष्ठा siphash *state);
#घोषणा siphash24_compress_byte(byte, state) siphash24_compress((स्थिर uint8_t[]) { (byte) }, 1, (state))
#घोषणा siphash24_compress_typesafe(in, state)                  \
        siphash24_compress(&(in), माप(typeof(in)), (state))

अटल अंतरभूत व्योम siphash24_compress_boolean(bool in, काष्ठा siphash *state) {
        siphash24_compress_byte(in, state);
}

अटल अंतरभूत व्योम siphash24_compress_usec_t(usec_t in, काष्ठा siphash *state) {
        uint64_t u = htole64(in);
        siphash24_compress_typesafe(u, state);
}

अटल अंतरभूत व्योम siphash24_compress_safe(स्थिर व्योम *in, माप_प्रकार inlen, काष्ठा siphash *state) {
        अगर (inlen == 0)
                वापस;

        siphash24_compress(in, inlen, state);
}

व्योम siphash24_compress_string(स्थिर अक्षर *in, काष्ठा siphash *state);
व्योम siphash24_compress_iovec(स्थिर काष्ठा iovec *iov, काष्ठा siphash *state);

uint64_t siphash24_finalize(काष्ठा siphash *state);

uint64_t siphash24(स्थिर व्योम *in, माप_प्रकार inlen, स्थिर uint8_t k[अटल 16]);

uint64_t siphash24_uint64(uint64_t in, स्थिर uint8_t k[अटल 16]);

uint64_t siphash24_string(स्थिर अक्षर *s, स्थिर uint8_t k[अटल 16]);
