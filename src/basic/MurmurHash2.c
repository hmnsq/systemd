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

#ضمن "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#إذا defined(_MSC_VER)

#تعریف_کن BIG_CONSTANT(x) (x)

// Other compilers

#وإلا        // defined(_MSC_VER)

#تعریف_کن BIG_CONSTANT(x) (x##LLU)

#پایان_اگر // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( ثابت فراغ * کلید, صحيح len, uint32_t دانه )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  ثابت uint32_t m = 0x5bd1e995;
  ثابت صحيح r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = دانه ^ len;

  // Mix 4 bytes at a time into the hash

  ثابت بیعلامت حرف * داده = (ثابت بیعلامت حرف *)کلید;

  طالما (len >= 4)
  {
    uint32_t k = *(uint32_t*)داده;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    داده += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  بدل(len)
  {
  حالة 3: h ^= داده[2] << 16; /* fall through */
  حالة 2: h ^= داده[1] << 8;  /* fall through */
  حالة 1: h ^= داده[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  أرجع h;
}
