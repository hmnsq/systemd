/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
શૂન્ય hmac_sha256(અચલ શૂન્ય *ચાવી, size_t ચાવી_size, અચલ શૂન્ય *આગત, size_t આગત_size, uint8_t res[સ્થિર SHA256_DIGEST_SIZE]);
