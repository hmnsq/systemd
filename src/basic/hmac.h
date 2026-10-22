/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
शून्य hmac_sha256(ध्रुव शून्य *कुंजी, माप_प्रकार कुंजी_size, ध्रुव शून्य *निवेश, माप_प्रकार निवेश_size, uint8_t res[स्थिर SHA256_DIGEST_SIZE]);
