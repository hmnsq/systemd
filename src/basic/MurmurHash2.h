/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ಯದಿ defined(_MSC_VER)

ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ವರ್ಣ uint8_t;
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned long uint32_t;
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned __int64 uint64_t;

// Other compilers

#ಅನ್ಯಥಾ        // defined(_MSC_VER)

#ಅನ್ತರ್ಭಾವ <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ಧ್ರುವ ಶೂನ್ಯ * ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ len, uint32_t seed );

//-----------------------------------------------------------------------------
