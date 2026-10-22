/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-seq.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಶೂನ್ಯ ansi_seq_parser_ಮುಗಿದ(AnsiSeqParser *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ ansi_seq_parser_begin_string(AnsiSeqParser *p, ಅಕ್ಷರ introducer) {
        ಪ್ರತಿಜ್ಞಾ(p);

        p->string_len = 0;
        ಆದರೆ (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ansi_seq_parser_ತಳ್ಳು_string(AnsiSeqParser *p, ಅಕ್ಷರ c) {
        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        ಆದರೆ (p->string_len >= ANSI_SEQ_STRING_MAX)
                ಹಿಂತಿರುಗಿ 0; /* too long */

        ಆದರೆ (p->capture) {
                ಆದರೆ (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        ಹಿಂತಿರುಗಿ 1; /* added */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ansi_seq_parser_bad(AnsiSeqParser *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_ABORT;
}

ಪೂರ್ಣಾಂಕ ansi_seq_parser_feed(AnsiSeqParser *p, ಅಕ್ಷರ c) {
        uint8_t u = (uint8_t) c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        ಆಯ್ಕೆ (p->state) {

        ಸ್ಥಿತಿ ANSI_SEQ_STATE_GROUND:
                ಆದರೆ (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_ಪಠ್ಯ;

        ಸ್ಥಿತಿ ANSI_SEQ_STATE_ESC:
                ಆಯ್ಕೆ (c) {

                ಸ್ಥಿತಿ '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;

                ಸ್ಥಿತಿ ']': /* OSC */
                ಸ್ಥಿತಿ 'P': /* DCS */
                ಸ್ಥಿತಿ 'X': /* SOS */
                ಸ್ಥಿತಿ '^': /* PM */
                ಸ್ಥಿತಿ '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;

                ಸ್ಥಿತಿ 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                ಸ್ಥಿತಿ 0x51 ... 0x57:
                ಸ್ಥಿತಿ 0x59 ... 0x5a:
                ಸ್ಥಿತಿ 0x5c:
                ಸ್ಥಿತಿ 0x60 ... 0x7e: /* Fs sequence */
                ಸ್ಥಿತಿ 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_ಕೊನೆ;

                ಸ್ಥಿತಿ 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;

                ಪೂರ್ವನಿಯೋಜಿತ:
                        /* Anything else is not a valid escape sequence, give up on it */
                        ಹಿಂತಿರುಗಿ ansi_seq_parser_bad(p);
                }

        ಸ್ಥಿತಿ ANSI_SEQ_STATE_CSI:
                ಆದರೆ (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_ಕೊನೆ;
                }

                ಆದರೆ (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r > 0)
                                ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ಹಿಂತಿರುಗಿ ansi_seq_parser_bad(p);

        ಸ್ಥಿತಿ ANSI_SEQ_STATE_STRING:
                ಆದರೆ (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_ಕೊನೆ;
                }

                ಆದರೆ (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ಆದರೆ (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r > 0)
                                ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                ಹಿಂತಿರುಗಿ ansi_seq_parser_bad(p);

        ಸ್ಥಿತಿ ANSI_SEQ_STATE_STRING_ESC:
                ಆದರೆ (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_ಕೊನೆ;
                }

                ಹಿಂತಿರುಗಿ ansi_seq_parser_bad(p);

        ಸ್ಥಿತಿ ANSI_SEQ_STATE_NF:
                ಆದರೆ (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_ಕೊನೆ;
                }

                ಆದರೆ (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ತಳ್ಳು_string(p, c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r > 0)
                                ಹಿಂತಿರುಗಿ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ಹಿಂತಿರುಗಿ ansi_seq_parser_bad(p);

        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಂಕ ansi_seq_parser_feed_harder(AnsiSeqParser *p, ಅಕ್ಷರ c) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        ಮಾಡು {
                r = ansi_seq_parser_feed(p, c);
        } ಆಗ (r == ANSI_SEQ_EVENT_ABORT);

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_seq_parser_string(ಸ್ಥಿರಾಂಕ AnsiSeqParser *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        ಆದರೆ (!p->capture || p->bad || p->introducer == 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ strempty(p->string);
}
