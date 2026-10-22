/* SPDX-License-Identifier: LicenseRef-murmurhash2-public-domain */
//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

#అన్తర్భావ "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ఒకవేళ defined(_MSC_VER)

#నిర్వచించు BIG_CONSTANT(x) (x)

// Other compilers

#లేకపోతే        // defined(_MSC_VER)

#నిర్వచించు BIG_CONSTANT(x) (x##LLU)

#అయితే_ముగింపు // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( స్థిరరాశి శూన్య * కీలకం, పూర్ణాంకం len, uint32_t విత్తనం )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  స్థిరరాశి uint32_t m = 0x5bd1e995;
  స్థిరరాశి పూర్ణాంకం r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = విత్తనం ^ len;

  // Mix 4 bytes at a time into the hash

  స్థిరరాశి గుర్తులేని అక్షరం * దత్తాంశం = (స్థిరరాశి గుర్తులేని అక్షరం *)కీలకం;

  అయితే (len >= 4)
  {
    uint32_t k = *(uint32_t*)దత్తాంశం;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    దత్తాంశం += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  ఎంపిక(len)
  {
  స్థితి 3: h ^= దత్తాంశం[2] << 16; /* fall through */
  స్థితి 2: h ^= దత్తాంశం[1] << 8;  /* fall through */
  స్థితి 1: h ^= దత్తాంశం[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  తిరిగి h;
}
