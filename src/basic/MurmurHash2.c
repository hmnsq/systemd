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

#അന്തര്ഭാവ "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#എങ്കിൽ defined(_MSC_VER)

#define BIG_CONSTANT(x) (x)

// Other compilers

#അല്ലെങ്കിൽ        // defined(_MSC_VER)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( സ്ഥിരം ശൂന്യം * താക്കോൽ, പൂർണ്ണസംഖ്യ len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  സ്ഥിരം uint32_t m = 0x5bd1e995;
  സ്ഥിരം പൂർണ്ണസംഖ്യ r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  സ്ഥിരം unsigned അക്ഷരം * data = (സ്ഥിരം unsigned അക്ഷരം *)താക്കോൽ;

  ആകുമ്പോൾ (len >= 4)
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

  തിരഞ്ഞെടുപ്പ്(len)
  {
  അവസ്ഥ 3: h ^= data[2] << 16; /* fall through */
  അവസ്ഥ 2: h ^= data[1] << 8;  /* fall through */
  അവസ്ഥ 1: h ^= data[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  തിരികെ h;
}
