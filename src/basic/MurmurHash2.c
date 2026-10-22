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

#অন্তর্ভাব "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#যদি defined(_MSC_VER)

#define BIG_CONSTANT(x) (x)

// Other compilers

#অন্যথা        // defined(_MSC_VER)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( ধ্রুবক শূন্য * চাবি, পূর্ণসংখ্যা len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  ধ্রুবক uint32_t m = 0x5bd1e995;
  ধ্রুবক পূর্ণসংখ্যা r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  ধ্রুবক unsigned অক্ষর * তথ্য = (ধ্রুবক unsigned অক্ষর *)চাবি;

  যখন (len >= 4)
  {
    uint32_t k = *(uint32_t*)তথ্য;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    তথ্য += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  নির্বাচন(len)
  {
  ক্ষেত্র 3: h ^= তথ্য[2] << 16; /* fall through */
  ক্ষেত্র 2: h ^= তথ্য[1] << 8;  /* fall through */
  ক্ষেত্র 1: h ^= তথ্য[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  ফেরত h;
}
