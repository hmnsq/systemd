/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <string.h>

#अन्तर्भाव "hmac.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "sha256.h"

#परिभाषय HMAC_खण्डम्_आकार 64
#परिभाषय INNER_PADDING_BYTE 0x36
#परिभाषय OUTER_PADDING_BYTE 0x5c

शून्य hmac_sha256(ध्रुव शून्य *कुञ्चिका,
                 size_t कुञ्चिका_आकार,
                 ध्रुव शून्य *निवेशः,
                 size_t निवेशः_आकार,
                 uint8_t res[स्थिर SHA256_DIGEST_आकार]) {

        uint8_t inner_padding[HMAC_खण्डम्_आकार] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_खण्डम्_आकार] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_कुञ्चिका[SHA256_DIGEST_आकार];
        CLEANUP_ERASE(replacement_कुञ्चिका); /* SHA-256 of the key when key_size > block size */
        संरचना sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        प्रतिज्ञा(कुञ्चिका);
        प्रतिज्ञा(कुञ्चिका_आकार > 0);
        प्रतिज्ञा(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        यदि (कुञ्चिका_आकार > HMAC_खण्डम्_आकार) {
                sha256_प्रत्यक्षम्(कुञ्चिका, कुञ्चिका_आकार, replacement_कुञ्चिका);
                कुञ्चिका = replacement_कुञ्चिका;
                कुञ्चिका_आकार = SHA256_DIGEST_आकार;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        स्मृतिं_प्रतिलिख(inner_padding, कुञ्चिका, कुञ्चिका_आकार);
        स्मृतिं_प्रतिलिख(outer_padding, कुञ्चिका, कुञ्चिका_आकार);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        प्रति (size_t i = 0; i < HMAC_खण्डम्_आकार; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_अष्टकानि(inner_padding, HMAC_खण्डम्_आकार, &hash);
        sha256_process_अष्टकानि(निवेशः, निवेशः_आकार, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_अष्टकानि(outer_padding, HMAC_खण्डम्_आकार, &hash);
        sha256_process_अष्टकानि(res, SHA256_DIGEST_आकार, &hash);
        sha256_finish_ctx(&hash, res);
}
