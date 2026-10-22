/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#જો defined(_MSC_VER)

પ્રકારવ્યાખ્યા unsigned અક્ષર uint8_t;
પ્રકારવ્યાખ્યા unsigned long uint32_t;
પ્રકારવ્યાખ્યા unsigned __int64 uint64_t;

// Other compilers

#નહીંતર        // defined(_MSC_VER)

#અન્તર્ભાવ <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( અચલ શૂન્ય * ચાવી, પૂર્ણાંક len, uint32_t seed );

//-----------------------------------------------------------------------------
