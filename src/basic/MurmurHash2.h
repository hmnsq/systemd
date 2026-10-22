/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#إذا defined(_MSC_VER)

تعريفنوع unsigned حرف uint8_t;
تعريفنوع unsigned long uint32_t;
تعريفنوع unsigned __int64 uint64_t;

// Other compilers

#وإلا        // defined(_MSC_VER)

#ضمن <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ثابت فراغ * مفتاح, صحيح len, uint32_t seed );

//-----------------------------------------------------------------------------
