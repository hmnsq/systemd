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

#அந்தர்பாவ "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#எனில் defined(_MSC_VER)

#define BIG_CONSTANT(x) (x)

// Other compilers

#இல்லையெனில்        // defined(_MSC_VER)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( மாறிலி வெறுமை * சாவி, முழு len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  மாறிலி uint32_t m = 0x5bd1e995;
  மாறிலி முழு r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  மாறிலி unsigned எழுத்து * data = (மாறிலி unsigned எழுத்து *)சாவி;

  போது (len >= 4)
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

  தேர்வு(len)
  {
  நிலை 3: h ^= data[2] << 16; /* fall through */
  நிலை 2: h ^= data[1] << 8;  /* fall through */
  நிலை 1: h ^= data[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  திரும்பு h;
}
