/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <string.h>

#అన్తర్భావ "hmac.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "sha256.h"

#నిర్వచించు HMAC_ఖండం_కొలత 64
#నిర్వచించు INNER_PADDING_BYTE 0x36
#నిర్వచించు OUTER_PADDING_BYTE 0x5c

శూన్య hmac_sha256(స్థిరరాశి శూన్య *కీలకం,
                 size_t కీలకం_కొలత,
                 స్థిరరాశి శూన్య *ఆదానం,
                 size_t ఆదానం_కొలత,
                 uint8_t res[స్థిర SHA256_DIGEST_కొలత]) {

        uint8_t inner_padding[HMAC_ఖండం_కొలత] = { };
        CLEANUP_ERASE(inner_padding); /* carries key ^ 0x36, trivially reversible to the original key */
        uint8_t outer_padding[HMAC_ఖండం_కొలత] = { };
        CLEANUP_ERASE(outer_padding); /* carries key ^ 0x5c, trivially reversible to the original key */
        uint8_t replacement_కీలకం[SHA256_DIGEST_కొలత];
        CLEANUP_ERASE(replacement_కీలకం); /* SHA-256 of the key when key_size > block size */
        నిర్మాణం sha256_ctx hash;
        CLEANUP_ERASE(hash); /* intermediate state derived from key material */

        ప్రతిజ్ఞా(కీలకం);
        ప్రతిజ్ఞా(కీలకం_కొలత > 0);
        ప్రతిజ్ఞా(res);

        /* Implement algorithm as described by FIPS 198. */

        /* The key needs to be block size length or less, hash it if it's longer. */
        ఒకవేళ (కీలకం_కొలత > HMAC_ఖండం_కొలత) {
                sha256_ప్రత్యక్ష(కీలకం, కీలకం_కొలత, replacement_కీలకం);
                కీలకం = replacement_కీలకం;
                కీలకం_కొలత = SHA256_DIGEST_కొలత;
        }

        /* First, copy the key into the padding arrays. If it's shorter than
         * the block size, the arrays are already initialized to 0. */
        జ్ఞాపకం_నకలు(inner_padding, కీలకం, కీలకం_కొలత);
        జ్ఞాపకం_నకలు(outer_padding, కీలకం, కీలకం_కొలత);

        /* Then, XOR the provided key and any padding leftovers with the fixed
         * padding bytes as defined in FIPS 198. */
        కొరకు (size_t i = 0; i < HMAC_ఖండం_కొలత; i++) {
                inner_padding[i] ^= INNER_PADDING_BYTE;
                outer_padding[i] ^= OUTER_PADDING_BYTE;
        }

        /* First pass: hash the inner padding array and the input. */
        sha256_init_ctx(&hash);
        sha256_process_బైట్లు(inner_padding, HMAC_ఖండం_కొలత, &hash);
        sha256_process_బైట్లు(ఆదానం, ఆదానం_కొలత, &hash);
        sha256_ముగింపు_ctx(&hash, res);

        /* Second pass: hash the outer padding array and the result of the first pass. */
        sha256_init_ctx(&hash);
        sha256_process_బైట్లు(outer_padding, HMAC_ఖండం_కొలత, &hash);
        sha256_process_బైట్లు(res, SHA256_DIGEST_కొలత, &hash);
        sha256_ముగింపు_ctx(&hash, res);
}
