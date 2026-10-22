/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "ansi-seq.h"
#अन्तर्भाव "string-util.h"

शून्य ansi_seq_parser_done(AnsiSeqParser *p) {
        प्रतिज्ञा(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

स्थिर शून्य ansi_seq_parser_begin_string(AnsiSeqParser *p, अक्षर introducer) {
        प्रतिज्ञा(p);

        p->string_len = 0;
        जर (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

स्थिर पूर्णांक ansi_seq_parser_ढकला_string(AnsiSeqParser *p, अक्षर c) {
        प्रतिज्ञा(p);
        प्रतिज्ञा(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        जर (p->string_len >= ANSI_SEQ_STRING_MAX)
                परत 0; /* too long */

        जर (p->capture) {
                जर (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        परत -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        परत 1; /* added */
}

स्थिर पूर्णांक ansi_seq_parser_bad(AnsiSeqParser *p) {
        प्रतिज्ञा(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        परत ANSI_SEQ_EVENT_ABORT;
}

पूर्णांक ansi_seq_parser_feed(AnsiSeqParser *p, अक्षर c) {
        uint8_t u = (uint8_t) c;
        पूर्णांक r;

        प्रतिज्ञा(p);

        निवड (p->state) {

        स्थिती ANSI_SEQ_STATE_GROUND:
                जर (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        परत ANSI_SEQ_EVENT_SEQUENCE;
                }

                परत ANSI_SEQ_EVENT_TEXT;

        स्थिती ANSI_SEQ_STATE_ESC:
                निवड (c) {

                स्थिती '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        परत ANSI_SEQ_EVENT_SEQUENCE;

                स्थिती ']': /* OSC */
                स्थिती 'P': /* DCS */
                स्थिती 'X': /* SOS */
                स्थिती '^': /* PM */
                स्थिती '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        परत ANSI_SEQ_EVENT_SEQUENCE;

                स्थिती 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                स्थिती 0x51 ... 0x57:
                स्थिती 0x59 ... 0x5a:
                स्थिती 0x5c:
                स्थिती 0x60 ... 0x7e: /* Fs sequence */
                स्थिती 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        परत ANSI_SEQ_EVENT_END;

                स्थिती 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        परत ANSI_SEQ_EVENT_SEQUENCE;

                पूर्वनिर्धारित:
                        /* Anything else is not a valid escape sequence, give up on it */
                        परत ansi_seq_parser_bad(p);
                }

        स्थिती ANSI_SEQ_STATE_CSI:
                जर (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ढकला_string(p, c);
                        जर (r < 0)
                                परत r;
                        जर (r == 0)
                                परत ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        परत ANSI_SEQ_EVENT_END;
                }

                जर (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ढकला_string(p, c);
                        जर (r < 0)
                                परत r;
                        जर (r > 0)
                                परत ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                परत ansi_seq_parser_bad(p);

        स्थिती ANSI_SEQ_STATE_STRING:
                जर (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        परत ANSI_SEQ_EVENT_END;
                }

                जर (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        परत ANSI_SEQ_EVENT_SEQUENCE;
                }

                जर (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_ढकला_string(p, c);
                        जर (r < 0)
                                परत r;
                        जर (r > 0)
                                परत ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                परत ansi_seq_parser_bad(p);

        स्थिती ANSI_SEQ_STATE_STRING_ESC:
                जर (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        परत ANSI_SEQ_EVENT_END;
                }

                परत ansi_seq_parser_bad(p);

        स्थिती ANSI_SEQ_STATE_NF:
                जर (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ढकला_string(p, c);
                        जर (r < 0)
                                परत r;
                        जर (r == 0)
                                परत ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        परत ANSI_SEQ_EVENT_END;
                }

                जर (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ढकला_string(p, c);
                        जर (r < 0)
                                परत r;
                        जर (r > 0)
                                परत ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                परत ansi_seq_parser_bad(p);

        पूर्वनिर्धारित:
                assert_not_reached();
        }
}

पूर्णांक ansi_seq_parser_feed_harder(AnsiSeqParser *p, अक्षर c) {
        पूर्णांक r;

        प्रतिज्ञा(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        करा {
                r = ansi_seq_parser_feed(p, c);
        } पर्यंत (r == ANSI_SEQ_EVENT_ABORT);

        परत r;
}

स्थिरांक अक्षर* ansi_seq_parser_string(स्थिरांक AnsiSeqParser *p) {
        प्रतिज्ञा(p);

        जर (!p->capture || p->bad || p->introducer == 0)
                परत रिक्त;

        परत strempty(p->string);
}
