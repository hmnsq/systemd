/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "ansi-seq.h"
#ضمن "string-util.h"

فراغ ansi_seq_parser_done(AnsiSeqParser *p) {
        أكد(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

ساكن فراغ ansi_seq_parser_begin_string(AnsiSeqParser *p, حرف introducer) {
        أكد(p);

        p->string_len = 0;
        ܐܢ (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

ساكن صحيح ansi_seq_parser_push_string(AnsiSeqParser *p, حرف c) {
        أكد(p);
        أكد(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        ܐܢ (p->string_len >= ANSI_SEQ_STRING_MAX)
                ܦܢܐ 0; /* too long */

        ܐܢ (p->capture) {
                ܐܢ (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        ܦܢܐ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        ܦܢܐ 1; /* added */
}

ساكن صحيح ansi_seq_parser_bad(AnsiSeqParser *p) {
        أكد(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        ܦܢܐ ANSI_SEQ_EVENT_ABORT;
}

صحيح ansi_seq_parser_feed(AnsiSeqParser *p, حرف c) {
        uint8_t u = (uint8_t) c;
        صحيح r;

        أكد(p);

        بدل (p->state) {

        حالة ANSI_SEQ_STATE_GROUND:
                ܐܢ (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ܦܢܐ ANSI_SEQ_EVENT_TEXT;

        حالة ANSI_SEQ_STATE_ESC:
                بدل (c) {

                حالة '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;

                حالة ']': /* OSC */
                حالة 'P': /* DCS */
                حالة 'X': /* SOS */
                حالة '^': /* PM */
                حالة '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;

                حالة 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                حالة 0x51 ... 0x57:
                حالة 0x59 ... 0x5a:
                حالة 0x5c:
                حالة 0x60 ... 0x7e: /* Fs sequence */
                حالة 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        ܦܢܐ ANSI_SEQ_EVENT_END;

                حالة 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;

                افتراضي:
                        /* Anything else is not a valid escape sequence, give up on it */
                        ܦܢܐ ansi_seq_parser_bad(p);
                }

        حالة ANSI_SEQ_STATE_CSI:
                ܐܢ (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_push_string(p, c);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                        ܐܢ (r == 0)
                                ܦܢܐ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ܦܢܐ ANSI_SEQ_EVENT_END;
                }

                ܐܢ (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_push_string(p, c);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                        ܐܢ (r > 0)
                                ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ܦܢܐ ansi_seq_parser_bad(p);

        حالة ANSI_SEQ_STATE_STRING:
                ܐܢ (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ܦܢܐ ANSI_SEQ_EVENT_END;
                }

                ܐܢ (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ܐܢ (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_push_string(p, c);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                        ܐܢ (r > 0)
                                ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                ܦܢܐ ansi_seq_parser_bad(p);

        حالة ANSI_SEQ_STATE_STRING_ESC:
                ܐܢ (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ܦܢܐ ANSI_SEQ_EVENT_END;
                }

                ܦܢܐ ansi_seq_parser_bad(p);

        حالة ANSI_SEQ_STATE_NF:
                ܐܢ (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_push_string(p, c);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                        ܐܢ (r == 0)
                                ܦܢܐ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ܦܢܐ ANSI_SEQ_EVENT_END;
                }

                ܐܢ (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_push_string(p, c);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                        ܐܢ (r > 0)
                                ܦܢܐ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ܦܢܐ ansi_seq_parser_bad(p);

        افتراضي:
                assert_not_reached();
        }
}

صحيح ansi_seq_parser_feed_harder(AnsiSeqParser *p, حرف c) {
        صحيح r;

        أكد(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        ܥܒܕ {
                r = ansi_seq_parser_feed(p, c);
        } ܟܡܐ (r == ANSI_SEQ_EVENT_ABORT);

        ܦܢܐ r;
}

ثابت حرف* ansi_seq_parser_string(ثابت AnsiSeqParser *p) {
        أكد(p);

        ܐܢ (!p->capture || p->bad || p->introducer == 0)
                ܦܢܐ عدم;

        ܦܢܐ strempty(p->string);
}
