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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"

/* The round is small enough that it is worth inlining at every call site, but large enough that gcc's
 * -O2 inlining budget declines to do so on its own, leaving a call/ret and a reload of the state around
 * six rounds of ~14 ALU ops. clang inlines it either way, and the SipHash reference implementation this
 * file is derived from inlines the round manually, by writing it as a macro. */
ਸਥਿਰ ਸਤਰਅੰਦਰ _always_inline_ uint64_t rotate_ਖੱਬਾ(uint64_t x, uint8_t b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(b < 64);

        ਵਾਪਸ (x << b) | (x >> (64 - b));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ _always_inline_ ਖਾਲੀ sipround(ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        state->v0 += state->v1;
        state->v1 = rotate_ਖੱਬਾ(state->v1, 13);
        state->v1 ^= state->v0;
        state->v0 = rotate_ਖੱਬਾ(state->v0, 32);
        state->v2 += state->v3;
        state->v3 = rotate_ਖੱਬਾ(state->v3, 16);
        state->v3 ^= state->v2;
        state->v0 += state->v3;
        state->v3 = rotate_ਖੱਬਾ(state->v3, 21);
        state->v3 ^= state->v0;
        state->v2 += state->v1;
        state->v1 = rotate_ਖੱਬਾ(state->v1, 17);
        state->v1 ^= state->v2;
        state->v2 = rotate_ਖੱਬਾ(state->v2, 32);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ _always_inline_ ਖਾਲੀ siphash24_init_inline(ਬਣਤਰ siphash *state, ਅਚਲ uint8_t k[ਸਥਿਰ 16]) {
        uint64_t k0, k1;

        ਪ੍ਰਤਿਜ੍ਞਾ(state);
        ਪ੍ਰਤਿਜ੍ਞਾ(k);

        k0 = unaligned_read_le64(k);
        k1 = unaligned_read_le64(k + 8);

        *state = (ਬਣਤਰ siphash) {
                /* "somepseudorandomlygeneratedbytes" */
                .v0 = 0x736f6d6570736575ULL ^ k0,
                .v1 = 0x646f72616e646f6dULL ^ k1,
                .v2 = 0x6c7967656e657261ULL ^ k0,
                .v3 = 0x7465646279746573ULL ^ k1,
                .padding = 0,
                .inlen = 0,
        };
}

ਖਾਲੀ siphash24_init(ਬਣਤਰ siphash *state, ਅਚਲ uint8_t k[ਸਥਿਰ 16]) {
        siphash24_init_inline(state, k);
}

/* The input is read through a byte pointer, which may alias *state. Without restrict, gcc stores the
 * state back to memory after every block. */
ਸਥਿਰ ਸਤਰਅੰਦਰ _always_inline_ ਖਾਲੀ siphash24_compress_inline(
                ਅਚਲ ਖਾਲੀ *ਸੀਮਿਤ ਵਿੱਚ_2,
                size_t inlen,
                ਬਣਤਰ siphash *ਸੀਮਿਤ state) {

        ਅਚਲ uint8_t *ਵਿੱਚ = ASSERT_PTR(ਵਿੱਚ_2);
        ਅਚਲ uint8_t *ਸਿਰਾ = ਵਿੱਚ + inlen;
        size_t ਖੱਬਾ = state->inlen & 7;
        uint64_t m;

        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        /* Update total length */
        state->inlen += inlen;

        /* If padding exists, fill it out */
        ਜੇ (ਖੱਬਾ > 0) {
                ਲਈ ( ; ਵਿੱਚ < ਸਿਰਾ && ਖੱਬਾ < 8; ਵਿੱਚ ++, ਖੱਬਾ ++)
                        state->padding |= ((uint64_t) *ਵਿੱਚ) << (ਖੱਬਾ * 8);

                ਜੇ (ਵਿੱਚ == ਸਿਰਾ && ਖੱਬਾ < 8)
                        /* We did not have enough input to fill out the padding completely */
                        ਵਾਪਸ;

#ਜੇ ENABLE_DEBUG_SIPHASH
                ਲਿਖੋ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                ਲਿਖੋ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                ਲਿਖੋ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                ਲਿਖੋ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                ਲਿਖੋ("(%3zu) compress padding %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t)state->padding);
#ਜੇ_ਅੰਤ

                state->v3 ^= state->padding;
                sipround(state);
                sipround(state);
                state->v0 ^= state->padding;

                state->padding = 0;
        }

        ਸਿਰਾ -= (state->inlen % ਆਕਾਰ(uint64_t));

        ਲਈ ( ; ਵਿੱਚ < ਸਿਰਾ; ਵਿੱਚ += 8) {
                m = unaligned_read_le64(ਵਿੱਚ);
#ਜੇ ENABLE_DEBUG_SIPHASH
                ਲਿਖੋ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                ਲਿਖੋ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                ਲਿਖੋ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                ਲਿਖੋ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                ਲਿਖੋ("(%3zu) compress %08x %08x\n", state->inlen, (uint32_t) (m >> 32), (uint32_t) m);
#ਜੇ_ਅੰਤ
                state->v3 ^= m;
                sipround(state);
                sipround(state);
                state->v0 ^= m;
        }

        ਖੱਬਾ = state->inlen & 7;
        ਚੋਣ (ਖੱਬਾ) {
                ਹਾਲਤ 7:
                        state->padding |= ((uint64_t) ਵਿੱਚ[6]) << 48;
                        _fallthrough_;
                ਹਾਲਤ 6:
                        state->padding |= ((uint64_t) ਵਿੱਚ[5]) << 40;
                        _fallthrough_;
                ਹਾਲਤ 5:
                        state->padding |= ((uint64_t) ਵਿੱਚ[4]) << 32;
                        _fallthrough_;
                ਹਾਲਤ 4:
                        state->padding |= ((uint64_t) ਵਿੱਚ[3]) << 24;
                        _fallthrough_;
                ਹਾਲਤ 3:
                        state->padding |= ((uint64_t) ਵਿੱਚ[2]) << 16;
                        _fallthrough_;
                ਹਾਲਤ 2:
                        state->padding |= ((uint64_t) ਵਿੱਚ[1]) <<  8;
                        _fallthrough_;
                ਹਾਲਤ 1:
                        state->padding |= ((uint64_t) ਵਿੱਚ[0]);
                        _fallthrough_;
                ਹਾਲਤ 0:
                        ਰੋਕੋ;
        }
}

ਖਾਲੀ siphash24_compress(ਅਚਲ ਖਾਲੀ *ਵਿੱਚ, size_t inlen, ਬਣਤਰ siphash *state) {
        siphash24_compress_inline(ਵਿੱਚ, inlen, state);
}

ਖਾਲੀ siphash24_compress_string(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ, ਬਣਤਰ siphash *state) {
        siphash24_compress_ਸੁਰੱਖਿਅਤ(ਵਿੱਚ, strlen_ptr(ਵਿੱਚ), state);
}

ਖਾਲੀ siphash24_compress_iovec(ਅਚਲ ਬਣਤਰ iovec *iov, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec_is_valid(iov));
        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        ਜੇ (!iovec_is_ਸੈੱਟ(iov))
                ਵਾਪਸ;

        siphash24_compress(iov->iov_base, iov->iov_len, state);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ _always_inline_ uint64_t siphash24_finalize_inline(ਬਣਤਰ siphash *state) {
        uint64_t b;

        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        b = state->padding | (((uint64_t) state->inlen) << 56);

#ਜੇ ENABLE_DEBUG_SIPHASH
        ਲਿਖੋ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        ਲਿਖੋ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        ਲਿਖੋ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        ਲਿਖੋ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
        ਲਿਖੋ("(%3zu) padding   %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t) state->padding);
#ਜੇ_ਅੰਤ

        state->v3 ^= b;
        sipround(state);
        sipround(state);
        state->v0 ^= b;

#ਜੇ ENABLE_DEBUG_SIPHASH
        ਲਿਖੋ("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        ਲਿਖੋ("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        ਲਿਖੋ("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        ਲਿਖੋ("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
#ਜੇ_ਅੰਤ
        state->v2 ^= 0xff;

        sipround(state);
        sipround(state);
        sipround(state);
        sipround(state);

        ਵਾਪਸ state->v0 ^ state->v1 ^ state->v2  ^ state->v3;
}

uint64_t siphash24_finalize(ਬਣਤਰ siphash *state) {
        ਵਾਪਸ siphash24_finalize_inline(state);
}

uint64_t siphash24(ਅਚਲ ਖਾਲੀ *ਵਿੱਚ, size_t inlen, ਅਚਲ uint8_t k[ਸਥਿਰ 16]) {
        ਬਣਤਰ siphash state;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ);
        ਪ੍ਰਤਿਜ੍ਞਾ(k);

        /* Calls to the exported functions go through the PLT and keep the state in memory. */
        siphash24_init_inline(&state, k);
        siphash24_compress_inline(ਵਿੱਚ, inlen, &state);

        ਵਾਪਸ siphash24_finalize_inline(&state);
}

uint64_t siphash24_uint64(uint64_t ਵਿੱਚ, ਅਚਲ uint8_t k[ਸਥਿਰ 16]) {
        uint64_t b = UINT64_C(8) << 56;
        ਬਣਤਰ siphash state;

        ਪ੍ਰਤਿਜ੍ਞਾ(k);

        /* This returns the same value as siphash24() over the 8 byte little-endian encoding of in. It
         * does not buffer partial input like siphash24_compress(), so gcc keeps the state in registers. */

        siphash24_init_inline(&state, k);

        state.v3 ^= ਵਿੱਚ;
        sipround(&state);
        sipround(&state);
        state.v0 ^= ਵਿੱਚ;

        state.v3 ^= b;
        sipround(&state);
        sipround(&state);
        state.v0 ^= b;

        state.v2 ^= 0xff;
        sipround(&state);
        sipround(&state);
        sipround(&state);
        sipround(&state);

        ਵਾਪਸ state.v0 ^ state.v1 ^ state.v2 ^ state.v3;
}

uint64_t siphash24_string(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ uint8_t k[ਸਥਿਰ 16]) {
        ਵਾਪਸ siphash24(s, ਲੜੀ_ਲੰਬਾਈ(s) + 1, k);
}
