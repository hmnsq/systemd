/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"
#ضمن "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
فراغ hmac_sha256(ثابت فراغ *کلید, size_t کلید_اندازه, ثابت فراغ *درون_داد, size_t درون_داد_اندازه, uint8_t res[ساكن SHA256_DIGEST_اندازه]);
