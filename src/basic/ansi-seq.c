/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "ansi-seq.h"
#అన్తర్భావ "string-util.h"

శూన్య ansi_seq_parser_పూర్తి(AnsiSeqParser *p) {
        ప్రతిజ్ఞా(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

స్థిర శూన్య ansi_seq_parser_begin_string(AnsiSeqParser *p, అక్షరం introducer) {
        ప్రతిజ్ఞా(p);

        p->string_len = 0;
        ఒకవేళ (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

స్థిర పూర్ణాంకం ansi_seq_parser_నెట్టు_string(AnsiSeqParser *p, అక్షరం c) {
        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        ఒకవేళ (p->string_len >= ANSI_SEQ_STRING_MAX)
                తిరిగి 0; /* too long */

        ఒకవేళ (p->capture) {
                ఒకవేళ (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        తిరిగి -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        తిరిగి 1; /* added */
}

స్థిర పూర్ణాంకం ansi_seq_parser_bad(AnsiSeqParser *p) {
        ప్రతిజ్ఞా(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        తిరిగి ANSI_SEQ_EVENT_ABORT;
}

పూర్ణాంకం ansi_seq_parser_feed(AnsiSeqParser *p, అక్షరం c) {
        uint8_t u = (uint8_t) c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);

        ఎంపిక (p->state) {

        స్థితి ANSI_SEQ_STATE_GROUND:
                ఒకవేళ (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        తిరిగి ANSI_SEQ_EVENT_SEQUENCE;
                }

                తిరిగి ANSI_SEQ_EVENT_పాఠ్యం;

        స్థితి ANSI_SEQ_STATE_ESC:
                ఎంపిక (c) {

                స్థితి '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        తిరిగి ANSI_SEQ_EVENT_SEQUENCE;

                స్థితి ']': /* OSC */
                స్థితి 'P': /* DCS */
                స్థితి 'X': /* SOS */
                స్థితి '^': /* PM */
                స్థితి '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        తిరిగి ANSI_SEQ_EVENT_SEQUENCE;

                స్థితి 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                స్థితి 0x51 ... 0x57:
                స్థితి 0x59 ... 0x5a:
                స్థితి 0x5c:
                స్థితి 0x60 ... 0x7e: /* Fs sequence */
                స్థితి 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        తిరిగి ANSI_SEQ_EVENT_చివర;

                స్థితి 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        తిరిగి ANSI_SEQ_EVENT_SEQUENCE;

                అప్రమేయ:
                        /* Anything else is not a valid escape sequence, give up on it */
                        తిరిగి ansi_seq_parser_bad(p);
                }

        స్థితి ANSI_SEQ_STATE_CSI:
                ఒకవేళ (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_నెట్టు_string(p, c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        తిరిగి ANSI_SEQ_EVENT_చివర;
                }

                ఒకవేళ (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_నెట్టు_string(p, c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r > 0)
                                తిరిగి ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                తిరిగి ansi_seq_parser_bad(p);

        స్థితి ANSI_SEQ_STATE_STRING:
                ఒకవేళ (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        తిరిగి ANSI_SEQ_EVENT_చివర;
                }

                ఒకవేళ (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        తిరిగి ANSI_SEQ_EVENT_SEQUENCE;
                }

                ఒకవేళ (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_నెట్టు_string(p, c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r > 0)
                                తిరిగి ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                తిరిగి ansi_seq_parser_bad(p);

        స్థితి ANSI_SEQ_STATE_STRING_ESC:
                ఒకవేళ (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        తిరిగి ANSI_SEQ_EVENT_చివర;
                }

                తిరిగి ansi_seq_parser_bad(p);

        స్థితి ANSI_SEQ_STATE_NF:
                ఒకవేళ (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_నెట్టు_string(p, c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        తిరిగి ANSI_SEQ_EVENT_చివర;
                }

                ఒకవేళ (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_నెట్టు_string(p, c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r > 0)
                                తిరిగి ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                తిరిగి ansi_seq_parser_bad(p);

        అప్రమేయ:
                assert_not_reached();
        }
}

పూర్ణాంకం ansi_seq_parser_feed_harder(AnsiSeqParser *p, అక్షరం c) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        చేయి {
                r = ansi_seq_parser_feed(p, c);
        } అయితే (r == ANSI_SEQ_EVENT_ABORT);

        తిరిగి r;
}

స్థిరరాశి అక్షరం* ansi_seq_parser_string(స్థిరరాశి AnsiSeqParser *p) {
        ప్రతిజ్ఞా(p);

        ఒకవేళ (!p->capture || p->bad || p->introducer == 0)
                తిరిగి ఖాళీ;

        తిరిగి strempty(p->string);
}
