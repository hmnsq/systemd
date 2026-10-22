/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
व्योम hmac_sha256(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_आकार, स्थिर व्योम *निवेश, माप_प्रकार निवेश_आकार, uint8_t res[अटल SHA256_DIGEST_आकार]);
