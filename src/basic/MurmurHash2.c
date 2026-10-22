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

#ਅਨ੍ਤਰ੍ਭਾਵ "MurmurHash2.h"

_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#ਜੇ defined(_MSC_VER)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIG_CONSTANT(x) (x)

// Other compilers

#ਨਹੀਂਤਾਂ        // defined(_MSC_VER)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIG_CONSTANT(x) (x##LLU)

#ਜੇ_ਅੰਤ // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( ਅਚਲ ਖਾਲੀ * ਕੁੰਜੀ, ਪੂਰਨ len, uint32_t ਬੀਜ )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  ਅਚਲ uint32_t m = 0x5bd1e995;
  ਅਚਲ ਪੂਰਨ r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = ਬੀਜ ^ len;

  // Mix 4 bytes at a time into the hash

  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ * ਅੰਕੜੇ = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਕੁੰਜੀ;

  ਜਦੋਂ (len >= 4)
  {
    uint32_t k = *(uint32_t*)ਅੰਕੜੇ;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    ਅੰਕੜੇ += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  ਚੋਣ(len)
  {
  ਹਾਲਤ 3: h ^= ਅੰਕੜੇ[2] << 16; /* fall through */
  ਹਾਲਤ 2: h ^= ਅੰਕੜੇ[1] << 8;  /* fall through */
  ਹਾਲਤ 1: h ^= ਅੰਕੜੇ[0];       /* fall through */
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  ਵਾਪਸ h;
}
