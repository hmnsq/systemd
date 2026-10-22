/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <string.h>

#समावेश "hmac.h"
#समावेश "memory-util.h"
#समावेश "sha256.h"

#घोषणा HMAC_BLOCK_आकार 64
#घोषणा INNER_PADDING_BYTE 0x36
#घोषणा OUTER_PADDING_BYTE 0x5c

व्योम hmac_sha256(स्थिर व्योम *कुंजी,
                 माप_प्रकार कुंजी_आकार,
                 स्थिर व्योम *निवेश,
                 माप_प्रकार निवेश_आकार,
                 uint8_t res[अटल SHA256_DIGEST_आकार]) {

        uint8_t inner_padding[HMAC_BLOCK_आकार] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_आकार] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_कुंजी[SHA256_DIGEST_आकार];
        CLEANUP_ERASE(replacement_कुंजी); /* SHA-256 of the key when key_size > block size */
        काष्ठा sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        निश्चित(कुंजी);
        निश्चित(कुंजी_आकार > 0);
        निश्चित(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        अगर (कुंजी_आकार > HMAC_BLOCK_आकार) {
                sha256_direct(कुंजी, कुंजी_आकार, replacement_कुंजी);
                कुंजी = replacement_कुंजी;
                कुंजी_आकार = SHA256_DIGEST_आकार;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        स_नकल(inner_padding, कुंजी, कुंजी_आकार);
        स_नकल(outer_padding, कुंजी, कुंजी_आकार);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        क्रम (माप_प्रकार i = 0; i < HMAC_BLOCK_आकार; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_आकार, &hash);
        sha256_process_bytes(निवेश, निवेश_आकार, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_आकार, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_आकार, &hash);
        sha256_finish_ctx(&hash, res);
}
