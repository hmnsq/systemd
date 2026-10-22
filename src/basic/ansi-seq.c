/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-seq.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಶೂನ್ಯ ansi_seq_parser_done(AnsiSeqParser *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ ansi_seq_parser_begin_string(AnsiSeqParser *p, ವರ್ಣ introducer) {
        ಪ್ರತಿಜ್ಞಾ(p);

        p->string_len = 0;
        ಯದಿ (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ansi_seq_parser_ತಳ್ಳು_string(AnsiSeqParser *p, ವರ್ಣ c) {
        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        ಯದಿ (p->string_len >= ANSI_SEQ_STRING_MAX)
                ಪ್ರತ್ಯಾಗಮ 0; /* too long */

        ಯದಿ (p->capture) {
                ಯದಿ (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        ಪ್ರತ್ಯಾಗಮ 1; /* added */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ansi_seq_parser_bad(AnsiSeqParser *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_ABORT;
}

ಪೂರ್ಣಾಙ್ಕ ansi_seq_parser_feed(AnsiSeqParser *p, ವರ್ಣ c) {
        uint8_t u = (uint8_t) c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        ವಿಕಲ್ಪ (p->state) {

        ಪಕ್ಷ ANSI_SEQ_STATE_GROUND:
                ಯದಿ (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_TEXT;

        ಪಕ್ಷ ANSI_SEQ_STATE_ESC:
                ವಿಕಲ್ಪ (c) {

                ಪಕ್ಷ '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;

                ಪಕ್ಷ ']': /* OSC */
                ಪಕ್ಷ 'P': /* DCS */
                ಪಕ್ಷ 'X': /* SOS */
                ಪಕ್ಷ '^': /* PM */
                ಪಕ್ಷ '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;

                ಪಕ್ಷ 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                ಪಕ್ಷ 0x51 ... 0x57:
                ಪಕ್ಷ 0x59 ... 0x5a:
                ಪಕ್ಷ 0x5c:
                ಪಕ್ಷ 0x60 ... 0x7e: /* Fs sequence */
                ಪಕ್ಷ 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_END;

                ಪಕ್ಷ 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;

                ಸ್ವಭಾವ:
                        /* Anything else is not a valid escape sequence, give up on it */
                        ಪ್ರತ್ಯಾಗಮ ansi_seq_parser_bad(p);
                }

        ಪಕ್ಷ ANSI_SEQ_STATE_CSI:
                ಯದಿ (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_END;
                }

                ಯದಿ (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r > 0)
                                ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ಪ್ರತ್ಯಾಗಮ ansi_seq_parser_bad(p);

        ಪಕ್ಷ ANSI_SEQ_STATE_STRING:
                ಯದಿ (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_END;
                }

                ಯದಿ (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ಯದಿ (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r > 0)
                                ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                ಪ್ರತ್ಯಾಗಮ ansi_seq_parser_bad(p);

        ಪಕ್ಷ ANSI_SEQ_STATE_STRING_ESC:
                ಯದಿ (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_END;
                }

                ಪ್ರತ್ಯಾಗಮ ansi_seq_parser_bad(p);

        ಪಕ್ಷ ANSI_SEQ_STATE_NF:
                ಯದಿ (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_END;
                }

                ಯದಿ (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r > 0)
                                ಪ್ರತ್ಯಾಗಮ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ಪ್ರತ್ಯಾಗಮ ansi_seq_parser_bad(p);

        ಸ್ವಭಾವ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಙ್ಕ ansi_seq_parser_feed_harder(AnsiSeqParser *p, ವರ್ಣ c) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        ಕುರು {
                r = ansi_seq_parser_feed(p, c);
        } ಯಾವತ್ (r == ANSI_SEQ_EVENT_ABORT);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಧ್ರುವ ವರ್ಣ* ansi_seq_parser_string(ಧ್ರುವ AnsiSeqParser *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        ಯದಿ (!p->capture || p->bad || p->introducer == 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ strempty(p->string);
}
