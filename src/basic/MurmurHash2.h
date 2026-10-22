/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#सचे defined(_MSC_VER)

प्रकारनिर्देश unsigned वर्ण uint8_t;
प्रकारनिर्देश unsigned long uint32_t;
प्रकारनिर्देश unsigned __int64 uint64_t;

// Other compilers

#अञ्ञथा        // defined(_MSC_VER)

#अन्तर्भाव <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( ध्रुव शून्य * कुञ्चिका, पूर्णाङ्क len, uint32_t seed );

//-----------------------------------------------------------------------------
