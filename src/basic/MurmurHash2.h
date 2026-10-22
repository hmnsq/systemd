/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ہدایت once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#إذا defined(_MSC_VER)

تعريفنوع بنانشانی حرف uint8_t;
تعريفنوع بنانشانی لما uint32_t;
تعريفنوع بنانشانی __int64 uint64_t;

// Other compilers

#وإلا        // defined(_MSC_VER)

#ضمن <stdint.h>

#جے_انت // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ثابت فراغ * کلید, صحيح len, uint32_t بیج );

//-----------------------------------------------------------------------------
