/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#எனில் defined(_MSC_VER)

வகைவரை unsigned எழுத்து uint8_t;
வகைவரை unsigned long uint32_t;
வகைவரை unsigned __int64 uint64_t;

// Other compilers

#இல்லையெனில்        // defined(_MSC_VER)

#அந்தர்பாவ <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( மாறிலி வெறுமை * சாவி, முழு len, uint32_t seed );

//-----------------------------------------------------------------------------
