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

#समावेश "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#अगर defined(_MSC_VER)

#घोषणा BIG_CONSTANT(x) (x)

// Other compilers

#अन्यथा        // defined(_MSC_VER)

#घोषणा BIG_CONSTANT(x) (x##LLU)

#पूर्ण_अगर // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( स्थिर व्योम * कुंजी, पूर्णांक len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  स्थिर uint32_t m = 0x5bd1e995;
  स्थिर पूर्णांक r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  स्थिर अचिन्हित अक्षर * तथ्य = (स्थिर अचिन्हित अक्षर *)कुंजी;

  जबतक (len >= 4)
  {
    uint32_t k = *(uint32_t*)तथ्य;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    तथ्य += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  चयन(len)
  {
  हाल 3: h ^= तथ्य[2] << 16; /* fall through */
  हाल 2: h ^= तथ्य[1] << 8;  /* fall through */
  हाल 1: h ^= तथ्य[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  वापस h;
}
