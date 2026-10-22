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
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <byteswap.h>
#ಅನ್ತರ್ಭಾವ <endian.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CHECKER__
#ವ್ಯಾಖ್ಯಾನಿಸು __sd_bitwise __attribute__((__bitwise__))
#ವ್ಯಾಖ್ಯಾನಿಸು __sd_force __attribute__((__force__))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು __sd_bitwise
#ವ್ಯಾಖ್ಯಾನಿಸು __sd_force
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint16_t __sd_bitwise le16_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint16_t __sd_bitwise be16_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t __sd_bitwise le32_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t __sd_bitwise be32_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t __sd_bitwise le64_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t __sd_bitwise be64_t;

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು htobe16
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು htole16
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು be16toh
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು le16toh
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು htobe32
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು htole32
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು be32toh
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು le32toh
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು htobe64
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು htole64
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು be64toh
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು le64toh

#ಆದರೆ __BYTE_ORDER == __LITTLE_ENDIAN
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_16_on_le(x) bswap_16(x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_32_on_le(x) bswap_32(x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_64_on_le(x) bswap_64(x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_16_on_be(x) (x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_32_on_be(x) (x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_64_on_be(x) (x)
#ಅಥವಾಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_16_on_le(x) (x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_32_on_le(x) (x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_64_on_le(x) (x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_16_on_be(x) bswap_16(x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_32_on_be(x) bswap_32(x)
#ವ್ಯಾಖ್ಯಾನಿಸು bswap_64_on_be(x) bswap_64(x)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ le16_t htole16(uint16_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ (le16_t __sd_force) bswap_16_on_be(ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ le32_t htole32(uint32_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ (le32_t __sd_force) bswap_32_on_be(ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ le64_t htole64(uint64_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ (le64_t __sd_force) bswap_64_on_be(ಮೌಲ್ಯ); }

ಸ್ಥಿರ ಸಾಲೊಳಗೆ be16_t htobe16(uint16_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ (be16_t __sd_force) bswap_16_on_le(ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ be32_t htobe32(uint32_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ (be32_t __sd_force) bswap_32_on_le(ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ be64_t htobe64(uint64_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ (be64_t __sd_force) bswap_64_on_le(ಮೌಲ್ಯ); }

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint16_t le16toh(le16_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ bswap_16_on_be((uint16_t __sd_force)ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t le32toh(le32_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ bswap_32_on_be((uint32_t __sd_force)ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t le64toh(le64_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ bswap_64_on_be((uint64_t __sd_force)ಮೌಲ್ಯ); }

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint16_t be16toh(be16_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ bswap_16_on_le((uint16_t __sd_force)ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t be32toh(be32_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ bswap_32_on_le((uint32_t __sd_force)ಮೌಲ್ಯ); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t be64toh(be64_t ಮೌಲ್ಯ) { ಹಿಂತಿರುಗಿ bswap_64_on_le((uint64_t __sd_force)ಮೌಲ್ಯ); }

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __sd_bitwise
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __sd_force
