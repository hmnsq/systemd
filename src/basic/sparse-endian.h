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
#निर्देशः once

#अन्तर्भाव <byteswap.h>
#अन्तर्भाव <endian.h>

#अन्तर्भाव "forward.h"

#यदि_परिभाषितम् __CHECKER__
#परिभाषय __sd_bitwise __attribute__((__bitwise__))
#परिभाषय __sd_force __attribute__((__force__))
#अन्यथा
#परिभाषय __sd_bitwise
#परिभाषय __sd_force
#यदि_अन्तः

प्रकारनिर्देश uint16_t __sd_bitwise le16_t;
प्रकारनिर्देश uint16_t __sd_bitwise be16_t;
प्रकारनिर्देश uint32_t __sd_bitwise le32_t;
प्रकारनिर्देश uint32_t __sd_bitwise be32_t;
प्रकारनिर्देश uint64_t __sd_bitwise le64_t;
प्रकारनिर्देश uint64_t __sd_bitwise be64_t;

#अपरिभाषय htobe16
#अपरिभाषय htole16
#अपरिभाषय be16toh
#अपरिभाषय le16toh
#अपरिभाषय htobe32
#अपरिभाषय htole32
#अपरिभाषय be32toh
#अपरिभाषय le32toh
#अपरिभाषय htobe64
#अपरिभाषय htole64
#अपरिभाषय be64toh
#अपरिभाषय le64toh

#यदि __BYTE_ORDER == __LITTLE_ENDIAN
#परिभाषय bswap_16_on_le(x) bswap_16(x)
#परिभाषय bswap_32_on_le(x) bswap_32(x)
#परिभाषय bswap_64_on_le(x) bswap_64(x)
#परिभाषय bswap_16_on_be(x) (x)
#परिभाषय bswap_32_on_be(x) (x)
#परिभाषय bswap_64_on_be(x) (x)
#नोचेद्यदि __BYTE_ORDER == __BIG_ENDIAN
#परिभाषय bswap_16_on_le(x) (x)
#परिभाषय bswap_32_on_le(x) (x)
#परिभाषय bswap_64_on_le(x) (x)
#परिभाषय bswap_16_on_be(x) bswap_16(x)
#परिभाषय bswap_32_on_be(x) bswap_32(x)
#परिभाषय bswap_64_on_be(x) bswap_64(x)
#यदि_अन्तः

स्थिर अन्तःस्थ le16_t htole16(uint16_t मानम्) { प्रत्यागम (le16_t __sd_force) bswap_16_on_be(मानम्); }
स्थिर अन्तःस्थ le32_t htole32(uint32_t मानम्) { प्रत्यागम (le32_t __sd_force) bswap_32_on_be(मानम्); }
स्थिर अन्तःस्थ le64_t htole64(uint64_t मानम्) { प्रत्यागम (le64_t __sd_force) bswap_64_on_be(मानम्); }

स्थिर अन्तःस्थ be16_t htobe16(uint16_t मानम्) { प्रत्यागम (be16_t __sd_force) bswap_16_on_le(मानम्); }
स्थिर अन्तःस्थ be32_t htobe32(uint32_t मानम्) { प्रत्यागम (be32_t __sd_force) bswap_32_on_le(मानम्); }
स्थिर अन्तःस्थ be64_t htobe64(uint64_t मानम्) { प्रत्यागम (be64_t __sd_force) bswap_64_on_le(मानम्); }

स्थिर अन्तःस्थ uint16_t le16toh(le16_t मानम्) { प्रत्यागम bswap_16_on_be((uint16_t __sd_force)मानम्); }
स्थिर अन्तःस्थ uint32_t le32toh(le32_t मानम्) { प्रत्यागम bswap_32_on_be((uint32_t __sd_force)मानम्); }
स्थिर अन्तःस्थ uint64_t le64toh(le64_t मानम्) { प्रत्यागम bswap_64_on_be((uint64_t __sd_force)मानम्); }

स्थिर अन्तःस्थ uint16_t be16toh(be16_t मानम्) { प्रत्यागम bswap_16_on_le((uint16_t __sd_force)मानम्); }
स्थिर अन्तःस्थ uint32_t be32toh(be32_t मानम्) { प्रत्यागम bswap_32_on_le((uint32_t __sd_force)मानम्); }
स्थिर अन्तःस्थ uint64_t be64toh(be64_t मानम्) { प्रत्यागम bswap_64_on_le((uint64_t __sd_force)मानम्); }

#अपरिभाषय __sd_bitwise
#अपरिभाषय __sd_force
