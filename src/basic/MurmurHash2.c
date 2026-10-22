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

#अन्तर्भाव "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#यदि defined(_MSC_VER)

#परिभाषय BIG_CONSTANT(x) (x)

// Other compilers

#अन्यथा        // defined(_MSC_VER)

#परिभाषय BIG_CONSTANT(x) (x##LLU)

#यदि_अन्तः // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( ध्रुव शून्य * कुञ्चिका, पूर्णाङ्क len, uint32_t बीजम् )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  ध्रुव uint32_t m = 0x5bd1e995;
  ध्रुव पूर्णाङ्क r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = बीजम् ^ len;

  // Mix 4 bytes at a time into the hash

  ध्रुव अचिह्नित वर्ण * दत्त = (ध्रुव अचिह्नित वर्ण *)कुञ्चिका;

  यावत् (len >= 4)
  {
    uint32_t k = *(uint32_t*)दत्त;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    दत्त += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  विकल्प(len)
  {
  पक्ष 3: h ^= दत्त[2] << 16; /* fall through */
  पक्ष 2: h ^= दत्त[1] << 8;  /* fall through */
  पक्ष 1: h ^= दत्त[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  प्रत्यागम h;
}
