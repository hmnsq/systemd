/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#సూచన once

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ఒకవేళ defined(_MSC_VER)

రకనిర్వచనం గుర్తులేని అక్షరం uint8_t;
రకనిర్వచనం గుర్తులేని దీర్ఘ uint32_t;
రకనిర్వచనం గుర్తులేని __int64 uint64_t;

// Other compilers

#లేకపోతే        // defined(_MSC_VER)

#అన్తర్భావ <stdint.h>

#అయితే_ముగింపు // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2        ( స్థిరరాశి శూన్య * కీలకం, పూర్ణాంకం len, uint32_t విత్తనం );

//-----------------------------------------------------------------------------
