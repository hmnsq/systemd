/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
خالی hmac_sha256(ثابت خالی *کلید, size_t کلید_سائز, ثابت خالی *داخلہ, size_t داخلہ_سائز, uint8_t res[ساکن SHA256_DIGEST_سائز]);
