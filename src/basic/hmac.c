/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>

#ضمن "hmac.h"
#ضمن "memory-util.h"
#ضمن "sha256.h"

#تعریف_کن HMAC_بلوک_اندازه 64
#تعریف_کن INNER_PADDING_BYTE 0x36
#تعریف_کن OUTER_PADDING_BYTE 0x5c

فراغ hmac_sha256(ثابت فراغ *کلید,
                 size_t کلید_اندازه,
                 ثابت فراغ *درون_داد,
                 size_t درون_داد_اندازه,
                 uint8_t res[ساكن SHA256_DIGEST_اندازه]) {

        uint8_t inner_padding[HMAC_بلوک_اندازه] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_بلوک_اندازه] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_کلید[SHA256_DIGEST_اندازه];
        CLEANUP_ERASE(replacement_کلید); /* SHA-256 of the key when key_size > block size */
        بنية sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        أكد(کلید);
        أكد(کلید_اندازه > 0);
        أكد(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        إذا (کلید_اندازه > HMAC_بلوک_اندازه) {
                sha256_مستقیم(کلید, کلید_اندازه, replacement_کلید);
                کلید = replacement_کلید;
                کلید_اندازه = SHA256_DIGEST_اندازه;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        کپی_حافظه(inner_padding, کلید, کلید_اندازه);
        کپی_حافظه(outer_padding, کلید, کلید_اندازه);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        لكل (size_t i = 0; i < HMAC_بلوک_اندازه; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_بایتها(inner_padding, HMAC_بلوک_اندازه, &hash);
        sha256_process_بایتها(درون_داد, درون_داد_اندازه, &hash);
        sha256_پایان_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_بایتها(outer_padding, HMAC_بلوک_اندازه, &hash);
        sha256_process_بایتها(res, SHA256_DIGEST_اندازه, &hash);
        sha256_پایان_ctx(&hash, res);
}
