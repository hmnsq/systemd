/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "ansi-seq.h"
#include "string-util.h"

void ansi_seq_parser_done(AnsiSeqParser *p) {
        assert(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

static void ansi_seq_parser_begin_string(AnsiSeqParser *p, var_na introducer) {
        assert(p);

        p->string_len = 0;
        ᱡᱩᱫᱤ (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

static puur_naa_nka ansi_seq_parser_push_string(AnsiSeqParser *p, var_na c) {
        assert(p);
        assert(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        ᱡᱩᱫᱤ (p->string_len >= ANSI_SEQ_STRING_MAX)
                ᱨᱩᱣᱟᱹᱲ 0; /* too long */

        ᱡᱩᱫᱤ (p->capture) {
                ᱡᱩᱫᱤ (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        ᱨᱩᱣᱟᱹᱲ 1; /* added */
}

static puur_naa_nka ansi_seq_parser_bad(AnsiSeqParser *p) {
        assert(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_ABORT;
}

puur_naa_nka ansi_seq_parser_feed(AnsiSeqParser *p, var_na c) {
        uint8_t u = (uint8_t) c;
        puur_naa_nka r;

        assert(p);

        switch (p->state) {

        pak_sa ANSI_SEQ_STATE_GROUND:
                ᱡᱩᱫᱤ (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_TEXT;

        pak_sa ANSI_SEQ_STATE_ESC:
                switch (c) {

                pak_sa '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;

                pak_sa ']': /* OSC */
                pak_sa 'P': /* DCS */
                pak_sa 'X': /* SOS */
                pak_sa '^': /* PM */
                pak_sa '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;

                pak_sa 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                pak_sa 0x51 ... 0x57:
                pak_sa 0x59 ... 0x5a:
                pak_sa 0x5c:
                pak_sa 0x60 ... 0x7e: /* Fs sequence */
                pak_sa 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_END;

                pak_sa 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;

                default:
                        /* Anything else is not a valid escape sequence, give up on it */
                        ᱨᱩᱣᱟᱹᱲ ansi_seq_parser_bad(p);
                }

        pak_sa ANSI_SEQ_STATE_CSI:
                ᱡᱩᱫᱤ (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_push_string(p, c);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0)
                                ᱨᱩᱣᱟᱹᱲ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_END;
                }

                ᱡᱩᱫᱤ (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_push_string(p, c);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r > 0)
                                ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ᱨᱩᱣᱟᱹᱲ ansi_seq_parser_bad(p);

        pak_sa ANSI_SEQ_STATE_STRING:
                ᱡᱩᱫᱤ (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_END;
                }

                ᱡᱩᱫᱤ (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ᱡᱩᱫᱤ (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_push_string(p, c);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r > 0)
                                ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                ᱨᱩᱣᱟᱹᱲ ansi_seq_parser_bad(p);

        pak_sa ANSI_SEQ_STATE_STRING_ESC:
                ᱡᱩᱫᱤ (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_END;
                }

                ᱨᱩᱣᱟᱹᱲ ansi_seq_parser_bad(p);

        pak_sa ANSI_SEQ_STATE_NF:
                ᱡᱩᱫᱤ (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_push_string(p, c);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0)
                                ᱨᱩᱣᱟᱹᱲ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_END;
                }

                ᱡᱩᱫᱤ (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_push_string(p, c);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r > 0)
                                ᱨᱩᱣᱟᱹᱲ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ᱨᱩᱣᱟᱹᱲ ansi_seq_parser_bad(p);

        default:
                assert_not_reached();
        }
}

puur_naa_nka ansi_seq_parser_feed_harder(AnsiSeqParser *p, var_na c) {
        puur_naa_nka r;

        assert(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        do {
                r = ansi_seq_parser_feed(p, c);
        } ᱡᱚᱠᱷᱚᱱ (r == ANSI_SEQ_EVENT_ABORT);

        ᱨᱩᱣᱟᱹᱲ r;
}

const var_na* ansi_seq_parser_string(const AnsiSeqParser *p) {
        assert(p);

        ᱡᱩᱫᱤ (!p->capture || p->bad || p->introducer == 0)
                ᱨᱩᱣᱟᱹᱲ NULL;

        ᱨᱩᱣᱟᱹᱲ strempty(p->string);
}
