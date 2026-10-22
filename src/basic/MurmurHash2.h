/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ଯଦି defined(_MSC_VER)

ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned ଅକ୍ଷର uint8_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long uint32_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned __int64 uint64_t;

// Other compilers

#ଅନ୍ୟଥା        // defined(_MSC_VER)

#ଅନ୍ତର୍ଭାଵ <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ଅଚଳ ଶୂନ୍ୟ * ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, uint32_t seed );

//-----------------------------------------------------------------------------
