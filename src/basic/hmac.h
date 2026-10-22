/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
শূন্য hmac_sha256(ধ্রুব শূন্য *চাবি, size_t চাবি_size, ধ্রুব শূন্য *প্রবেশ, size_t প্রবেশ_size, uint8_t res[স্থির SHA256_DIGEST_SIZE]);
