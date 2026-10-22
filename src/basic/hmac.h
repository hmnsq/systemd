/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
ଶୂନ୍ୟ hmac_sha256(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_size, ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ, size_t ନିବେଶ_size, uint8_t res[ସ୍ଥିର SHA256_DIGEST_SIZE]);
