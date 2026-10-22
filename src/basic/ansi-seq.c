/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "ansi-seq.h"
#include "string-util.h"

void ansi_seq_parser_done(AnsiSeqParser *p) {
        assert(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

static void ansi_seq_parser_begin_string(AnsiSeqParser *p, char introducer) {
        assert(p);

        p->string_len = 0;
        nếu (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

static số_nguyên ansi_seq_parser_push_string(AnsiSeqParser *p, char c) {
        assert(p);
        assert(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        nếu (p->string_len >= ANSI_SEQ_STRING_MAX)
                trả_về 0; /* too long */

        nếu (p->capture) {
                nếu (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        trả_về -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        trả_về 1; /* added */
}

static số_nguyên ansi_seq_parser_bad(AnsiSeqParser *p) {
        assert(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        trả_về ANSI_SEQ_EVENT_ABORT;
}

số_nguyên ansi_seq_parser_feed(AnsiSeqParser *p, char c) {
        uint8_t u = (uint8_t) c;
        số_nguyên r;

        assert(p);

        switch (p->state) {

        case ANSI_SEQ_STATE_GROUND:
                nếu (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        trả_về ANSI_SEQ_EVENT_SEQUENCE;
                }

                trả_về ANSI_SEQ_EVENT_TEXT;

        case ANSI_SEQ_STATE_ESC:
                switch (c) {

                case '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        trả_về ANSI_SEQ_EVENT_SEQUENCE;

                case ']': /* OSC */
                case 'P': /* DCS */
                case 'X': /* SOS */
                case '^': /* PM */
                case '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        trả_về ANSI_SEQ_EVENT_SEQUENCE;

                case 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                case 0x51 ... 0x57:
                case 0x59 ... 0x5a:
                case 0x5c:
                case 0x60 ... 0x7e: /* Fs sequence */
                case 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        trả_về ANSI_SEQ_EVENT_END;

                case 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        trả_về ANSI_SEQ_EVENT_SEQUENCE;

                default:
                        /* Anything else is not a valid escape sequence, give up on it */
                        trả_về ansi_seq_parser_bad(p);
                }

        case ANSI_SEQ_STATE_CSI:
                nếu (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_push_string(p, c);
                        nếu (r < 0)
                                trả_về r;
                        nếu (r == 0)
                                trả_về ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        trả_về ANSI_SEQ_EVENT_END;
                }

                nếu (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_push_string(p, c);
                        nếu (r < 0)
                                trả_về r;
                        nếu (r > 0)
                                trả_về ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                trả_về ansi_seq_parser_bad(p);

        case ANSI_SEQ_STATE_STRING:
                nếu (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        trả_về ANSI_SEQ_EVENT_END;
                }

                nếu (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        trả_về ANSI_SEQ_EVENT_SEQUENCE;
                }

                nếu (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_push_string(p, c);
                        nếu (r < 0)
                                trả_về r;
                        nếu (r > 0)
                                trả_về ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                trả_về ansi_seq_parser_bad(p);

        case ANSI_SEQ_STATE_STRING_ESC:
                nếu (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        trả_về ANSI_SEQ_EVENT_END;
                }

                trả_về ansi_seq_parser_bad(p);

        case ANSI_SEQ_STATE_NF:
                nếu (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_push_string(p, c);
                        nếu (r < 0)
                                trả_về r;
                        nếu (r == 0)
                                trả_về ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        trả_về ANSI_SEQ_EVENT_END;
                }

                nếu (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_push_string(p, c);
                        nếu (r < 0)
                                trả_về r;
                        nếu (r > 0)
                                trả_về ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                trả_về ansi_seq_parser_bad(p);

        default:
                assert_not_reached();
        }
}

số_nguyên ansi_seq_parser_feed_harder(AnsiSeqParser *p, char c) {
        số_nguyên r;

        assert(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        do {
                r = ansi_seq_parser_feed(p, c);
        } trong_khi (r == ANSI_SEQ_EVENT_ABORT);

        trả_về r;
}

const char* ansi_seq_parser_string(const AnsiSeqParser *p) {
        assert(p);

        nếu (!p->capture || p->bad || p->introducer == 0)
                trả_về NULL;

        trả_về strempty(p->string);
}
