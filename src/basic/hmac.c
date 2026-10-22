/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>

#ضمن "hmac.h"
#ضمن "memory-util.h"
#ضمن "sha256.h"

#تعریف_کرو HMAC_بلاک_ناپ 64
#تعریف_کرو INNER_PADDING_BYTE 0x36
#تعریف_کرو OUTER_PADDING_BYTE 0x5c

فراغ hmac_sha256(ثابت فراغ *کلید,
                 size_t کلید_ناپ,
                 ثابت فراغ *داخلہ,
                 size_t داخلہ_ناپ,
                 uint8_t res[ساكن SHA256_DIGEST_ناپ]) {

        uint8_t inner_padding[HMAC_بلاک_ناپ] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_بلاک_ناپ] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_کلید[SHA256_DIGEST_ناپ];
        CLEANUP_ERASE(replacement_کلید); /* SHA-256 of the key when key_size > block size */
        بنية sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        أكد(کلید);
        أكد(کلید_ناپ > 0);
        أكد(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        إذا (کلید_ناپ > HMAC_بلاک_ناپ) {
                sha256_سدھا(کلید, کلید_ناپ, replacement_کلید);
                کلید = replacement_کلید;
                کلید_ناپ = SHA256_DIGEST_ناپ;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        حافظہ_نقل(inner_padding, کلید, کلید_ناپ);
        حافظہ_نقل(outer_padding, کلید, کلید_ناپ);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        لكل (size_t i = 0; i < HMAC_بلاک_ناپ; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_بائٹاں(inner_padding, HMAC_بلاک_ناپ, &hash);
        sha256_process_بائٹاں(داخلہ, داخلہ_ناپ, &hash);
        sha256_انت_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_بائٹاں(outer_padding, HMAC_بلاک_ناپ, &hash);
        sha256_process_بائٹاں(res, SHA256_DIGEST_ناپ, &hash);
        sha256_انت_ctx(&hash, res);
}
