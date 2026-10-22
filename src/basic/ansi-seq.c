/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "ansi-seq.h"
#અન્તર્ભાવ "string-util.h"

શૂન્ય ansi_seq_parser_done(AnsiSeqParser *p) {
        પ્રતિજ્ઞા(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

સ્થિર શૂન્ય ansi_seq_parser_begin_string(AnsiSeqParser *p, અક્ષર introducer) {
        પ્રતિજ્ઞા(p);

        p->string_len = 0;
        જો (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

સ્થિર પૂર્ણાંક ansi_seq_parser_ધકેલો_string(AnsiSeqParser *p, અક્ષર c) {
        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        જો (p->string_len >= ANSI_SEQ_STRING_MAX)
                પાછા 0; /* too long */

        જો (p->capture) {
                જો (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        પાછા -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        પાછા 1; /* added */
}

સ્થિર પૂર્ણાંક ansi_seq_parser_bad(AnsiSeqParser *p) {
        પ્રતિજ્ઞા(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        પાછા ANSI_SEQ_EVENT_ABORT;
}

પૂર્ણાંક ansi_seq_parser_feed(AnsiSeqParser *p, અક્ષર c) {
        uint8_t u = (uint8_t) c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);

        પસંદગી (p->state) {

        સ્થિતિ ANSI_SEQ_STATE_GROUND:
                જો (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        પાછા ANSI_SEQ_EVENT_SEQUENCE;
                }

                પાછા ANSI_SEQ_EVENT_TEXT;

        સ્થિતિ ANSI_SEQ_STATE_ESC:
                પસંદગી (c) {

                સ્થિતિ '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        પાછા ANSI_SEQ_EVENT_SEQUENCE;

                સ્થિતિ ']': /* OSC */
                સ્થિતિ 'P': /* DCS */
                સ્થિતિ 'X': /* SOS */
                સ્થિતિ '^': /* PM */
                સ્થિતિ '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        પાછા ANSI_SEQ_EVENT_SEQUENCE;

                સ્થિતિ 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                સ્થિતિ 0x51 ... 0x57:
                સ્થિતિ 0x59 ... 0x5a:
                સ્થિતિ 0x5c:
                સ્થિતિ 0x60 ... 0x7e: /* Fs sequence */
                સ્થિતિ 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        પાછા ANSI_SEQ_EVENT_END;

                સ્થિતિ 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        પાછા ANSI_SEQ_EVENT_SEQUENCE;

                મૂળભૂત:
                        /* Anything else is not a valid escape sequence, give up on it */
                        પાછા ansi_seq_parser_bad(p);
                }

        સ્થિતિ ANSI_SEQ_STATE_CSI:
                જો (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ધકેલો_string(p, c);
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        પાછા ANSI_SEQ_EVENT_END;
                }

                જો (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ધકેલો_string(p, c);
                        જો (r < 0)
                                પાછા r;
                        જો (r > 0)
                                પાછા ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                પાછા ansi_seq_parser_bad(p);

        સ્થિતિ ANSI_SEQ_STATE_STRING:
                જો (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        પાછા ANSI_SEQ_EVENT_END;
                }

                જો (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        પાછા ANSI_SEQ_EVENT_SEQUENCE;
                }

                જો (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_ધકેલો_string(p, c);
                        જો (r < 0)
                                પાછા r;
                        જો (r > 0)
                                પાછા ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                પાછા ansi_seq_parser_bad(p);

        સ્થિતિ ANSI_SEQ_STATE_STRING_ESC:
                જો (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        પાછા ANSI_SEQ_EVENT_END;
                }

                પાછા ansi_seq_parser_bad(p);

        સ્થિતિ ANSI_SEQ_STATE_NF:
                જો (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ધકેલો_string(p, c);
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        પાછા ANSI_SEQ_EVENT_END;
                }

                જો (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ધકેલો_string(p, c);
                        જો (r < 0)
                                પાછા r;
                        જો (r > 0)
                                પાછા ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                પાછા ansi_seq_parser_bad(p);

        મૂળભૂત:
                assert_not_reached();
        }
}

પૂર્ણાંક ansi_seq_parser_feed_harder(AnsiSeqParser *p, અક્ષર c) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        કરો {
                r = ansi_seq_parser_feed(p, c);
        } જ્યારે (r == ANSI_SEQ_EVENT_ABORT);

        પાછા r;
}

અચલ અક્ષર* ansi_seq_parser_string(અચલ AnsiSeqParser *p) {
        પ્રતિજ્ઞા(p);

        જો (!p->capture || p->bad || p->introducer == 0)
                પાછા ખાલી;

        પાછા strempty(p->string);
}
