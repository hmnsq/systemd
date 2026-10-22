/* SPDX-License-Identifier: CC0-1.0 */

/*
   SipHash reference C implementation

   Written in 2012 by
   Jean-Philippe Aumasson <jeanphilippe.aumasson@gmail.com>
   Daniel J. Bernstein <djb@cr.yp.to>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <https://creativecommons.org/publicdomain/zero/1.0/>.

   (Minimal changes made by Lennart Poettering, to make clean for inclusion in systemd)
   (Refactored by Tom Gundersen to split up in several functions and follow systemd
    coding style)
*/

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

/* The round is small enough that it is worth inlining at every call site, but large enough that gcc's
 * -O2 inlining budget declines to do so on its own, leaving a call/ret and a reload of the state around
 * six rounds of ~14 ALU ops. clang inlines it either way, and the SipHash reference implementation this
 * file is derived from inlines the round manually, by writing it as a macro. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ _always_inline_ uint64_t rotate_ಎಡ(uint64_t x, uint8_t b) {
        ಪ್ರತಿಜ್ಞಾ(b < 64);

        ಹಿಂತಿರುಗಿ (x << b) | (x >> (64 - b));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ _always_inline_ ಶೂನ್ಯ sipround(ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(state);

        state->v0 += state->v1;
        state->v1 = rotate_ಎಡ(state->v1, 13);
        state->v1 ^= state->v0;
        state->v0 = rotate_ಎಡ(state->v0, 32);
        state->v2 += state->v3;
        state->v3 = rotate_ಎಡ(state->v3, 16);
        state->v3 ^= state->v2;
        state->v0 += state->v3;
        state->v3 = rotate_ಎಡ(state->v3, 21);
        state->v3 ^= state->v0;
        state->v2 += state->v1;
        state->v1 = rotate_ಎಡ(state->v1, 17);
        state->v1 ^= state->v2;
        state->v2 = rotate_ಎಡ(state->v2, 32);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ _always_inline_ ಶೂನ್ಯ siphash24_init_inline(ರಚನೆ siphash *state, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]) {
        uint64_t k0, k1;

        ಪ್ರತಿಜ್ಞಾ(state);
        ಪ್ರತಿಜ್ಞಾ(k);

        k0 = unaligned_read_le64(k);
        k1 = unaligned_read_le64(k + 8);

        *state = (ರಚನೆ siphash) {
                /* "somepseudorandomlygeneratedbytes" */
                .v0 = 0x736f6d6570736575ULL ^ k0,
                .v1 = 0x646f72616e646f6dULL ^ k1,
                .v2 = 0x6c7967656e657261ULL ^ k0,
                .v3 = 0x7465646279746573ULL ^ k1,
                .padding = 0,
                .inlen = 0,
        };
}

ಶೂನ್ಯ siphash24_init(ರಚನೆ siphash *state, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]) {
        siphash24_init_inline(state, k);
}

/* The input is read through a byte pointer, which may alias *state. Without restrict, gcc stores the
 * state back to memory after every block. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ _always_inline_ ಶೂನ್ಯ siphash24_compress_inline(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಸೀಮಿತ ಒಳ_2,
                size_t inlen,
                ರಚನೆ siphash *ಸೀಮಿತ state) {

        ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ = ASSERT_PTR(ಒಳ_2);
        ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ = ಒಳ + inlen;
        size_t ಎಡ = state->inlen & 7;
        uint64_t m;

        ಪ್ರತಿಜ್ಞಾ(state);

        /* Update total length */
        state->inlen += inlen;

        /* If padding exists, fill it out */
        ಆದರೆ (ಎಡ > 0) {
                ಗಾಗಿ ( ; ಒಳ < ಕೊನೆ && ಎಡ < 8; ಒಳ ++, ಎಡ ++)
                        state->padding |= ((uint64_t) *ಒಳ) << (ಎಡ * 8);

                ಆದರೆ (ಒಳ == ಕೊನೆ && ಎಡ < 8)
                        /* We did not have enough input to fill out the padding completely */
                        ಹಿಂತಿರುಗಿ;

#ಆದರೆ ENABLE_DEBUG_SIPHASH
                ಬರೆ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                ಬರೆ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                ಬರೆ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                ಬರೆ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                ಬರೆ("(%3zu) compress padding %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t)state->padding);
#ಆದರೆ_ಅಂತ್ಯ

                state->v3 ^= state->padding;
                sipround(state);
                sipround(state);
                state->v0 ^= state->padding;

                state->padding = 0;
        }

        ಕೊನೆ -= (state->inlen % ಗಾತ್ರ(uint64_t));

        ಗಾಗಿ ( ; ಒಳ < ಕೊನೆ; ಒಳ += 8) {
                m = unaligned_read_le64(ಒಳ);
#ಆದರೆ ENABLE_DEBUG_SIPHASH
                ಬರೆ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                ಬರೆ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                ಬರೆ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                ಬರೆ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                ಬರೆ("(%3zu) compress %08x %08x\n", state->inlen, (uint32_t) (m >> 32), (uint32_t) m);
#ಆದರೆ_ಅಂತ್ಯ
                state->v3 ^= m;
                sipround(state);
                sipround(state);
                state->v0 ^= m;
        }

        ಎಡ = state->inlen & 7;
        ಆಯ್ಕೆ (ಎಡ) {
                ಸ್ಥಿತಿ 7:
                        state->padding |= ((uint64_t) ಒಳ[6]) << 48;
                        _fallthrough_;
                ಸ್ಥಿತಿ 6:
                        state->padding |= ((uint64_t) ಒಳ[5]) << 40;
                        _fallthrough_;
                ಸ್ಥಿತಿ 5:
                        state->padding |= ((uint64_t) ಒಳ[4]) << 32;
                        _fallthrough_;
                ಸ್ಥಿತಿ 4:
                        state->padding |= ((uint64_t) ಒಳ[3]) << 24;
                        _fallthrough_;
                ಸ್ಥಿತಿ 3:
                        state->padding |= ((uint64_t) ಒಳ[2]) << 16;
                        _fallthrough_;
                ಸ್ಥಿತಿ 2:
                        state->padding |= ((uint64_t) ಒಳ[1]) <<  8;
                        _fallthrough_;
                ಸ್ಥಿತಿ 1:
                        state->padding |= ((uint64_t) ಒಳ[0]);
                        _fallthrough_;
                ಸ್ಥಿತಿ 0:
                        ನಿಲ್ಲಿಸು;
        }
}

