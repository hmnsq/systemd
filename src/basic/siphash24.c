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

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unaligned.h"

/* The round is small enough that it is worth inlining at every call site, but large enough that gcc's
 * -O2 inlining budget declines to do so on its own, leaving a call/ret and a reload of the state around
 * six rounds of ~14 ALU ops. clang inlines it either way, and the SipHash reference implementation this
 * file is derived from inlines the round manually, by writing it as a macro. */
स्थिर अन्तःस्थ _always_inline_ uint64_t rotate_वामः(uint64_t x, uint8_t b) {
        प्रतिज्ञा(b < 64);

        प्रत्यागम (x << b) | (x >> (64 - b));
}

स्थिर अन्तःस्थ _always_inline_ शून्य sipround(संरचना siphash *state) {
        प्रतिज्ञा(state);

        state->v0 += state->v1;
        state->v1 = rotate_वामः(state->v1, 13);
        state->v1 ^= state->v0;
        state->v0 = rotate_वामः(state->v0, 32);
        state->v2 += state->v3;
        state->v3 = rotate_वामः(state->v3, 16);
        state->v3 ^= state->v2;
        state->v0 += state->v3;
        state->v3 = rotate_वामः(state->v3, 21);
        state->v3 ^= state->v0;
        state->v2 += state->v1;
        state->v1 = rotate_वामः(state->v1, 17);
        state->v1 ^= state->v2;
        state->v2 = rotate_वामः(state->v2, 32);
}

स्थिर अन्तःस्थ _always_inline_ शून्य siphash24_init_inline(संरचना siphash *state, ध्रुव uint8_t k[स्थिर 16]) {
        uint64_t k0, k1;

        प्रतिज्ञा(state);
        प्रतिज्ञा(k);

        k0 = unaligned_read_le64(k);
        k1 = unaligned_read_le64(k + 8);

        *state = (संरचना siphash) {
                /* "somepseudorandomlygeneratedbytes" */
                .v0 = 0x736f6d6570736575ULL ^ k0,
                .v1 = 0x646f72616e646f6dULL ^ k1,
                .v2 = 0x6c7967656e657261ULL ^ k0,
                .v3 = 0x7465646279746573ULL ^ k1,
                .padding = 0,
                .inlen = 0,
        };
}

शून्य siphash24_init(संरचना siphash *state, ध्रुव uint8_t k[स्थिर 16]) {
        siphash24_init_inline(state, k);
}

/* The input is read through a byte pointer, which may alias *state. Without restrict, gcc stores the
 * state back to memory after every block. */
