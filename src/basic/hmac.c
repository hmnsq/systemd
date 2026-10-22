/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "hmac.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "sha256.h"

#ವ್ಯಾಖ್ಯಾನಿಸು HMAC_ಖಂಡ_ಗಾತ್ರ 64
#ವ್ಯಾಖ್ಯಾನಿಸು INNER_PADDING_BYTE 0x36
#ವ್ಯಾಖ್ಯಾನಿಸು OUTER_PADDING_BYTE 0x5c

ಶೂನ್ಯ hmac_sha256(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ,
                 size_t ಕೀಲಿ_ಗಾತ್ರ,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳಹರಿವು,
                 size_t ಒಳಹರಿವು_ಗಾತ್ರ,
                 uint8_t res[ಸ್ಥಿರ SHA256_DIGEST_ಗಾತ್ರ]) {

        uint8_t inner_padding[HMAC_ಖಂಡ_ಗಾತ್ರ] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_ಖಂಡ_ಗಾತ್ರ] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_ಕೀಲಿ[SHA256_DIGEST_ಗಾತ್ರ];
        CLEANUP_ERASE(replacement_ಕೀಲಿ); /* SHA-256 of the key when key_size > block size */
        ರಚನೆ sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        ಆದರೆ (ಕೀಲಿ_ಗಾತ್ರ > HMAC_ಖಂಡ_ಗಾತ್ರ) {
                sha256_ನೇರ(ಕೀಲಿ, ಕೀಲಿ_ಗಾತ್ರ, replacement_ಕೀಲಿ);
                ಕೀಲಿ = replacement_ಕೀಲಿ;
                ಕೀಲಿ_ಗಾತ್ರ = SHA256_DIGEST_ಗಾತ್ರ;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        ಸ್ಮರಣೆ_ನಕಲು(inner_padding, ಕೀಲಿ, ಕೀಲಿ_ಗಾತ್ರ);
        ಸ್ಮರಣೆ_ನಕಲು(outer_padding, ಕೀಲಿ, ಕೀಲಿ_ಗಾತ್ರ);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        ಗಾಗಿ (size_t i = 0; i < HMAC_ಖಂಡ_ಗಾತ್ರ; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_ಬೈಟುಗಳು(inner_padding, HMAC_ಖಂಡ_ಗಾತ್ರ, &hash);
        sha256_process_ಬೈಟುಗಳು(ಒಳಹರಿವು, ಒಳಹರಿವು_ಗಾತ್ರ, &hash);
        sha256_ಮುಕ್ತಾಯ_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_ಬೈಟುಗಳು(outer_padding, HMAC_ಖಂಡ_ಗಾತ್ರ, &hash);
        sha256_process_ಬೈಟುಗಳು(res, SHA256_DIGEST_ಗಾತ್ರ, &hash);
        sha256_ಮುಕ್ತಾಯ_ctx(&hash, res);
}
