/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "ansi-seq.h"
#समावेश "string-util.h"

व्योम ansi_seq_parser_done(AnsiSeqParser *p) {
        निश्चित(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

अटल व्योम ansi_seq_parser_begin_string(AnsiSeqParser *p, अक्षर introducer) {
        निश्चित(p);

        p->string_len = 0;
        अगर (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

अटल पूर्णांक ansi_seq_parser_धकेलो_string(AnsiSeqParser *p, अक्षर c) {
        निश्चित(p);
        निश्चित(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        अगर (p->string_len >= ANSI_SEQ_STRING_MAX)
                वापस 0; /* too long */

        अगर (p->capture) {
                अगर (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        वापस -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        वापस 1; /* added */
}

अटल पूर्णांक ansi_seq_parser_bad(AnsiSeqParser *p) {
        निश्चित(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        वापस ANSI_SEQ_EVENT_ABORT;
}

पूर्णांक ansi_seq_parser_feed(AnsiSeqParser *p, अक्षर c) {
        uint8_t u = (uint8_t) c;
        पूर्णांक r;

        निश्चित(p);

        चयन (p->state) {

        हाल ANSI_SEQ_STATE_GROUND:
                अगर (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        वापस ANSI_SEQ_EVENT_SEQUENCE;
                }

                वापस ANSI_SEQ_EVENT_TEXT;

        हाल ANSI_SEQ_STATE_ESC:
                चयन (c) {

                हाल '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        वापस ANSI_SEQ_EVENT_SEQUENCE;

                हाल ']': /* OSC */
                हाल 'P': /* DCS */
                हाल 'X': /* SOS */
                हाल '^': /* PM */
                हाल '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        वापस ANSI_SEQ_EVENT_SEQUENCE;

                हाल 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                हाल 0x51 ... 0x57:
                हाल 0x59 ... 0x5a:
                हाल 0x5c:
                हाल 0x60 ... 0x7e: /* Fs sequence */
                हाल 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        वापस ANSI_SEQ_EVENT_END;

                हाल 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        वापस ANSI_SEQ_EVENT_SEQUENCE;

                शेष:
                        /* Anything else is not a valid escape sequence, give up on it */
                        वापस ansi_seq_parser_bad(p);
                }

        हाल ANSI_SEQ_STATE_CSI:
                अगर (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_धकेलो_string(p, c);
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        वापस ANSI_SEQ_EVENT_END;
                }

                अगर (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_धकेलो_string(p, c);
                        अगर (r < 0)
                                वापस r;
                        अगर (r > 0)
                                वापस ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                वापस ansi_seq_parser_bad(p);

        हाल ANSI_SEQ_STATE_STRING:
                अगर (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        वापस ANSI_SEQ_EVENT_END;
                }

                अगर (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        वापस ANSI_SEQ_EVENT_SEQUENCE;
                }

                अगर (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_धकेलो_string(p, c);
                        अगर (r < 0)
                                वापस r;
                        अगर (r > 0)
                                वापस ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                वापस ansi_seq_parser_bad(p);

        हाल ANSI_SEQ_STATE_STRING_ESC:
                अगर (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        वापस ANSI_SEQ_EVENT_END;
                }

                वापस ansi_seq_parser_bad(p);

        हाल ANSI_SEQ_STATE_NF:
                अगर (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_धकेलो_string(p, c);
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        वापस ANSI_SEQ_EVENT_END;
                }

                अगर (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_धकेलो_string(p, c);
                        अगर (r < 0)
                                वापस r;
                        अगर (r > 0)
                                वापस ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                वापस ansi_seq_parser_bad(p);

        शेष:
                assert_not_reached();
        }
}

पूर्णांक ansi_seq_parser_feed_harder(AnsiSeqParser *p, अक्षर c) {
        पूर्णांक r;

        निश्चित(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        करो {
                r = ansi_seq_parser_feed(p, c);
        } जबतक (r == ANSI_SEQ_EVENT_ABORT);

        वापस r;
}

स्थिर अक्षर* ansi_seq_parser_string(स्थिर AnsiSeqParser *p) {
        निश्चित(p);

        अगर (!p->capture || p->bad || p->introducer == 0)
                वापस शून्य;

        वापस strempty(p->string);
}
