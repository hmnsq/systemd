/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#اگر defined(_MSC_VER)

تعریفنوع unsigned نویسه uint8_t;
تعریفنوع unsigned long uint32_t;
تعریفنوع unsigned __int64 uint64_t;

// Other compilers

#وگرنه        // defined(_MSC_VER)

#شامل <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ثابت تهی * کلید, صحیح len, uint32_t seed );

//-----------------------------------------------------------------------------
