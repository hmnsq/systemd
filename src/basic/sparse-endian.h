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
#ہدایت once

#شامل <byteswap.h>
#شامل <endian.h>

#شامل "forward.h"

#اگر_تعریف_شدہ __CHECKER__
#تعریف_کرو __sd_bitwise __attribute__((__bitwise__))
#تعریف_کرو __sd_force __attribute__((__force__))
#ورنہ
#تعریف_کرو __sd_bitwise
#تعریف_کرو __sd_force
#اگر_ختم

قسمتعریف uint16_t __sd_bitwise le16_t;
قسمتعریف uint16_t __sd_bitwise be16_t;
قسمتعریف uint32_t __sd_bitwise le32_t;
قسمتعریف uint32_t __sd_bitwise be32_t;
قسمتعریف uint64_t __sd_bitwise le64_t;
قسمتعریف uint64_t __sd_bitwise be64_t;

#تعریف_منسوخ htobe16
#تعریف_منسوخ htole16
#تعریف_منسوخ be16toh
#تعریف_منسوخ le16toh
#تعریف_منسوخ htobe32
#تعریف_منسوخ htole32
#تعریف_منسوخ be32toh
#تعریف_منسوخ le32toh
#تعریف_منسوخ htobe64
#تعریف_منسوخ htole64
#تعریف_منسوخ be64toh
#تعریف_منسوخ le64toh

#اگر __BYTE_ORDER == __LITTLE_ENDIAN
#تعریف_کرو bswap_16_on_le(x) bswap_16(x)
#تعریف_کرو bswap_32_on_le(x) bswap_32(x)
#تعریف_کرو bswap_64_on_le(x) bswap_64(x)
#تعریف_کرو bswap_16_on_be(x) (x)
#تعریف_کرو bswap_32_on_be(x) (x)
#تعریف_کرو bswap_64_on_be(x) (x)
#وگرنہ_اگر __BYTE_ORDER == __BIG_ENDIAN
#تعریف_کرو bswap_16_on_le(x) (x)
#تعریف_کرو bswap_32_on_le(x) (x)
#تعریف_کرو bswap_64_on_le(x) (x)
#تعریف_کرو bswap_16_on_be(x) bswap_16(x)
#تعریف_کرو bswap_32_on_be(x) bswap_32(x)
#تعریف_کرو bswap_64_on_be(x) bswap_64(x)
#اگر_ختم

ساکن اندرونی le16_t htole16(uint16_t قدر) { واپس (le16_t __sd_force) bswap_16_on_be(قدر); }
ساکن اندرونی le32_t htole32(uint32_t قدر) { واپس (le32_t __sd_force) bswap_32_on_be(قدر); }
ساکن اندرونی le64_t htole64(uint64_t قدر) { واپس (le64_t __sd_force) bswap_64_on_be(قدر); }

ساکن اندرونی be16_t htobe16(uint16_t قدر) { واپس (be16_t __sd_force) bswap_16_on_le(قدر); }
ساکن اندرونی be32_t htobe32(uint32_t قدر) { واپس (be32_t __sd_force) bswap_32_on_le(قدر); }
ساکن اندرونی be64_t htobe64(uint64_t قدر) { واپس (be64_t __sd_force) bswap_64_on_le(قدر); }

ساکن اندرونی uint16_t le16toh(le16_t قدر) { واپس bswap_16_on_be((uint16_t __sd_force)قدر); }
ساکن اندرونی uint32_t le32toh(le32_t قدر) { واپس bswap_32_on_be((uint32_t __sd_force)قدر); }
ساکن اندرونی uint64_t le64toh(le64_t قدر) { واپس bswap_64_on_be((uint64_t __sd_force)قدر); }

ساکن اندرونی uint16_t be16toh(be16_t قدر) { واپس bswap_16_on_le((uint16_t __sd_force)قدر); }
ساکن اندرونی uint32_t be32toh(be32_t قدر) { واپس bswap_32_on_le((uint32_t __sd_force)قدر); }
ساکن اندرونی uint64_t be64toh(be64_t قدر) { واپس bswap_64_on_le((uint64_t __sd_force)قدر); }

#تعریف_منسوخ __sd_bitwise
#تعریف_منسوخ __sd_force
