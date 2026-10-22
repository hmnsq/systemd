/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ಸೂಚನೆ once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ಆದರೆ defined(_MSC_VER)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uint8_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ uint32_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ __int64 uint64_t;

// Other compilers

#ಇಲ್ಲದಿದ್ದರೆ        // defined(_MSC_VER)

#ಅನ್ತರ್ಭಾವ <stdint.h>

#ಆದರೆ_ಅಂತ್ಯ // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * ಕೀಲಿ, ಪೂರ್ಣಾಂಕ len, uint32_t ಬೀಜ );

//-----------------------------------------------------------------------------
