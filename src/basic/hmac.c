/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <string.h>

#شامل "hmac.h"
#شامل "memory-util.h"
#شامل "sha256.h"

#تعریف_کرو HMAC_بلاک_سائز 64
#تعریف_کرو INNER_PADDING_BYTE 0x36
#تعریف_کرو OUTER_PADDING_BYTE 0x5c

خالی hmac_sha256(ثابت خالی *کلید,
                 size_t کلید_سائز,
                 ثابت خالی *داخلہ,
                 size_t داخلہ_سائز,
                 uint8_t res[ساکن SHA256_DIGEST_سائز]) {

        uint8_t inner_padding[HMAC_بلاک_سائز] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_بلاک_سائز] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_کلید[SHA256_DIGEST_سائز];
        CLEANUP_ERASE(replacement_کلید); /* SHA-256 of the key when key_size > block size */
        ساخت sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        أكد(کلید);
        أكد(کلید_سائز > 0);
        أكد(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        اگر (کلید_سائز > HMAC_بلاک_سائز) {
                sha256_براہ_راست(کلید, کلید_سائز, replacement_کلید);
                کلید = replacement_کلید;
                کلید_سائز = SHA256_DIGEST_سائز;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        حافظہ_نقل(inner_padding, کلید, کلید_سائز);
        حافظہ_نقل(outer_padding, کلید, کلید_سائز);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        برائے (size_t i = 0; i < HMAC_بلاک_سائز; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_بائٹیں(inner_padding, HMAC_بلاک_سائز, &hash);
        sha256_process_بائٹیں(داخلہ, داخلہ_سائز, &hash);
        sha256_اختتام_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_بائٹیں(outer_padding, HMAC_بلاک_سائز, &hash);
        sha256_process_بائٹیں(res, SHA256_DIGEST_سائز, &hash);
        sha256_اختتام_ctx(&hash, res);
}
