/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
శూన్య hmac_sha256(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_కొలత, స్థిరరాశి శూన్య *ఆదానం, size_t ఆదానం_కొలత, uint8_t res[స్థిర SHA256_DIGEST_కొలత]);
