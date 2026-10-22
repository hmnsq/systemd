/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ہدایت once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#اگر defined(_MSC_VER)

قسمتعریف غیرعلامتی حرف uint8_t;
قسمتعریف غیرعلامتی طویل uint32_t;
قسمتعریف غیرعلامتی __int64 uint64_t;

// Other compilers

#ورنہ        // defined(_MSC_VER)

#شامل <stdint.h>

#اگر_ختم // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ثابت خالی * کلید, صحیح len, uint32_t بیج );

//-----------------------------------------------------------------------------
