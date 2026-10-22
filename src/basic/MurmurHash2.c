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

#ಆದರೆ defined(_MSC_VER)

#ವ್ಯಾಖ್ಯಾನಿಸು BIG_CONSTANT(x) (x)

// Other compilers

#ಇಲ್ಲದಿದ್ದರೆ        // defined(_MSC_VER)

#ವ್ಯಾಖ್ಯಾನಿಸು BIG_CONSTANT(x) (x##LLU)

#ಆದರೆ_ಅಂತ್ಯ // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * ಕೀಲಿ, ಪೂರ್ಣಾಂಕ len, uint32_t ಬೀಜ )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  ಸ್ಥಿರಾಂಕ uint32_t m = 0x5bd1e995;
  ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = ಬೀಜ ^ len;

  // Mix 4 bytes at a time into the hash

  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ * ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಕೀಲಿ;

  ಆಗ (len >= 4)
  {
    uint32_t k = *(uint32_t*)ದತ್ತಾಂಶ;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    ದತ್ತಾಂಶ += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  ಆಯ್ಕೆ(len)
  {
  ಸ್ಥಿತಿ 3: h ^= ದತ್ತಾಂಶ[2] << 16; /* fall through */
  ಸ್ಥಿತಿ 2: h ^= ದತ್ತಾಂಶ[1] << 8;  /* fall through */
  ಸ್ಥಿತಿ 1: h ^= ದತ್ತಾಂಶ[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  ಹಿಂತಿರುಗಿ h;
}
