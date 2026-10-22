/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "ansi-seq.h"
#अन्तर्भाव "string-util.h"

शून्य ansi_seq_parser_done(AnsiSeqParser *p) {
        प्रतिज्ञा(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

स्थिर शून्य ansi_seq_parser_begin_string(AnsiSeqParser *p, वर्ण introducer) {
        प्रतिज्ञा(p);

        p->string_len = 0;
        यदि (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

स्थिर पूर्णाङ्क ansi_seq_parser_थप_string(AnsiSeqParser *p, वर्ण c) {
        प्रतिज्ञा(p);
        प्रतिज्ञा(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        यदि (p->string_len >= ANSI_SEQ_STRING_MAX)
                फर्काऊ 0; /* too long */

        यदि (p->capture) {
                यदि (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        फर्काऊ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        फर्काऊ 1; /* added */
}

स्थिर पूर्णाङ्क ansi_seq_parser_bad(AnsiSeqParser *p) {
        प्रतिज्ञा(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        फर्काऊ ANSI_SEQ_EVENT_ABORT;
}

पूर्णाङ्क ansi_seq_parser_feed(AnsiSeqParser *p, वर्ण c) {
        uint8_t u = (uint8_t) c;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);

        विकल्प (p->state) {

        पक्ष ANSI_SEQ_STATE_GROUND:
                यदि (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;
                }

                फर्काऊ ANSI_SEQ_EVENT_TEXT;

        पक्ष ANSI_SEQ_STATE_ESC:
                विकल्प (c) {

                पक्ष '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;

                पक्ष ']': /* OSC */
                पक्ष 'P': /* DCS */
                पक्ष 'X': /* SOS */
                पक्ष '^': /* PM */
                पक्ष '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;

                पक्ष 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                पक्ष 0x51 ... 0x57:
                पक्ष 0x59 ... 0x5a:
                पक्ष 0x5c:
                पक्ष 0x60 ... 0x7e: /* Fs sequence */
                पक्ष 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        फर्काऊ ANSI_SEQ_EVENT_END;

                पक्ष 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;

                स्वभाव:
                        /* Anything else is not a valid escape sequence, give up on it */
                        फर्काऊ ansi_seq_parser_bad(p);
                }

        पक्ष ANSI_SEQ_STATE_CSI:
                यदि (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_थप_string(p, c);
                        यदि (r < 0)
                                फर्काऊ r;
                        यदि (r == 0)
                                फर्काऊ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        फर्काऊ ANSI_SEQ_EVENT_END;
                }

                यदि (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_थप_string(p, c);
                        यदि (r < 0)
                                फर्काऊ r;
                        यदि (r > 0)
                                फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                फर्काऊ ansi_seq_parser_bad(p);

        पक्ष ANSI_SEQ_STATE_STRING:
                यदि (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        फर्काऊ ANSI_SEQ_EVENT_END;
                }

                यदि (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;
                }

                यदि (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_थप_string(p, c);
                        यदि (r < 0)
                                फर्काऊ r;
                        यदि (r > 0)
                                फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                फर्काऊ ansi_seq_parser_bad(p);

        पक्ष ANSI_SEQ_STATE_STRING_ESC:
                यदि (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        फर्काऊ ANSI_SEQ_EVENT_END;
                }

                फर्काऊ ansi_seq_parser_bad(p);

        पक्ष ANSI_SEQ_STATE_NF:
                यदि (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_थप_string(p, c);
                        यदि (r < 0)
                                फर्काऊ r;
                        यदि (r == 0)
                                फर्काऊ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        फर्काऊ ANSI_SEQ_EVENT_END;
                }

                यदि (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_थप_string(p, c);
                        यदि (r < 0)
                                फर्काऊ r;
                        यदि (r > 0)
                                फर्काऊ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                फर्काऊ ansi_seq_parser_bad(p);

        स्वभाव:
                assert_not_reached();
        }
}

पूर्णाङ्क ansi_seq_parser_feed_harder(AnsiSeqParser *p, वर्ण c) {
        पूर्णाङ्क r;

        प्रतिज्ञा(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        गर {
                r = ansi_seq_parser_feed(p, c);
        } जबसम्म (r == ANSI_SEQ_EVENT_ABORT);

        फर्काऊ r;
}

अचर वर्ण* ansi_seq_parser_string(अचर AnsiSeqParser *p) {
        प्रतिज्ञा(p);

        यदि (!p->capture || p->bad || p->introducer == 0)
                फर्काऊ रिक्त;

        फर्काऊ strempty(p->string);
}
