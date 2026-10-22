/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
ਖਾਲੀ hmac_sha256(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_ਮਾਪ, ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼, size_t ਨਿਵੇਸ਼_ਮਾਪ, uint8_t res[ਸਥਿਰ SHA256_DIGEST_ਮਾਪ]);
