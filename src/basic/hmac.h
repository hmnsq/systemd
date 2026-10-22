/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"
#ضمن "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
فراغ hmac_sha256(ثابت فراغ *key, size_t key_size, ثابت فراغ *input, size_t input_size, uint8_t res[ساكن SHA256_DIGEST_SIZE]);
