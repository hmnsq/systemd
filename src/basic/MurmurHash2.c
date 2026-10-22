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

#ಅನ್ತರ್ಭಾವ "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ಯದಿ defined(_MSC_VER)

#define BIG_CONSTANT(x) (x)

// Other compilers

#ಅನ್ಯಥಾ        // defined(_MSC_VER)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( ಧ್ರುವ ಶೂನ್ಯ * ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  ಧ್ರುವ uint32_t m = 0x5bd1e995;
  ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  ಧ್ರುವ unsigned ವರ್ಣ * data = (ಧ್ರುವ unsigned ವರ್ಣ *)ಕೀಲಿ;

  ಯಾವತ್ (len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  ವಿಕಲ್ಪ(len)
  {
  ಪಕ್ಷ 3: h ^= data[2] << 16; /* fall through */
  ಪಕ್ಷ 2: h ^= data[1] << 8;  /* fall through */
  ಪಕ್ಷ 1: h ^= data[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  ಪ್ರತ್ಯಾಗಮ h;
}
