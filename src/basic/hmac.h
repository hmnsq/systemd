/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
تهی hmac_sha256(ثابت تهی *کلید, size_t کلید_size, ثابت تهی *درون_داد, size_t درون_داد_size, uint8_t res[ایستا SHA256_DIGEST_SIZE]);