स्थिर अन्तःस्थ _always_inline_ शून्य siphash24_compress_inline(
                ध्रुव शून्य *सीमित अन्तर्_2,
                size_t inlen,
                संरचना siphash *सीमित state) {

        ध्रुव uint8_t *अन्तर् = ASSERT_PTR(अन्तर्_2);
        ध्रुव uint8_t *अन्तः = अन्तर् + inlen;
        size_t वामः = state->inlen & 7;
        uint64_t m;

        प्रतिज्ञा(state);

        /* Update total length */
        state->inlen += inlen;

        /* If padding exists, fill it out */
        यदि (वामः > 0) {
                प्रति ( ; अन्तर् < अन्तः && वामः < 8; अन्तर् ++, वामः ++)
                        state->padding |= ((uint64_t) *अन्तर्) << (वामः * 8);

                यदि (अन्तर् == अन्तः && वामः < 8)
                        /* We did not have enough input to fill out the padding completely */
                        प्रत्यागम;

#यदि ENABLE_DEBUG_SIPHASH
                प्रारूप_लिख("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                प्रारूप_लिख("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                प्रारूप_लिख("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                प्रारूप_लिख("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                प्रारूप_लिख("(%3zu) compress padding %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t)state->padding);
#यदि_अन्तः

                state->v3 ^= state->padding;
                sipround(state);
                sipround(state);
                state->v0 ^= state->padding;

                state->padding = 0;
        }

        अन्तः -= (state->inlen % परिमाण(uint64_t));

        प्रति ( ; अन्तर् < अन्तः; अन्तर् += 8) {
                m = unaligned_read_le64(अन्तर्);
#यदि ENABLE_DEBUG_SIPHASH
                प्रारूप_लिख("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
                प्रारूप_लिख("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
                प्रारूप_लिख("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
                प्रारूप_लिख("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
                प्रारूप_लिख("(%3zu) compress %08x %08x\n", state->inlen, (uint32_t) (m >> 32), (uint32_t) m);
#यदि_अन्तः
                state->v3 ^= m;
                sipround(state);
                sipround(state);
                state->v0 ^= m;
        }

        वामः = state->inlen & 7;
        विकल्प (वामः) {
                पक्ष 7:
                        state->padding |= ((uint64_t) अन्तर्[6]) << 48;
                        _fallthrough_;
                पक्ष 6:
                        state->padding |= ((uint64_t) अन्तर्[5]) << 40;
                        _fallthrough_;
                पक्ष 5:
                        state->padding |= ((uint64_t) अन्तर्[4]) << 32;
                        _fallthrough_;
                पक्ष 4:
                        state->padding |= ((uint64_t) अन्तर्[3]) << 24;
                        _fallthrough_;
                पक्ष 3:
                        state->padding |= ((uint64_t) अन्तर्[2]) << 16;
                        _fallthrough_;
                पक्ष 2:
                        state->padding |= ((uint64_t) अन्तर्[1]) <<  8;
                        _fallthrough_;
                पक्ष 1:
                        state->padding |= ((uint64_t) अन्तर्[0]);
                        _fallthrough_;
                पक्ष 0:
                        भङ्ग;
        }
}

शून्य siphash24_compress(ध्रुव शून्य *अन्तर्, size_t inlen, संरचना siphash *state) {
        siphash24_compress_inline(अन्तर्, inlen, state);
}

शून्य siphash24_compress_string(ध्रुव वर्ण *अन्तर्, संरचना siphash *state) {
        siphash24_compress_safe(अन्तर्, strlen_ptr(अन्तर्), state);
}

शून्य siphash24_compress_iovec(ध्रुव संरचना iovec *iov, संरचना siphash *state) {
        प्रतिज्ञा(iovec_is_valid(iov));
        प्रतिज्ञा(state);

        यदि (!iovec_is_समुच्चयः(iov))
                प्रत्यागम;

        siphash24_compress(iov->iov_base, iov->iov_len, state);
}

स्थिर अन्तःस्थ _always_inline_ uint64_t siphash24_finalize_inline(संरचना siphash *state) {
        uint64_t b;

        प्रतिज्ञा(state);

        b = state->padding | (((uint64_t) state->inlen) << 56);

#यदि ENABLE_DEBUG_SIPHASH
        प्रारूप_लिख("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        प्रारूप_लिख("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        प्रारूप_लिख("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        प्रारूप_लिख("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
        प्रारूप_लिख("(%3zu) padding   %08x %08x\n", state->inlen, (uint32_t) (state->padding >> 32), (uint32_t) state->padding);
#यदि_अन्तः

        state->v3 ^= b;
        sipround(state);
        sipround(state);
        state->v0 ^= b;

#यदि ENABLE_DEBUG_SIPHASH
        प्रारूप_लिख("(%3zu) v0 %08x %08x\n", state->inlen, (uint32_t) (state->v0 >> 32), (uint32_t) state->v0);
        प्रारूप_लिख("(%3zu) v1 %08x %08x\n", state->inlen, (uint32_t) (state->v1 >> 32), (uint32_t) state->v1);
        प्रारूप_लिख("(%3zu) v2 %08x %08x\n", state->inlen, (uint32_t) (state->v2 >> 32), (uint32_t) state->v2);
        प्रारूप_लिख("(%3zu) v3 %08x %08x\n", state->inlen, (uint32_t) (state->v3 >> 32), (uint32_t) state->v3);
#यदि_अन्तः
        state->v2 ^= 0xff;

        sipround(state);
        sipround(state);
        sipround(state);
        sipround(state);

        प्रत्यागम state->v0 ^ state->v1 ^ state->v2  ^ state->v3;
}

uint64_t siphash24_finalize(संरचना siphash *state) {
        प्रत्यागम siphash24_finalize_inline(state);
}

uint64_t siphash24(ध्रुव शून्य *अन्तर्, size_t inlen, ध्रुव uint8_t k[स्थिर 16]) {
        संरचना siphash state;

        प्रतिज्ञा(अन्तर्);
        प्रतिज्ञा(k);

        /* Calls to the exported functions go through the PLT and keep the state in memory. */
        siphash24_init_inline(&state, k);
        siphash24_compress_inline(अन्तर्, inlen, &state);

        प्रत्यागम siphash24_finalize_inline(&state);
}

uint64_t siphash24_uint64(uint64_t अन्तर्, ध्रुव uint8_t k[स्थिर 16]) {
        uint64_t b = UINT64_C(8) << 56;
        संरचना siphash state;

        प्रतिज्ञा(k);

        /* This returns the same value as siphash24() over the 8 byte little-endian encoding of in. It
         * does not buffer partial input like siphash24_compress(), so gcc keeps the state in registers. */

        siphash24_init_inline(&state, k);

        state.v3 ^= अन्तर्;
        sipround(&state);
        sipround(&state);
        state.v0 ^= अन्तर्;

        state.v3 ^= b;
        sipround(&state);
        sipround(&state);
        state.v0 ^= b;

        state.v2 ^= 0xff;
        sipround(&state);
        sipround(&state);
        sipround(&state);
        sipround(&state);

        प्रत्यागम state.v0 ^ state.v1 ^ state.v2 ^ state.v3;
}

uint64_t siphash24_string(ध्रुव वर्ण *s, ध्रुव uint8_t k[स्थिर 16]) {
        प्रत्यागम siphash24(s, वाक्यदैर्घ्यम्(s) + 1, k);
}
