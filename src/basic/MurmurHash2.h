/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#आशय once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#अगर defined(_MSC_VER)

प्रकार अचिन्हित अक्षर uint8_t;
प्रकार अचिन्हित दीर्घ uint32_t;
प्रकार अचिन्हित __int64 uint64_t;

// Other compilers

#अन्यथा        // defined(_MSC_VER)

#समावेश <stdint.h>

#पूर्ण_अगर // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( स्थिर व्योम * कुंजी, पूर्णांक len, uint32_t seed );

//-----------------------------------------------------------------------------
