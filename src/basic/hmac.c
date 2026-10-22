/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <string.h>

#अन्तर्भाव "hmac.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "sha256.h"

#define HMAC_BLOCK_SIZE 64
#define INNER_PADDING_BYTE 0x36
#define OUTER_PADDING_BYTE 0x5c

शून्य hmac_sha256(ध्रुव शून्य *कुञ्चिका,
                 size_t कुञ्चिका_size,
                 ध्रुव शून्य *निवेशः,
                 size_t निवेशः_size,
                 uint8_t res[स्थिर SHA256_DIGEST_SIZE]) {

        uint8_t inner_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_कुञ्चिका[SHA256_DIGEST_SIZE];
        CLEANUP_ERASE(replacement_कुञ्चिका); /* SHA-256 of the key when key_size > block size */
        संरचना sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        प्रतिज्ञा(कुञ्चिका);
        प्रतिज्ञा(कुञ्चिका_size > 0);
        प्रतिज्ञा(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        जइ (कुञ्चिका_size > HMAC_BLOCK_SIZE) {
                sha256_direct(कुञ्चिका, कुञ्चिका_size, replacement_कुञ्चिका);
                कुञ्चिका = replacement_कुञ्चिका;
                कुञ्चिका_size = SHA256_DIGEST_SIZE;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        memcpy(inner_padding, कुञ्चिका, कुञ्चिका_size);
        memcpy(outer_padding, कुञ्चिका, कुञ्चिका_size);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        प्रति (size_t i = 0; i < HMAC_BLOCK_SIZE; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(निवेशः, निवेशः_size, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_SIZE, &hash);
        sha256_finish_ctx(&hash, res);
}
