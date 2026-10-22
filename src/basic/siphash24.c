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

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "unaligned.h"

/* The round is small enough that it is worth inlining at every call site, but large enough that gcc's
 * -O2 inlining budget declines to do so on its own, leaving a call/ret and a reload of the state around
 * six rounds of ~14 ALU ops. clang inlines it either way, and the SipHash reference implementation this
 * file is derived from inlines the round manually, by writing it as a macro. */
స్థిర అంతఃవరుస _always_inline_ uint64_t rotate_ఎడమ(uint64_t x, uint8_t b) {
        ప్రతిజ్ఞా(b < 64);

        తిరిగి (x << b) | (x >> (64 - b));
}

స్థిర అంతఃవరుస _always_inline_ శూన్య sipround(నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(state);

        state->v0 += state->v1;
        state->v1 = rotate_ఎడమ(state->v1, 13);
        state->v1 ^= state->v0;
        state->v0 = rotate_ఎడమ(state->v0, 32);
        state->v2 += state->v3;
        state->v3 = rotate_ఎడమ(state->v3, 16);
        state->v3 ^= state->v2;
        state->v0 += state->v3;
        state->v3 = rotate_ఎడమ(state->v3, 21);
        state->v3 ^= state->v0;
        state->v2 += state->v1;
        state->v1 = rotate_ఎడమ(state->v1, 17);
        state->v1 ^= state->v2;
        state->v2 = rotate_ఎడమ(state->v2, 32);
}

స్థిర అంతఃవరుస _always_inline_ శూన్య siphash24_init_inline(నిర్మాణం siphash *state, స్థిరరాశి uint8_t k[స్థిర 16]) {
        uint64_t k0, k1;

        ప్రతిజ్ఞా(state);
        ప్రతిజ్ఞా(k);

        k0 = unaligned_read_le64(k);
        k1 = unaligned_read_le64(k + 8);

        *state = (నిర్మాణం siphash) {
                /* "somepseudorandomlygeneratedbytes" */
                .v0 = 0x736f6d6570736575ULL ^ k0,
                .v1 = 0x646f72616e646f6dULL ^ k1,
                .v2 = 0x6c7967656e657261ULL ^ k0,
                .v3 = 0x7465646279746573ULL ^ k1,
                .padding = 0,
                .inlen = 0,
        };
}

శూన్య siphash24_init(నిర్మాణం siphash *state, స్థిరరాశి uint8_t k[స్థిర 16]) {
        siphash24_init_inline(state, k);
}

/* The input is read through a byte pointer, which may alias *state. Without restrict, gcc stores the
 * state back to memory after every block. */
