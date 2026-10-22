/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "hmac.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "sha256.h"

#define HMAC_BLOCK_SIZE 64
#define INNER_PADDING_BYTE 0x36
#define OUTER_PADDING_BYTE 0x5c

ಶೂನ್ಯ hmac_sha256(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ,
                 size_t ಕೀಲಿ_size,
                 ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು,
                 size_t ಒಳಹರಿವು_size,
                 uint8_t res[ಸ್ಥಿರ SHA256_DIGEST_SIZE]) {

        uint8_t inner_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_ಕೀಲಿ[SHA256_DIGEST_SIZE];
        CLEANUP_ERASE(replacement_ಕೀಲಿ); /* SHA-256 of the key when key_size > block size */
        ಸಂರಚನಾ sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ_size > 0);
        ಪ್ರತಿಜ್ಞಾ(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        ಯದಿ (ಕೀಲಿ_size > HMAC_BLOCK_SIZE) {
                sha256_direct(ಕೀಲಿ, ಕೀಲಿ_size, replacement_ಕೀಲಿ);
                ಕೀಲಿ = replacement_ಕೀಲಿ;
                ಕೀಲಿ_size = SHA256_DIGEST_SIZE;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        memcpy(inner_padding, ಕೀಲಿ, ಕೀಲಿ_size);
        memcpy(outer_padding, ಕೀಲಿ, ಕೀಲಿ_size);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        ಪ್ರತಿ (size_t i = 0; i < HMAC_BLOCK_SIZE; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(ಒಳಹರಿವು, ಒಳಹರಿವು_size, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_SIZE, &hash);
        sha256_finish_ctx(&hash, res);
}
