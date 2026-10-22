/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
ಶೂನ್ಯ hmac_sha256(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_size, ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು, size_t ಒಳಹರಿವು_size, uint8_t res[ಸ್ಥಿರ SHA256_DIGEST_SIZE]);
