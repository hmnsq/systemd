/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#যদি defined(_MSC_VER)

প্রকারলেখ unsigned অক্ষর uint8_t;
প্রকারলেখ unsigned long uint32_t;
প্রকারলেখ unsigned __int64 uint64_t;

// Other compilers

#অন্যথা        // defined(_MSC_VER)

#অন্তর্ভাব <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ধ্রুবক শূন্য * চাবি, পূর্ণসংখ্যা len, uint32_t seed );

//-----------------------------------------------------------------------------
