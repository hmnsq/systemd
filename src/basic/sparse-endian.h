/* SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2012 Josh Triplett <josh@joshtriplett.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#pragma once

#ଅନ୍ତର୍ଭାଵ <byteswap.h>
#ଅନ୍ତର୍ଭାଵ <endian.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

#ifdef __CHECKER__
#define __sd_bitwise __attribute__((__bitwise__))
#define __sd_force __attribute__((__force__))
#ଅନ୍ୟଥା
#define __sd_bitwise
#define __sd_force
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା uint16_t __sd_bitwise le16_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint16_t __sd_bitwise be16_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint32_t __sd_bitwise le32_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint32_t __sd_bitwise be32_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint64_t __sd_bitwise le64_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint64_t __sd_bitwise be64_t;

#undef htobe16
#undef htole16
#undef be16toh
#undef le16toh
#undef htobe32
#undef htole32
#undef be32toh
#undef le32toh
#undef htobe64
#undef htole64
#undef be64toh
#undef le64toh

#ଯଦି __BYTE_ORDER == __LITTLE_ENDIAN
#define bswap_16_on_le(x) bswap_16(x)
#define bswap_32_on_le(x) bswap_32(x)
#define bswap_64_on_le(x) bswap_64(x)
#define bswap_16_on_be(x) (x)
#define bswap_32_on_be(x) (x)
#define bswap_64_on_be(x) (x)
#ନଚେତଯଦି __BYTE_ORDER == __BIG_ENDIAN
#define bswap_16_on_le(x) (x)
#define bswap_32_on_le(x) (x)
#define bswap_64_on_le(x) (x)
#define bswap_16_on_be(x) bswap_16(x)
#define bswap_32_on_be(x) bswap_32(x)
#define bswap_64_on_be(x) bswap_64(x)
#endif

ସ୍ଥିର ଧାଡ଼ିଭିତରେ le16_t htole16(uint16_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ (le16_t __sd_force) bswap_16_on_be(ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ le32_t htole32(uint32_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ (le32_t __sd_force) bswap_32_on_be(ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ le64_t htole64(uint64_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ (le64_t __sd_force) bswap_64_on_be(ମୂଲ୍ୟ); }

ସ୍ଥିର ଧାଡ଼ିଭିତରେ be16_t htobe16(uint16_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ (be16_t __sd_force) bswap_16_on_le(ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ be32_t htobe32(uint32_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ (be32_t __sd_force) bswap_32_on_le(ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ be64_t htobe64(uint64_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ (be64_t __sd_force) bswap_64_on_le(ମୂଲ୍ୟ); }

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint16_t le16toh(le16_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ bswap_16_on_be((uint16_t __sd_force)ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t le32toh(le32_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ bswap_32_on_be((uint32_t __sd_force)ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t le64toh(le64_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ bswap_64_on_be((uint64_t __sd_force)ମୂଲ୍ୟ); }

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint16_t be16toh(be16_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ bswap_16_on_le((uint16_t __sd_force)ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t be32toh(be32_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ bswap_32_on_le((uint32_t __sd_force)ମୂଲ୍ୟ); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t be64toh(be64_t ମୂଲ୍ୟ) { ଫେରନ୍ତୁ bswap_64_on_le((uint64_t __sd_force)ମୂଲ୍ୟ); }

#undef __sd_bitwise
#undef __sd_force
