/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#رهنمود once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#إذا defined(_MSC_VER)

تعريفنوع بیعلامت حرف uint8_t;
تعريفنوع بیعلامت دراز uint32_t;
تعريفنوع بیعلامت __int64 uint64_t;

// Other compilers

#وإلا        // defined(_MSC_VER)

#ضمن <stdint.h>

#پایان_اگر // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ثابت فراغ * کلید, صحيح len, uint32_t دانه );

//-----------------------------------------------------------------------------
