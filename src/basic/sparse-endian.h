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
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <byteswap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <endian.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CHECKER__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __sd_bitwise __attribute__((__bitwise__))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __sd_force __attribute__((__force__))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __sd_bitwise
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __sd_force
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint16_t __sd_bitwise le16_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint16_t __sd_bitwise be16_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t __sd_bitwise le32_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t __sd_bitwise be32_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t __sd_bitwise le64_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t __sd_bitwise be64_t;

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ htobe16
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ htole16
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ be16toh
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ le16toh
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ htobe32
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ htole32
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ be32toh
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ le32toh
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ htobe64
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ htole64
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ be64toh
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ le64toh

#ਜੇ __BYTE_ORDER == __LITTLE_ENDIAN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_16_on_le(x) bswap_16(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_32_on_le(x) bswap_32(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_64_on_le(x) bswap_64(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_16_on_be(x) (x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_32_on_be(x) (x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_64_on_be(x) (x)
#ਜਾਂਜੇ __BYTE_ORDER == __BIG_ENDIAN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_16_on_le(x) (x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_32_on_le(x) (x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_64_on_le(x) (x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_16_on_be(x) bswap_16(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_32_on_be(x) bswap_32(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bswap_64_on_be(x) bswap_64(x)
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ le16_t htole16(uint16_t ਮੁੱਲ) { ਵਾਪਸ (le16_t __sd_force) bswap_16_on_be(ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ le32_t htole32(uint32_t ਮੁੱਲ) { ਵਾਪਸ (le32_t __sd_force) bswap_32_on_be(ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ le64_t htole64(uint64_t ਮੁੱਲ) { ਵਾਪਸ (le64_t __sd_force) bswap_64_on_be(ਮੁੱਲ); }

ਸਥਿਰ ਸਤਰਅੰਦਰ be16_t htobe16(uint16_t ਮੁੱਲ) { ਵਾਪਸ (be16_t __sd_force) bswap_16_on_le(ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ be32_t htobe32(uint32_t ਮੁੱਲ) { ਵਾਪਸ (be32_t __sd_force) bswap_32_on_le(ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ be64_t htobe64(uint64_t ਮੁੱਲ) { ਵਾਪਸ (be64_t __sd_force) bswap_64_on_le(ਮੁੱਲ); }

ਸਥਿਰ ਸਤਰਅੰਦਰ uint16_t le16toh(le16_t ਮੁੱਲ) { ਵਾਪਸ bswap_16_on_be((uint16_t __sd_force)ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t le32toh(le32_t ਮੁੱਲ) { ਵਾਪਸ bswap_32_on_be((uint32_t __sd_force)ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t le64toh(le64_t ਮੁੱਲ) { ਵਾਪਸ bswap_64_on_be((uint64_t __sd_force)ਮੁੱਲ); }

ਸਥਿਰ ਸਤਰਅੰਦਰ uint16_t be16toh(be16_t ਮੁੱਲ) { ਵਾਪਸ bswap_16_on_le((uint16_t __sd_force)ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t be32toh(be32_t ਮੁੱਲ) { ਵਾਪਸ bswap_32_on_le((uint32_t __sd_force)ਮੁੱਲ); }
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t be64toh(be64_t ਮੁੱਲ) { ਵਾਪਸ bswap_64_on_le((uint64_t __sd_force)ਮੁੱਲ); }

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __sd_bitwise
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __sd_force
