/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#لارښوونه once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#إذا defined(_MSC_VER)

تعريفنوع بینښه حرف uint8_t;
تعريفنوع بینښه اوږد uint32_t;
تعريفنوع بینښه __int64 uint64_t;

// Other compilers

#وإلا        // defined(_MSC_VER)

#ضمن <stdint.h>

#د_که_پای // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ثابت فراغ * کلی, صحيح len, uint32_t تخم );

//-----------------------------------------------------------------------------
