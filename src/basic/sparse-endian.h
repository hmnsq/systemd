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
#సూచన once

#అన్తర్భావ <byteswap.h>
#అన్తర్భావ <endian.h>

#అన్తర్భావ "forward.h"

#నిర్వచితమైతే __CHECKER__
#నిర్వచించు __sd_bitwise __attribute__((__bitwise__))
#నిర్వచించు __sd_force __attribute__((__force__))
#లేకపోతే
#నిర్వచించు __sd_bitwise
#నిర్వచించు __sd_force
#అయితే_ముగింపు

రకనిర్వచనం uint16_t __sd_bitwise le16_t;
రకనిర్వచనం uint16_t __sd_bitwise be16_t;
రకనిర్వచనం uint32_t __sd_bitwise le32_t;
రకనిర్వచనం uint32_t __sd_bitwise be32_t;
రకనిర్వచనం uint64_t __sd_bitwise le64_t;
రకనిర్వచనం uint64_t __sd_bitwise be64_t;

#నిర్వచనం_రద్దు htobe16
#నిర్వచనం_రద్దు htole16
#నిర్వచనం_రద్దు be16toh
#నిర్వచనం_రద్దు le16toh
#నిర్వచనం_రద్దు htobe32
#నిర్వచనం_రద్దు htole32
#నిర్వచనం_రద్దు be32toh
#నిర్వచనం_రద్దు le32toh
#నిర్వచనం_రద్దు htobe64
#నిర్వచనం_రద్దు htole64
#నిర్వచనం_రద్దు be64toh
#నిర్వచనం_రద్దు le64toh

#ఒకవేళ __BYTE_ORDER == __LITTLE_ENDIAN
#నిర్వచించు bswap_16_on_le(x) bswap_16(x)
#నిర్వచించు bswap_32_on_le(x) bswap_32(x)
#నిర్వచించు bswap_64_on_le(x) bswap_64(x)
#నిర్వచించు bswap_16_on_be(x) (x)
#నిర్వచించు bswap_32_on_be(x) (x)
#నిర్వచించు bswap_64_on_be(x) (x)
#లేదావేళ __BYTE_ORDER == __BIG_ENDIAN
#నిర్వచించు bswap_16_on_le(x) (x)
#నిర్వచించు bswap_32_on_le(x) (x)
#నిర్వచించు bswap_64_on_le(x) (x)
#నిర్వచించు bswap_16_on_be(x) bswap_16(x)
#నిర్వచించు bswap_32_on_be(x) bswap_32(x)
#నిర్వచించు bswap_64_on_be(x) bswap_64(x)
#అయితే_ముగింపు

స్థిర అంతఃవరుస le16_t htole16(uint16_t విలువ) { తిరిగి (le16_t __sd_force) bswap_16_on_be(విలువ); }
స్థిర అంతఃవరుస le32_t htole32(uint32_t విలువ) { తిరిగి (le32_t __sd_force) bswap_32_on_be(విలువ); }
స్థిర అంతఃవరుస le64_t htole64(uint64_t విలువ) { తిరిగి (le64_t __sd_force) bswap_64_on_be(విలువ); }

స్థిర అంతఃవరుస be16_t htobe16(uint16_t విలువ) { తిరిగి (be16_t __sd_force) bswap_16_on_le(విలువ); }
స్థిర అంతఃవరుస be32_t htobe32(uint32_t విలువ) { తిరిగి (be32_t __sd_force) bswap_32_on_le(విలువ); }
స్థిర అంతఃవరుస be64_t htobe64(uint64_t విలువ) { తిరిగి (be64_t __sd_force) bswap_64_on_le(విలువ); }

స్థిర అంతఃవరుస uint16_t le16toh(le16_t విలువ) { తిరిగి bswap_16_on_be((uint16_t __sd_force)విలువ); }
స్థిర అంతఃవరుస uint32_t le32toh(le32_t విలువ) { తిరిగి bswap_32_on_be((uint32_t __sd_force)విలువ); }
స్థిర అంతఃవరుస uint64_t le64toh(le64_t విలువ) { తిరిగి bswap_64_on_be((uint64_t __sd_force)విలువ); }

స్థిర అంతఃవరుస uint16_t be16toh(be16_t విలువ) { తిరిగి bswap_16_on_le((uint16_t __sd_force)విలువ); }
స్థిర అంతఃవరుస uint32_t be32toh(be32_t విలువ) { తిరిగి bswap_32_on_le((uint32_t __sd_force)విలువ); }
స్థిర అంతఃవరుస uint64_t be64toh(be64_t విలువ) { తిరిగి bswap_64_on_le((uint64_t __sd_force)విలువ); }

#నిర్వచనం_రద్దు __sd_bitwise
#నిర్వచనం_రద్దు __sd_force
