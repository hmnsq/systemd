/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "hmac.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sha256.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMAC_ਖੰਡ_ਮਾਪ 64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INNER_PADDING_BYTE 0x36
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OUTER_PADDING_BYTE 0x5c

ਖਾਲੀ hmac_sha256(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ,
                 size_t ਕੁੰਜੀ_ਮਾਪ,
                 ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼,
                 size_t ਨਿਵੇਸ਼_ਮਾਪ,
                 uint8_t res[ਸਥਿਰ SHA256_DIGEST_ਮਾਪ]) {

        uint8_t inner_padding[HMAC_ਖੰਡ_ਮਾਪ] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_ਖੰਡ_ਮਾਪ] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_ਕੁੰਜੀ[SHA256_DIGEST_ਮਾਪ];
        CLEANUP_ERASE(replacement_ਕੁੰਜੀ); /* SHA-256 of the key when key_size > block size */
        ਬਣਤਰ sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        ਜੇ (ਕੁੰਜੀ_ਮਾਪ > HMAC_ਖੰਡ_ਮਾਪ) {
                sha256_ਸਿੱਧਾ(ਕੁੰਜੀ, ਕੁੰਜੀ_ਮਾਪ, replacement_ਕੁੰਜੀ);
                ਕੁੰਜੀ = replacement_ਕੁੰਜੀ;
                ਕੁੰਜੀ_ਮਾਪ = SHA256_DIGEST_ਮਾਪ;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(inner_padding, ਕੁੰਜੀ, ਕੁੰਜੀ_ਮਾਪ);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(outer_padding, ਕੁੰਜੀ, ਕੁੰਜੀ_ਮਾਪ);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        ਲਈ (size_t i = 0; i < HMAC_ਖੰਡ_ਮਾਪ; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_ਬਾਈਟਾਂ(inner_padding, HMAC_ਖੰਡ_ਮਾਪ, &hash);
        sha256_process_ਬਾਈਟਾਂ(ਨਿਵੇਸ਼, ਨਿਵੇਸ਼_ਮਾਪ, &hash);
        sha256_ਅੰਤ_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_ਬਾਈਟਾਂ(outer_padding, HMAC_ਖੰਡ_ਮਾਪ, &hash);
        sha256_process_ਬਾਈਟਾਂ(res, SHA256_DIGEST_ਮਾਪ, &hash);
        sha256_ਅੰਤ_ctx(&hash, res);
}
