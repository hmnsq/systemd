/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <string.h>

#அந்தர்பாவ "hmac.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "sha256.h"

#define HMAC_BLOCK_SIZE 64
#define INNER_PADDING_BYTE 0x36
#define OUTER_PADDING_BYTE 0x5c

வெறுமை hmac_sha256(மாறிலி வெறுமை *சாவி,
                 size_t சாவி_size,
                 மாறிலி வெறுமை *உள்ளீடு,
                 size_t உள்ளீடு_size,
                 uint8_t res[நிலையான SHA256_DIGEST_SIZE]) {

        uint8_t inner_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_சாவி[SHA256_DIGEST_SIZE];
        CLEANUP_ERASE(replacement_சாவி); /* SHA-256 of the key when key_size > block size */
        கட்டமைப்பு sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        ப்ரதிஜ்ஞா(சாவி);
        ப்ரதிஜ்ஞா(சாவி_size > 0);
        ப்ரதிஜ்ஞா(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        எனில் (சாவி_size > HMAC_BLOCK_SIZE) {
                sha256_direct(சாவி, சாவி_size, replacement_சாவி);
                சாவி = replacement_சாவி;
                சாவி_size = SHA256_DIGEST_SIZE;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        memcpy(inner_padding, சாவி, சாவி_size);
        memcpy(outer_padding, சாவி, சாவி_size);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        க்கு (size_t i = 0; i < HMAC_BLOCK_SIZE; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(உள்ளீடு, உள்ளீடு_size, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_SIZE, &hash);
        sha256_finish_ctx(&hash, res);
}
