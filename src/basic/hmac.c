/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "hmac.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "sha256.h"

#define HMAC_BLOCK_SIZE 64
#define INNER_PADDING_BYTE 0x36
#define OUTER_PADDING_BYTE 0x5c

શૂન્ય hmac_sha256(અચલ શૂન્ય *ચાવી,
                 size_t ચાવી_size,
                 અચલ શૂન્ય *આગત,
                 size_t આગત_size,
                 uint8_t res[સ્થિર SHA256_DIGEST_SIZE]) {

        uint8_t inner_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_ચાવી[SHA256_DIGEST_SIZE];
        CLEANUP_ERASE(replacement_ચાવી); /* SHA-256 of the key when key_size > block size */
        સંરચના sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        પ્રતિજ્ઞા(ચાવી);
        પ્રતિજ્ઞા(ચાવી_size > 0);
        પ્રતિજ્ઞા(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        જો (ચાવી_size > HMAC_BLOCK_SIZE) {
                sha256_direct(ચાવી, ચાવી_size, replacement_ચાવી);
                ચાવી = replacement_ચાવી;
                ચાવી_size = SHA256_DIGEST_SIZE;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        memcpy(inner_padding, ચાવી, ચાવી_size);
        memcpy(outer_padding, ચાવી, ચાવી_size);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        માટે (size_t i = 0; i < HMAC_BLOCK_SIZE; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(આગત, આગત_size, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_SIZE, &hash);
        sha256_finish_ctx(&hash, res);
}
