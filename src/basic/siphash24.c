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

#ضمن <stdio.h>

#ضمن "iovec-util.h"
#ضمن "siphash24.h"
#ضمن "string-util.h"
#ضمن "unaligned.h"

/* The round is small enough that it is worth inlining at every call site, but large enough that gcc's
 * -O2 inlining budget declines to do so on its own, leaving a call/ret and a reload of the state around
 * six rounds of ~14 ALU ops. clang inlines it either way, and the SipHash reference implementation this
 * file is derived from inlines the round manually, by writing it as a macro. */
ساكن مضمن _always_inline_ uint64_t rotate_يسار(uint64_t x, uint8_t b) {
        أكد(b < 64);

        إرجاع (x << b) | (x >> (64 - b));
}

ساكن مضمن _always_inline_ فراغ sipround(بنية siphash *state) {
        أكد(state);

        state->v0 += state->v1;
        state->v1 = rotate_يسار(state->v1, 13);
        state->v1 ^= state->v0;
        state->v0 = rotate_يسار(state->v0, 32);
        state->v2 += state->v3;
        state->v3 = rotate_يسار(state->v3, 16);
        state->v3 ^= state->v2;
        state->v0 += state->v3;
        state->v3 = rotate_يسار(state->v3, 21);
        state->v3 ^= state->v0;
        state->v2 += state->v1;
        state->v1 = rotate_يسار(state->v1, 17);
        state->v1 ^= state->v2;
        state->v2 = rotate_يسار(state->v2, 32);
}

ساكن مضمن _always_inline_ فراغ siphash24_init_inline(بنية siphash *state, ثابت uint8_t k[ساكن 16]) {
        uint64_t k0, k1;

        أكد(state);
        أكد(k);

        k0 = unaligned_read_le64(k);
        k1 = unaligned_read_le64(k + 8);

        *state = (بنية siphash) {
                /* "somepseudorandomlygeneratedbytes" */
                .v0 = 0x736f6d6570736575ULL ^ k0,
                .v1 = 0x646f72616e646f6dULL ^ k1,
                .v2 = 0x6c7967656e657261ULL ^ k0,
                .v3 = 0x7465646279746573ULL ^ k1,
                .padding = 0,
                .inlen = 0,
        };
}

فراغ siphash24_init(بنية siphash *state, ثابت uint8_t k[ساكن 16]) {
        siphash24_init_inline(state, k);
}

/* The input is read through a byte pointer, which may alias *state. Without restrict, gcc stores the
 * state back to memory after every block. */
