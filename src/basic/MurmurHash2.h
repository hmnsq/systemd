/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#هدايت once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#إذا defined(_MSC_VER)

تعريفنوع بنانشان حرف uint8_t;
تعريفنوع بنانشان ڊگهو uint32_t;
تعريفنوع بنانشان __int64 uint64_t;

// Other compilers

#وإلا        // defined(_MSC_VER)

#ضمن <stdint.h>

#جيڪڏهن_پورو // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ثابت فراغ * ڪنجي, صحيح len, uint32_t ٻج );

//-----------------------------------------------------------------------------
