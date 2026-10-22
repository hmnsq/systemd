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
#आशय once

#समावेश <byteswap.h>
#समावेश <endian.h>

#समावेश "forward.h"

#अगर_घोषित __CHECKER__
#घोषणा __sd_bitwise __attribute__((__bitwise__))
#घोषणा __sd_force __attribute__((__force__))
#अन्यथा
#घोषणा __sd_bitwise
#घोषणा __sd_force
#पूर्ण_अगर

प्रकार uint16_t __sd_bitwise le16_t;
प्रकार uint16_t __sd_bitwise be16_t;
प्रकार uint32_t __sd_bitwise le32_t;
प्रकार uint32_t __sd_bitwise be32_t;
प्रकार uint64_t __sd_bitwise le64_t;
प्रकार uint64_t __sd_bitwise be64_t;

#अघोषित htobe16
#अघोषित htole16
#अघोषित be16toh
#अघोषित le16toh
#अघोषित htobe32
#अघोषित htole32
#अघोषित be32toh
#अघोषित le32toh
#अघोषित htobe64
#अघोषित htole64
#अघोषित be64toh
#अघोषित le64toh

#अगर __BYTE_ORDER == __LITTLE_ENDIAN
#घोषणा bswap_16_on_le(x) bswap_16(x)
#घोषणा bswap_32_on_le(x) bswap_32(x)
#घोषणा bswap_64_on_le(x) bswap_64(x)
#घोषणा bswap_16_on_be(x) (x)
#घोषणा bswap_32_on_be(x) (x)
#घोषणा bswap_64_on_be(x) (x)
#या_अगर __BYTE_ORDER == __BIG_ENDIAN
#घोषणा bswap_16_on_le(x) (x)
#घोषणा bswap_32_on_le(x) (x)
#घोषणा bswap_64_on_le(x) (x)
#घोषणा bswap_16_on_be(x) bswap_16(x)
#घोषणा bswap_32_on_be(x) bswap_32(x)
#घोषणा bswap_64_on_be(x) bswap_64(x)
#पूर्ण_अगर

अटल अंतरभूत le16_t htole16(uint16_t मूल्य) { वापस (le16_t __sd_force) bswap_16_on_be(मूल्य); }
अटल अंतरभूत le32_t htole32(uint32_t मूल्य) { वापस (le32_t __sd_force) bswap_32_on_be(मूल्य); }
अटल अंतरभूत le64_t htole64(uint64_t मूल्य) { वापस (le64_t __sd_force) bswap_64_on_be(मूल्य); }

अटल अंतरभूत be16_t htobe16(uint16_t मूल्य) { वापस (be16_t __sd_force) bswap_16_on_le(मूल्य); }
अटल अंतरभूत be32_t htobe32(uint32_t मूल्य) { वापस (be32_t __sd_force) bswap_32_on_le(मूल्य); }
अटल अंतरभूत be64_t htobe64(uint64_t मूल्य) { वापस (be64_t __sd_force) bswap_64_on_le(मूल्य); }

अटल अंतरभूत uint16_t le16toh(le16_t मूल्य) { वापस bswap_16_on_be((uint16_t __sd_force)मूल्य); }
अटल अंतरभूत uint32_t le32toh(le32_t मूल्य) { वापस bswap_32_on_be((uint32_t __sd_force)मूल्य); }
अटल अंतरभूत uint64_t le64toh(le64_t मूल्य) { वापस bswap_64_on_be((uint64_t __sd_force)मूल्य); }

अटल अंतरभूत uint16_t be16toh(be16_t मूल्य) { वापस bswap_16_on_le((uint16_t __sd_force)मूल्य); }
अटल अंतरभूत uint32_t be32toh(be32_t मूल्य) { वापस bswap_32_on_le((uint32_t __sd_force)मूल्य); }
अटल अंतरभूत uint64_t be64toh(be64_t मूल्य) { वापस bswap_64_on_le((uint64_t __sd_force)मूल्य); }

#अघोषित __sd_bitwise
#अघोषित __sd_force
