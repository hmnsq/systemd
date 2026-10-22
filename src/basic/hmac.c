/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>

#ضمن "hmac.h"
#ضمن "memory-util.h"
#ضمن "sha256.h"

#تعریف_کړه HMAC_بلاک_اندازه 64
#تعریف_کړه INNER_PADDING_BYTE 0x36
#تعریف_کړه OUTER_PADDING_BYTE 0x5c

فراغ hmac_sha256(ثابت فراغ *کلی,
                 size_t کلی_اندازه,
                 ثابت فراغ *ننوت,
                 size_t ننوت_اندازه,
                 uint8_t res[ساكن SHA256_DIGEST_اندازه]) {

        uint8_t inner_padding[HMAC_بلاک_اندازه] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_بلاک_اندازه] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_کلی[SHA256_DIGEST_اندازه];
        CLEANUP_ERASE(replacement_کلی); /* SHA-256 of the key when key_size > block size */
        بنية sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        أكد(کلی);
        أكد(کلی_اندازه > 0);
        أكد(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        إذا (کلی_اندازه > HMAC_بلاک_اندازه) {
                sha256_مستقیم(کلی, کلی_اندازه, replacement_کلی);
                کلی = replacement_کلی;
                کلی_اندازه = SHA256_DIGEST_اندازه;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        حافظه_نقل(inner_padding, کلی, کلی_اندازه);
        حافظه_نقل(outer_padding, کلی, کلی_اندازه);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        لكل (size_t i = 0; i < HMAC_بلاک_اندازه; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_بایټونه(inner_padding, HMAC_بلاک_اندازه, &hash);
        sha256_process_بایټونه(ننوت, ننوت_اندازه, &hash);
        sha256_پای_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_بایټونه(outer_padding, HMAC_بلاک_اندازه, &hash);
        sha256_process_بایټونه(res, SHA256_DIGEST_اندازه, &hash);
        sha256_پای_ctx(&hash, res);
}
