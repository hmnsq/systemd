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
#لارښوونه once

#ضمن <byteswap.h>
#ضمن <endian.h>

#ضمن "forward.h"

#که_تعریف_شوی __CHECKER__
#تعریف_کړه __sd_bitwise __attribute__((__bitwise__))
#تعریف_کړه __sd_force __attribute__((__force__))
#وإلا
#تعریف_کړه __sd_bitwise
#تعریف_کړه __sd_force
#د_که_پای

تعريفنوع uint16_t __sd_bitwise le16_t;
تعريفنوع uint16_t __sd_bitwise be16_t;
تعريفنوع uint32_t __sd_bitwise le32_t;
تعريفنوع uint32_t __sd_bitwise be32_t;
تعريفنوع uint64_t __sd_bitwise le64_t;
تعريفنوع uint64_t __sd_bitwise be64_t;

#تعریف_لغوه htobe16
#تعریف_لغوه htole16
#تعریف_لغوه be16toh
#تعریف_لغوه le16toh
#تعریف_لغوه htobe32
#تعریف_لغوه htole32
#تعریف_لغوه be32toh
#تعریف_لغوه le32toh
#تعریف_لغوه htobe64
#تعریف_لغوه htole64
#تعریف_لغوه be64toh
#تعریف_لغوه le64toh

#إذا __BYTE_ORDER == __LITTLE_ENDIAN
#تعریف_کړه bswap_16_on_le(x) bswap_16(x)
#تعریف_کړه bswap_32_on_le(x) bswap_32(x)
#تعریف_کړه bswap_64_on_le(x) bswap_64(x)
#تعریف_کړه bswap_16_on_be(x) (x)
#تعریف_کړه bswap_32_on_be(x) (x)
#تعریف_کړه bswap_64_on_be(x) (x)
#وإلاإذا __BYTE_ORDER == __BIG_ENDIAN
#تعریف_کړه bswap_16_on_le(x) (x)
#تعریف_کړه bswap_32_on_le(x) (x)
#تعریف_کړه bswap_64_on_le(x) (x)
#تعریف_کړه bswap_16_on_be(x) bswap_16(x)
#تعریف_کړه bswap_32_on_be(x) bswap_32(x)
#تعریف_کړه bswap_64_on_be(x) bswap_64(x)
#د_که_پای

ساكن مضمن le16_t htole16(uint16_t ارزښت) { أرجع (le16_t __sd_force) bswap_16_on_be(ارزښت); }
ساكن مضمن le32_t htole32(uint32_t ارزښت) { أرجع (le32_t __sd_force) bswap_32_on_be(ارزښت); }
ساكن مضمن le64_t htole64(uint64_t ارزښت) { أرجع (le64_t __sd_force) bswap_64_on_be(ارزښت); }

ساكن مضمن be16_t htobe16(uint16_t ارزښت) { أرجع (be16_t __sd_force) bswap_16_on_le(ارزښت); }
ساكن مضمن be32_t htobe32(uint32_t ارزښت) { أرجع (be32_t __sd_force) bswap_32_on_le(ارزښت); }
ساكن مضمن be64_t htobe64(uint64_t ارزښت) { أرجع (be64_t __sd_force) bswap_64_on_le(ارزښت); }

ساكن مضمن uint16_t le16toh(le16_t ارزښت) { أرجع bswap_16_on_be((uint16_t __sd_force)ارزښت); }
ساكن مضمن uint32_t le32toh(le32_t ارزښت) { أرجع bswap_32_on_be((uint32_t __sd_force)ارزښت); }
ساكن مضمن uint64_t le64toh(le64_t ارزښت) { أرجع bswap_64_on_be((uint64_t __sd_force)ارزښت); }

ساكن مضمن uint16_t be16toh(be16_t ارزښت) { أرجع bswap_16_on_le((uint16_t __sd_force)ارزښت); }
ساكن مضمن uint32_t be32toh(be32_t ارزښت) { أرجع bswap_32_on_le((uint32_t __sd_force)ارزښت); }
ساكن مضمن uint64_t be64toh(be64_t ارزښت) { أرجع bswap_64_on_le((uint64_t __sd_force)ارزښت); }

#تعریف_لغوه __sd_bitwise
#تعریف_لغوه __sd_force
