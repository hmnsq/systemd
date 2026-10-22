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
#هدايت once

#ضمن <byteswap.h>
#ضمن <endian.h>

#ضمن "forward.h"

#جيڪڏهن_وصف_ٿيل __CHECKER__
#وصف_ڪريو __sd_bitwise __attribute__((__bitwise__))
#وصف_ڪريو __sd_force __attribute__((__force__))
#وإلا
#وصف_ڪريو __sd_bitwise
#وصف_ڪريو __sd_force
#جيڪڏهن_پورو

تعريفنوع uint16_t __sd_bitwise le16_t;
تعريفنوع uint16_t __sd_bitwise be16_t;
تعريفنوع uint32_t __sd_bitwise le32_t;
تعريفنوع uint32_t __sd_bitwise be32_t;
تعريفنوع uint64_t __sd_bitwise le64_t;
تعريفنوع uint64_t __sd_bitwise be64_t;

#وصف_رد htobe16
#وصف_رد htole16
#وصف_رد be16toh
#وصف_رد le16toh
#وصف_رد htobe32
#وصف_رد htole32
#وصف_رد be32toh
#وصف_رد le32toh
#وصف_رد htobe64
#وصف_رد htole64
#وصف_رد be64toh
#وصف_رد le64toh

#إذا __BYTE_ORDER == __LITTLE_ENDIAN
#وصف_ڪريو bswap_16_on_le(x) bswap_16(x)
#وصف_ڪريو bswap_32_on_le(x) bswap_32(x)
#وصف_ڪريو bswap_64_on_le(x) bswap_64(x)
#وصف_ڪريو bswap_16_on_be(x) (x)
#وصف_ڪريو bswap_32_on_be(x) (x)
#وصف_ڪريو bswap_64_on_be(x) (x)
#وإلاإذا __BYTE_ORDER == __BIG_ENDIAN
#وصف_ڪريو bswap_16_on_le(x) (x)
#وصف_ڪريو bswap_32_on_le(x) (x)
#وصف_ڪريو bswap_64_on_le(x) (x)
#وصف_ڪريو bswap_16_on_be(x) bswap_16(x)
#وصف_ڪريو bswap_32_on_be(x) bswap_32(x)
#وصف_ڪريو bswap_64_on_be(x) bswap_64(x)
#جيڪڏهن_پورو

ساكن مضمن le16_t htole16(uint16_t قدر) { أرجع (le16_t __sd_force) bswap_16_on_be(قدر); }
ساكن مضمن le32_t htole32(uint32_t قدر) { أرجع (le32_t __sd_force) bswap_32_on_be(قدر); }
ساكن مضمن le64_t htole64(uint64_t قدر) { أرجع (le64_t __sd_force) bswap_64_on_be(قدر); }

ساكن مضمن be16_t htobe16(uint16_t قدر) { أرجع (be16_t __sd_force) bswap_16_on_le(قدر); }
ساكن مضمن be32_t htobe32(uint32_t قدر) { أرجع (be32_t __sd_force) bswap_32_on_le(قدر); }
ساكن مضمن be64_t htobe64(uint64_t قدر) { أرجع (be64_t __sd_force) bswap_64_on_le(قدر); }

ساكن مضمن uint16_t le16toh(le16_t قدر) { أرجع bswap_16_on_be((uint16_t __sd_force)قدر); }
ساكن مضمن uint32_t le32toh(le32_t قدر) { أرجع bswap_32_on_be((uint32_t __sd_force)قدر); }
ساكن مضمن uint64_t le64toh(le64_t قدر) { أرجع bswap_64_on_be((uint64_t __sd_force)قدر); }

ساكن مضمن uint16_t be16toh(be16_t قدر) { أرجع bswap_16_on_le((uint16_t __sd_force)قدر); }
ساكن مضمن uint32_t be32toh(be32_t قدر) { أرجع bswap_32_on_le((uint32_t __sd_force)قدر); }
ساكن مضمن uint64_t be64toh(be64_t قدر) { أرجع bswap_64_on_le((uint64_t __sd_force)قدر); }

#وصف_رد __sd_bitwise
#وصف_رد __sd_force