ಶೂನ್ಯ siphash24_compress(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ, size_t inlen, ರಚನೆ siphash *state) {
        siphash24_compress_inline(ಒಳ, inlen, state);
}

ಶೂನ್ಯ siphash24_compress_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ, ರಚನೆ siphash *state) {
        siphash24_compress_ಸುರಕ್ಷಿತ(ಒಳ, strlen_ptr(ಒಳ), state);
}

ಶೂನ್ಯ siphash24_compress_iovec(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(iovec_is_valid(iov));
        ಪ್ರತಿಜ್ಞಾ(state);

        ಆದರೆ (!iovec_is_ಗಣ(iov))
                ಹಿಂತಿರುಗಿ;

        siphash24_compress(iov->iov_base, iov->iov_len, state);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ _always_inline_ uint64_t siphash24_finalize_inline(ರಚನೆ siphash *state) {
        uint64_t b;

        ಪ್ರತಿಜ್ಞಾ(state);

        b = state->padding | (((uint64_t) state->inlen) << 56);

#ಆದರೆ ENABLE_DEBUG_SIPHASH
        ಬರೆ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        ಬರೆ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        ಬರೆ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        ಬರೆ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
        ಬರೆ("(%3zu) padding   %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t) state->padding);
#ಆದರೆ_ಅಂತ್ಯ

        state->v3 ^= b;
        sipround(state);
        sipround(state);
        state->v0 ^= b;

#ಆದರೆ ENABLE_DEBUG_SIPHASH
        ಬರೆ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        ಬರೆ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        ಬರೆ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        ಬರೆ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
#ಆದರೆ_ಅಂತ್ಯ
        state->v2 ^= 0xff;

        sipround(state);
        sipround(state);
        sipround(state);
        sipround(state);

        ಹಿಂತಿರುಗಿ state->v0 ^ state->v1 ^ state->v2  ^ state->v3;
}

uint64_t siphash24_finalize(ರಚನೆ siphash *state) {
        ಹಿಂತಿರುಗಿ siphash24_finalize_inline(state);
}

uint64_t siphash24(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ, size_t inlen, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]) {
        ರಚನೆ siphash state;

        ಪ್ರತಿಜ್ಞಾ(ಒಳ);
        ಪ್ರತಿಜ್ಞಾ(k);

        /* Calls to the exported functions go through the PLT and keep the state in memory. */
        siphash24_init_inline(&state, k);
        siphash24_compress_inline(ಒಳ, inlen, &state);

        ಹಿಂತಿರುಗಿ siphash24_finalize_inline(&state);
}

uint64_t siphash24_uint64(uint64_t ಒಳ, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]) {
        uint64_t b = UINT64_C(8) << 56;
        ರಚನೆ siphash state;

        ಪ್ರತಿಜ್ಞಾ(k);

        /* This returns the same value as siphash24() over the 8 byte little-endian encoding of in. It
         * does not buffer partial input like siphash24_compress(), so gcc keeps the state in registers. */

        siphash24_init_inline(&state, k);

        state.v3 ^= ಒಳ;
        sipround(&state);
        sipround(&state);
        state.v0 ^= ಒಳ;

        state.v3 ^= b;
        sipround(&state);
        sipround(&state);
        state.v0 ^= b;

        state.v2 ^= 0xff;
        sipround(&state);
        sipround(&state);
        sipround(&state);
        sipround(&state);

        ಹಿಂತಿರುಗಿ state.v0 ^ state.v1 ^ state.v2 ^ state.v3;
}

uint64_t siphash24_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ uint8_t k[ಸ್ಥಿರ 16]) {
        ಹಿಂತಿರುಗಿ siphash24(s, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) + 1, k);
}
