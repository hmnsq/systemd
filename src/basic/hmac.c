/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "hmac.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "sha256.h"

#define HMAC_BLOCK_SIZE 64
#define INNER_PADDING_BYTE 0x36
#define OUTER_PADDING_BYTE 0x5c

ଶୂନ୍ୟ hmac_sha256(ଅଚଳ ଶୂନ୍ୟ *ଚାବି,
                 size_t ଚାବି_size,
                 ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ,
                 size_t ନିବେଶ_size,
                 uint8_t res[ସ୍ଥିର SHA256_DIGEST_SIZE]) {

        uint8_t inner_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_ଚାବି[SHA256_DIGEST_SIZE];
        CLEANUP_ERASE(replacement_ଚାବି); /* SHA-256 of the key when key_size > block size */
        ସଂରଚନା sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        ପ୍ରତିଜ୍ଞା(ଚାବି);
        ପ୍ରତିଜ୍ଞା(ଚାବି_size > 0);
        ପ୍ରତିଜ୍ଞା(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        ଯଦି (ଚାବି_size > HMAC_BLOCK_SIZE) {
                sha256_direct(ଚାବି, ଚାବି_size, replacement_ଚାବି);
                ଚାବି = replacement_ଚାବି;
                ଚାବି_size = SHA256_DIGEST_SIZE;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        memcpy(inner_padding, ଚାବି, ଚାବି_size);
        memcpy(outer_padding, ଚାବି, ଚାବି_size);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        ପାଇଁ (size_t i = 0; i < HMAC_BLOCK_SIZE; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(ନିବେଶ, ନିବେଶ_size, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_SIZE, &hash);
        sha256_finish_ctx(&hash, res);
}
