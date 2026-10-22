/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#जर defined(_MSC_VER)

प्रकारव्याख्या अचिन्हित अक्षर uint8_t;
प्रकारव्याख्या अचिन्हित दीर्घ uint32_t;
प्रकारव्याख्या अचिन्हित __int64 uint64_t;

// Other compilers

#अन्यथा        // defined(_MSC_VER)

#अन्तर्भाव <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( स्थिरांक शून्य * किल्ली, पूर्णांक len, uint32_t seed );

//-----------------------------------------------------------------------------
