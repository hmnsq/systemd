/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ਹਦਾਇਤ once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ਜੇ defined(_MSC_VER)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uint8_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ uint32_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ __int64 uint64_t;

// Other compilers

#ਨਹੀਂਤਾਂ        // defined(_MSC_VER)

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

#ਜੇ_ਅੰਤ // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ਅਚਲ ਖਾਲੀ * ਕੁੰਜੀ, ਪੂਰਨ len, uint32_t ਬੀਜ );

//-----------------------------------------------------------------------------
