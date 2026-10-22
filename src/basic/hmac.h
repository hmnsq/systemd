/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"
#ضمن "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
فراغ hmac_sha256(ثابت فراغ *ڪنجي, size_t ڪنجي_سائيز, ثابت فراغ *داخلا_2, size_t داخلا_سائيز_2, uint8_t res[ساكن SHA256_DIGEST_سائيز]);
