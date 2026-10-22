/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "hmac.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "sha256.h"

#define HMAC_BLOCK_SIZE 64
#define INNER_PADDING_BYTE 0x36
#define OUTER_PADDING_BYTE 0x5c

শূন্য hmac_sha256(ধ্রুব শূন্য *চাবি,
                 size_t চাবি_size,
                 ধ্রুব শূন্য *প্রবেশ,
                 size_t প্রবেশ_size,
                 uint8_t res[স্থির SHA256_DIGEST_SIZE]) {

        uint8_t inner_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_চাবি[SHA256_DIGEST_SIZE];
        CLEANUP_ERASE(replacement_চাবি); /* SHA-256 of the key when key_size > block size */
        সংরচনা sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(চাবি_size > 0);
        প্রতিজ্ঞা(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        যদি (চাবি_size > HMAC_BLOCK_SIZE) {
                sha256_direct(চাবি, চাবি_size, replacement_চাবি);
                চাবি = replacement_চাবি;
                চাবি_size = SHA256_DIGEST_SIZE;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        memcpy(inner_padding, চাবি, চাবি_size);
        memcpy(outer_padding, চাবি, চাবি_size);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        প্রতি (size_t i = 0; i < HMAC_BLOCK_SIZE; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(প্রবেশ, প্রবেশ_size, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_SIZE, &hash);
        sha256_finish_ctx(&hash, res);
}
