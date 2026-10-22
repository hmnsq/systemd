/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
வெறுமை hmac_sha256(மாறிலி வெறுமை *சாவி, size_t சாவி_size, மாறிலி வெறுமை *உள்ளீடு, size_t உள்ளீடு_size, uint8_t res[நிலையான SHA256_DIGEST_SIZE]);
