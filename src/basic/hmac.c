/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "hmac.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "sha256.h"

#define HMAC_BLOCK_SIZE 64
#define INNER_PADDING_BYTE 0x36
#define OUTER_PADDING_BYTE 0x5c

ശൂന്യം hmac_sha256(സ്ഥിരം ശൂന്യം *താക്കോൽ,
                 size_t താക്കോൽ_size,
                 സ്ഥിരം ശൂന്യം *നിവേശം,
                 size_t നിവേശം_size,
                 uint8_t res[സ്ഥിതം SHA256_DIGEST_SIZE]) {

        uint8_t inner_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_BLOCK_SIZE] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_താക്കോൽ[SHA256_DIGEST_SIZE];
        CLEANUP_ERASE(replacement_താക്കോൽ); /* SHA-256 of the key when key_size > block size */
        ഘടന sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        പ്രതിജ്ഞാ(താക്കോൽ);
        പ്രതിജ്ഞാ(താക്കോൽ_size > 0);
        പ്രതിജ്ഞാ(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        എങ്കിൽ (താക്കോൽ_size > HMAC_BLOCK_SIZE) {
                sha256_direct(താക്കോൽ, താക്കോൽ_size, replacement_താക്കോൽ);
                താക്കോൽ = replacement_താക്കോൽ;
                താക്കോൽ_size = SHA256_DIGEST_SIZE;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        memcpy(inner_padding, താക്കോൽ, താക്കോൽ_size);
        memcpy(outer_padding, താക്കോൽ, താക്കോൽ_size);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        വേണ്ടി (size_t i = 0; i < HMAC_BLOCK_SIZE; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(inner_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(നിവേശം, നിവേശം_size, &hash);
        sha256_finish_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_bytes(outer_padding, HMAC_BLOCK_SIZE, &hash);
        sha256_process_bytes(res, SHA256_DIGEST_SIZE, &hash);
        sha256_finish_ctx(&hash, res);
}
