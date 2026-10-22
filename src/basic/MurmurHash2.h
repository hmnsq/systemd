/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#എങ്കിൽ defined(_MSC_VER)

തരനിർവചനം unsigned അക്ഷരം uint8_t;
തരനിർവചനം unsigned long uint32_t;
തരനിർവചനം unsigned __int64 uint64_t;

// Other compilers

#അല്ലെങ്കിൽ        // defined(_MSC_VER)

#അന്തര്ഭാവ <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( സ്ഥിരം ശൂന്യം * താക്കോൽ, പൂർണ്ണസംഖ്യ len, uint32_t seed );

//-----------------------------------------------------------------------------