ساكن مضمن _always_inline_ فراغ siphash24_compress_inline(
                ثابت فراغ *restrict _in,
                size_t inlen,
                بنية siphash *restrict state) {

        ثابت uint8_t *in = ASSERT_PTR(_in);
        ثابت uint8_t *end = in + inlen;
        size_t يسار = state->inlen & 7;
        uint64_t m;

        أكد(state);

        /* Update total length */
        state->inlen += inlen;

        /* If padding exists, fill it out */
        إذا (يسار > 0) {
                لكل ( ; in < end && يسار < 8; in ++, يسار ++)
                        state->padding |= ((uint64_t) *in) << (يسار * 8);

                إذا (in == end && يسار < 8)
                        /* We did not have enough input to fill out the padding completely */
                        إرجاع;

#إذا ENABLE_DEBUG_SIPHASH
                printf("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                printf("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                printf("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                printf("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                printf("(%3zu) compress padding %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t)state->padding);
#endif

                state->v3 ^= state->padding;
                sipround(state);
                sipround(state);
                state->v0 ^= state->padding;

                state->padding = 0;
        }

        end -= (state->inlen % حجم(uint64_t));

        لكل ( ; in < end; in += 8) {
                m = unaligned_read_le64(in);
#إذا ENABLE_DEBUG_SIPHASH
                printf("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                printf("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                printf("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                printf("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                printf("(%3zu) compress %08x %08x\n", state->inlen, (uint32_t) (m >> 32), (uint32_t) m);
#endif
                state->v3 ^= m;
                sipround(state);
                sipround(state);
                state->v0 ^= m;
        }

        يسار = state->inlen & 7;
        بدل (يسار) {
                حالة 7:
                        state->padding |= ((uint64_t) in[6]) << 48;
                        _fallthrough_;
                حالة 6:
                        state->padding |= ((uint64_t) in[5]) << 40;
                        _fallthrough_;
                حالة 5:
                        state->padding |= ((uint64_t) in[4]) << 32;
                        _fallthrough_;
                حالة 4:
                        state->padding |= ((uint64_t) in[3]) << 24;
                        _fallthrough_;
                حالة 3:
                        state->padding |= ((uint64_t) in[2]) << 16;
                        _fallthrough_;
                حالة 2:
                        state->padding |= ((uint64_t) in[1]) <<  8;
                        _fallthrough_;
                حالة 1:
                        state->padding |= ((uint64_t) in[0]);
                        _fallthrough_;
                حالة 0:
                        اكسر;
        }
}

فراغ siphash24_compress(ثابت فراغ *in, size_t inlen, بنية siphash *state) {
        siphash24_compress_inline(in, inlen, state);
}

فراغ siphash24_compress_string(ثابت حرف *in, بنية siphash *state) {
        siphash24_compress_safe(in, strlen_ptr(in), state);
}

فراغ siphash24_compress_iovec(ثابت بنية iovec *iov, بنية siphash *state) {
        أكد(iovec_is_valid(iov));
        أكد(state);

        إذا (!iovec_is_set(iov))
                إرجاع;

        siphash24_compress(iov->iov_base, iov->iov_len, state);
}

ساكن مضمن _always_inline_ uint64_t siphash24_finalize_inline(بنية siphash *state) {
        uint64_t b;

        أكد(state);

        b = state->padding | (((uint64_t) state->inlen) << 56);

#إذا ENABLE_DEBUG_SIPHASH
        printf("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        printf("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        printf("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        printf("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
        printf("(%3zu) padding   %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t) state->padding);
#endif

        state->v3 ^= b;
        sipround(state);
        sipround(state);
        state->v0 ^= b;

#إذا ENABLE_DEBUG_SIPHASH
        printf("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        printf("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        printf("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        printf("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
#endif
        state->v2 ^= 0xff;

        sipround(state);
        sipround(state);
        sipround(state);
        sipround(state);

        إرجاع state->v0 ^ state->v1 ^ state->v2  ^ state->v3;
}

uint64_t siphash24_finalize(بنية siphash *state) {
        إرجاع siphash24_finalize_inline(state);
}

uint64_t siphash24(ثابت فراغ *in, size_t inlen, ثابت uint8_t k[ساكن 16]) {
        بنية siphash state;

        أكد(in);
        أكد(k);

        /* Calls to the exported functions go through the PLT and keep the state in memory. */
        siphash24_init_inline(&state, k);
        siphash24_compress_inline(in, inlen, &state);

        إرجاع siphash24_finalize_inline(&state);
}

uint64_t siphash24_uint64(uint64_t in, ثابت uint8_t k[ساكن 16]) {
        uint64_t b = UINT64_C(8) << 56;
        بنية siphash state;

        أكد(k);

        /* This returns the same value as siphash24() over the 8 byte little-endian encoding of in. It
         * does not buffer partial input like siphash24_compress(), so gcc keeps the state in registers. */

        siphash24_init_inline(&state, k);

        state.v3 ^= in;
        sipround(&state);
        sipround(&state);
        state.v0 ^= in;

        state.v3 ^= b;
        sipround(&state);
        sipround(&state);
        state.v0 ^= b;

        state.v2 ^= 0xff;
        sipround(&state);
        sipround(&state);
        sipround(&state);
        sipround(&state);

        إرجاع state.v0 ^ state.v1 ^ state.v2 ^ state.v3;
}

uint64_t siphash24_string(ثابت حرف *s, ثابت uint8_t k[ساكن 16]) {
        إرجاع siphash24(s, strlen(s) + 1, k);
}
