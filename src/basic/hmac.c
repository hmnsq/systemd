/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>

#ضمن "hmac.h"
#ضمن "memory-util.h"
#ضمن "sha256.h"

#وصف_ڪريو HMAC_بلاڪ_سائيز 64
#وصف_ڪريو INNER_PADDING_BYTE 0x36
#وصف_ڪريو OUTER_PADDING_BYTE 0x5c

فراغ hmac_sha256(ثابت فراغ *ڪنجي,
                 size_t ڪنجي_سائيز,
                 ثابت فراغ *داخلا_2,
                 size_t داخلا_سائيز_2,
                 uint8_t res[ساكن SHA256_DIGEST_سائيز]) {

        uint8_t inner_padding[HMAC_بلاڪ_سائيز] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_بلاڪ_سائيز] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_ڪنجي[SHA256_DIGEST_سائيز];
        CLEANUP_ERASE(replacement_ڪنجي); /* SHA-256 of the key when key_size > block size */
        بنية sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        أكد(ڪنجي);
        أكد(ڪنجي_سائيز > 0);
        أكد(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        إذا (ڪنجي_سائيز > HMAC_بلاڪ_سائيز) {
                sha256_سڌو(ڪنجي, ڪنجي_سائيز, replacement_ڪنجي);
                ڪنجي = replacement_ڪنجي;
                ڪنجي_سائيز = SHA256_DIGEST_سائيز;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        يادگيري_نقل(inner_padding, ڪنجي, ڪنجي_سائيز);
        يادگيري_نقل(outer_padding, ڪنجي, ڪنجي_سائيز);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        لكل (size_t i = 0; i < HMAC_بلاڪ_سائيز; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_بائيٽون(inner_padding, HMAC_بلاڪ_سائيز, &hash);
        sha256_process_بائيٽون(داخلا_2, داخلا_سائيز_2, &hash);
        sha256_پڄاڻي_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_بائيٽون(outer_padding, HMAC_بلاڪ_سائيز, &hash);
        sha256_process_بائيٽون(res, SHA256_DIGEST_سائيز, &hash);
        sha256_پڄاڻي_ctx(&hash, res);
}
