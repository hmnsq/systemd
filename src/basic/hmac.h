/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "sha256.h"

/* Unoptimized implementation based on FIPS 198. 'res' has to be allocated by
 * the caller. Prefer external OpenSSL functions, and use this only when
 * linking to OpenSSL is not desirable (eg: libsystemd.so). */
ശൂന്യം hmac_sha256(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_size, സ്ഥിരം ശൂന്യം *നിവേശം, size_t നിവേശം_size, uint8_t res[സ്ഥിതം SHA256_DIGEST_SIZE]);