స్థిర అంతఃవరుస _always_inline_ శూన్య siphash24_compress_inline(
                స్థిరరాశి శూన్య *పరిమిత లోపల_2,
                size_t inlen,
                నిర్మాణం siphash *పరిమిత state) {

        స్థిరరాశి uint8_t *లోపల = ASSERT_PTR(లోపల_2);
        స్థిరరాశి uint8_t *చివర = లోపల + inlen;
        size_t ఎడమ = state->inlen & 7;
        uint64_t m;

        ప్రతిజ్ఞా(state);

        /* Update total length */
        state->inlen += inlen;

        /* If padding exists, fill it out */
        ఒకవేళ (ఎడమ > 0) {
                కొరకు ( ; లోపల < చివర && ఎడమ < 8; లోపల ++, ఎడమ ++)
                        state->padding |= ((uint64_t) *లోపల) << (ఎడమ * 8);

                ఒకవేళ (లోపల == చివర && ఎడమ < 8)
                        /* We did not have enough input to fill out the padding completely */
                        తిరిగి;

#ఒకవేళ ENABLE_DEBUG_SIPHASH
                రాయి("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                రాయి("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                రాయి("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                రాయి("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                రాయి("(%3zu) compress padding %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t)state->padding);
#అయితే_ముగింపు

                state->v3 ^= state->padding;
                sipround(state);
                sipround(state);
                state->v0 ^= state->padding;

                state->padding = 0;
        }

        చివర -= (state->inlen % పరిమాణం(uint64_t));

        కొరకు ( ; లోపల < చివర; లోపల += 8) {
                m = unaligned_read_le64(లోపల);
#ఒకవేళ ENABLE_DEBUG_SIPHASH
                రాయి("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                రాయి("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                రాయి("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                రాయి("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                రాయి("(%3zu) compress %08x %08x\n", state->inlen, (uint32_t) (m >> 32), (uint32_t) m);
#అయితే_ముగింపు
                state->v3 ^= m;
                sipround(state);
                sipround(state);
                state->v0 ^= m;
        }

        ఎడమ = state->inlen & 7;
        ఎంపిక (ఎడమ) {
                స్థితి 7:
                        state->padding |= ((uint64_t) లోపల[6]) << 48;
                        _fallthrough_;
                స్థితి 6:
                        state->padding |= ((uint64_t) లోపల[5]) << 40;
                        _fallthrough_;
                స్థితి 5:
                        state->padding |= ((uint64_t) లోపల[4]) << 32;
                        _fallthrough_;
                స్థితి 4:
                        state->padding |= ((uint64_t) లోపల[3]) << 24;
                        _fallthrough_;
                స్థితి 3:
                        state->padding |= ((uint64_t) లోపల[2]) << 16;
                        _fallthrough_;
                స్థితి 2:
                        state->padding |= ((uint64_t) లోపల[1]) <<  8;
                        _fallthrough_;
                స్థితి 1:
                        state->padding |= ((uint64_t) లోపల[0]);
                        _fallthrough_;
                స్థితి 0:
                        ఆపు;
        }
}

శూన్య siphash24_compress(స్థిరరాశి శూన్య *లోపల, size_t inlen, నిర్మాణం siphash *state) {
        siphash24_compress_inline(లోపల, inlen, state);
}

శూన్య siphash24_compress_string(స్థిరరాశి అక్షరం *లోపల, నిర్మాణం siphash *state) {
        siphash24_compress_సురక్షిత(లోపల, strlen_ptr(లోపల), state);
}

శూన్య siphash24_compress_iovec(స్థిరరాశి నిర్మాణం iovec *iov, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(iovec_is_valid(iov));
        ప్రతిజ్ఞా(state);

        ఒకవేళ (!iovec_is_సమితి(iov))
                తిరిగి;

        siphash24_compress(iov->iov_base, iov->iov_len, state);
}

స్థిర అంతఃవరుస _always_inline_ uint64_t siphash24_finalize_inline(నిర్మాణం siphash *state) {
        uint64_t b;

        ప్రతిజ్ఞా(state);

        b = state->padding | (((uint64_t) state->inlen) << 56);

#ఒకవేళ ENABLE_DEBUG_SIPHASH
        రాయి("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        రాయి("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        రాయి("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        రాయి("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
        రాయి("(%3zu) padding   %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t) state->padding);
#అయితే_ముగింపు

        state->v3 ^= b;
        sipround(state);
        sipround(state);
        state->v0 ^= b;

#ఒకవేళ ENABLE_DEBUG_SIPHASH
        రాయి("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        రాయి("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        రాయి("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        రాయి("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
#అయితే_ముగింపు
        state->v2 ^= 0xff;

        sipround(state);
        sipround(state);
        sipround(state);
        sipround(state);

        తిరిగి state->v0 ^ state->v1 ^ state->v2  ^ state->v3;
}

uint64_t siphash24_finalize(నిర్మాణం siphash *state) {
        తిరిగి siphash24_finalize_inline(state);
}

uint64_t siphash24(స్థిరరాశి శూన్య *లోపల, size_t inlen, స్థిరరాశి uint8_t k[స్థిర 16]) {
        నిర్మాణం siphash state;

        ప్రతిజ్ఞా(లోపల);
        ప్రతిజ్ఞా(k);

        /* Calls to the exported functions go through the PLT and keep the state in memory. */
        siphash24_init_inline(&state, k);
        siphash24_compress_inline(లోపల, inlen, &state);

        తిరిగి siphash24_finalize_inline(&state);
}

uint64_t siphash24_uint64(uint64_t లోపల, స్థిరరాశి uint8_t k[స్థిర 16]) {
        uint64_t b = UINT64_C(8) << 56;
        నిర్మాణం siphash state;

        ప్రతిజ్ఞా(k);

        /* This returns the same value as siphash24() over the 8 byte little-endian encoding of in. It
         * does not buffer partial input like siphash24_compress(), so gcc keeps the state in registers. */

        siphash24_init_inline(&state, k);

        state.v3 ^= లోపల;
        sipround(&state);
        sipround(&state);
        state.v0 ^= లోపల;

        state.v3 ^= b;
        sipround(&state);
        sipround(&state);
        state.v0 ^= b;

        state.v2 ^= 0xff;
        sipround(&state);
        sipround(&state);
        sipround(&state);
        sipround(&state);

        తిరిగి state.v0 ^ state.v1 ^ state.v2 ^ state.v3;
}

uint64_t siphash24_string(స్థిరరాశి అక్షరం *s, స్థిరరాశి uint8_t k[స్థిర 16]) {
        తిరిగి siphash24(s, అక్షరశ్రేణి_పొడవు(s) + 1, k);
}
